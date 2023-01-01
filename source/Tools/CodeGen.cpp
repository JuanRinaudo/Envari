#include <thread>
#include <iostream>
#include <fstream>
#include <queue>
#include <filesystem>
#include <string.h>

#define Log printf

#include <Defines.h>
#include <MemoryStructs.h>
#include <Memory.h>

#include <File.h>
#include <Data.h>

using namespace std;

enum EntryType {
    EntryType_UNKNOWN,
    EntryType_FOLDER,
    EntryType_ENVARI_TABLE,
    EntryType_SHADER,
};

struct DataEntry
{
    char* fullPath;
    char* mapKey;
    char* name;
    char* path;
    EntryType type;
};

static MemoryArena stringArena;
static MemoryArena dataArena;

static i32 dataCount = 0;

static void FixFilePath(char* string)
{
    while(*string)
    {
        char singleChar = *string;
        if(singleChar == '\\') {
            singleChar = '/';
        }
        *string = singleChar;
        string++;
    }
}

static i32 PathNameOffset(char* string)
{
    i32 index = 0;
    i32 lastIndex = 0;
    while(*string)
    {
        if(*string == filesystem::path::preferred_separator && *(string + 1)) {
            lastIndex = index + 1;
        }
        string++;
        index++;
    }
    return lastIndex;
}

static void Write(ofstream* stream, const char* string)
{
    stream->write(string, strlen(string));
}

static void WriteLine(ofstream* stream, const char* string)
{
    stream->write(string, strlen(string));
    stream->write("\n", 1);
}

static void WriteConstant(ofstream* stream, const char* key, const char* value, const char* valuePostfix = "")
{
    Write(stream, "const char* ");
    Write(stream, key);
    Write(stream, " = \"");
    Write(stream, value);
    Write(stream, valuePostfix);
    Write(stream, "\";\n");
}

static void WriteScriptingConstant(ofstream* stream, const char* key, const char* value, const char* valuePostfix = "")
{
    Write(stream, key);
    Write(stream, " = \"");
    Write(stream, value);
    Write(stream, valuePostfix);
    Write(stream, "\";\n");
}

static void StartFile(ofstream* stream, const char* folderPath, const char* filepath)
{
    char* codegenPath = PushArray(&stringArena, strlen(folderPath) + strlen(filepath), char);
    strcpy(codegenPath, folderPath);
    strcat(codegenPath, filepath);

    stream->open(codegenPath, ios_base::out);
}

static void StartMapFile(ofstream* stream, const char* folderPath, const char* filepath, const char* define)
{
    char* codegenPath = PushArray(&stringArena, strlen(folderPath) + strlen(filepath), char);
    strcpy(codegenPath, folderPath);
    strcat(codegenPath, filepath);
    
    stream->open(codegenPath, ios_base::out);
    Write(stream, "#ifndef ");
    Write(stream, define);
    Write(stream, "\n#define ");
    Write(stream, define);
    Write(stream, "\n\n");
}

static void EndFile(ofstream* stream)
{
    stream->close();
}

static void EndMapFile(ofstream* stream)
{    
    Write(stream, "\n#endif");
    stream->close();
}

static EntryType GetEntryType(filesystem::directory_entry* entry, DataEntry* definition)
{
    if(entry->is_directory()) {
        return EntryType_FOLDER;
    }
    else {
        if(strstr(definition->path, ".envt")) {
            return EntryType_ENVARI_TABLE;
        }
        else if(strstr(definition->path, ".vert") || strstr(definition->path, ".frag")) {
            return EntryType_SHADER;
        }
        else {
            return EntryType_UNKNOWN;
        }
    }
}

i32 main()
{
    i32 stringMemorySize = Megabytes(32);
    i32 dataMemorySize = Megabytes(32);

    void* stringMemory = malloc(stringMemorySize);
    void* dataMemory = malloc(dataMemorySize);

    DataEntry* rootDefinitions = (DataEntry*)dataMemory;

    InitializeArena(&stringArena, stringMemorySize, (u8*)stringMemory, 0);
    InitializeArena(&dataArena, dataMemorySize, (u8*)dataMemory, 0);

    char workingDirectory[1024];
    filesystem::path workingDirectoryPath = filesystem::current_path();
    strcpy(workingDirectory, workingDirectoryPath.c_str());

    size_t workingDirectorySize = strlen(workingDirectory);

    const char* folderPath = "../CodeGen/";
    size_t folderPathSize = strlen(folderPath);
    cout << "Codegen folder path: " << folderPath << '\n';

    char scriptingPath[1024];
    strcpy(scriptingPath, workingDirectory);
    strcat(scriptingPath, "/scripts/codegen/");

    if(!filesystem::exists(folderPath)) {
        cout << "Create codegen folder" << '\n';
        filesystem::create_directory(folderPath);
    }
    
    if(!filesystem::exists(scriptingPath)) {
        cout << "Create codegen folder" << '\n';
        filesystem::create_directory(scriptingPath);
    }

    cout << "Started code generation on current working directory: " << workingDirectory << '\n';

    cout << '\n';

    ofstream foldersCodegen;
    ofstream fileCodegen;
    ofstream shaderCodegen;

    ofstream luaFoldersCodegen;
    ofstream luaFileCodegen;
    ofstream luaShaderCodegen;

    StartMapFile(&foldersCodegen, folderPath, "FolderMap.h", "FOLDERMAP_H");
    StartMapFile(&fileCodegen, folderPath, "FileMap.h", "FILEMAP_H");
    StartMapFile(&shaderCodegen, folderPath, "ShaderMap.h", "SHADERMAP_H");
    
    StartFile(&luaFoldersCodegen, scriptingPath, "FolderMap.lua");
    StartFile(&luaFileCodegen, scriptingPath, "FileMap.lua");
    StartFile(&luaShaderCodegen, scriptingPath, "ShaderMap.lua");

    cout << '\n';

    queue<filesystem::directory_entry> entryQueue;

    // #NOTE (Juan): Initial root working directory scan
    for (const auto& entry : filesystem::directory_iterator(workingDirectory)) {
        entryQueue.push(entry);
    }

    while(!entryQueue.empty()) {
        auto entry = entryQueue.front();

        DataEntry* definition = PushStruct(&dataArena, DataEntry);
        auto entryPathString = entry.path().string();
        const char* path = entryPathString.c_str();

        definition->fullPath = PushString(&stringArena, path);
        definition->path = PushString(&stringArena, path + workingDirectorySize + 1);
        FixFilePath(definition->path);
        definition->mapKey = PushString(&stringArena, path + workingDirectorySize + 1);
        FilenameToKey(definition->mapKey);
        i32 nameOffset = PathNameOffset(definition->fullPath);
        definition->name = PushString(&stringArena, path + nameOffset);
        definition->type = GetEntryType(&entry, definition);
        
        dataCount++;

        if(definition->type == EntryType_FOLDER) {
            for (const auto& entry : filesystem::directory_iterator(path)) {
                entryQueue.push(entry);
            }
        }

        entryQueue.pop();
    }

    for(i32 i = 0; i < dataCount; ++i) {
        DataEntry *definition = rootDefinitions + i;
        // #NOTE (Juan): Debug log data
        // cout << "Found: " << definition->fullPath <<
        //     " | Name: " << definition->name <<
        //     " | Path: " << definition->path <<
        //     " | Key: " << definition->mapKey <<
        //     " | Entry type: " << definition->type << '\n';

        if(definition->type == EntryType_FOLDER) {
            WriteConstant(&foldersCodegen, definition->mapKey, definition->path, "/");
            WriteScriptingConstant(&luaFoldersCodegen, definition->mapKey, definition->path, "/");
        } else {
            WriteConstant(&fileCodegen, definition->mapKey, definition->path);
            WriteScriptingConstant(&luaFileCodegen, definition->mapKey, definition->path);
        }

        if(definition->type == EntryType_SHADER) {
            char* relativePathStart = strstr(definition->path, "/core/");
            char* esPathStart = strstr(definition->path, "/es/");
            if(relativePathStart != 0 && esPathStart == 0) {
                char* relativePath = relativePathStart + 6;
                char* relativeKey = PushString(&stringArena, relativePath);
                FilenameToKey(relativeKey);
                WriteConstant(&shaderCodegen, relativeKey, relativePath);
                WriteScriptingConstant(&luaShaderCodegen, relativeKey, relativePath);
            }
        }

        if(definition->type == EntryType_ENVARI_TABLE) {
            ofstream tableCodegen;
            size_t nameSize = strlen(definition->name) - 5;
            char* name = PushString(&stringArena, definition->name, nameSize);
            *name = (char)toupper(*name);
            PushString(&stringArena, "Map.h", 6);
            char* define = PushString(&stringArena, name, strlen(name) + 1);
            FilenameToKey(define);
            StartMapFile(&tableCodegen, folderPath, name, define);

            *(define + nameSize) = 0;

            DataTokenizer tokenizer = StartTokenizer(definition->path);
            while(tokenizer.active) {
                char* token = NextToken(&tokenizer);

                if(tokenizer.tokenLineCount == 0) {
                    char* key = PushString(&stringArena, define, strlen(define));
                    PushChar(&stringArena, '_');
                    PushString(&stringArena, token);
                    FilenameToKey(key);

                    WriteConstant(&tableCodegen, key, token);
                }
            }
            EndTokenizer(&tokenizer);
            
            EndMapFile(&tableCodegen);
        }
    }

    EndMapFile(&foldersCodegen);
    EndMapFile(&fileCodegen);
    EndMapFile(&shaderCodegen);

    EndFile(&luaFoldersCodegen);
    EndFile(&luaFileCodegen);
    EndFile(&luaShaderCodegen);

    return 0;
}