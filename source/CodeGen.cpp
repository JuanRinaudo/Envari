#include <thread>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <queue>
#include <filesystem>

#include "IMGUI/imgui.h"
#include "STB/stb_truetype.h"

#include "Defines.h"
#include "GameStructs.h"
#include "EditorStructs.h"

#include "File.h"
#include "Memory.h"

using namespace std;

enum EntryType {
    ENTRY_UNKNOWN,
    ENTRY_FOLDER,
    ENTRY_ENVARI_TABLE,
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

static void StringToKey(char* string)
{
    while(*string)
    {
        char singleChar = *string;
        if(singleChar == '.') {
            singleChar = '_';
        }
        else if(singleChar == ' ') {
            singleChar = '_';
        }
        else if(singleChar == '-') {
            singleChar = '_';
        }
        else if(singleChar == '\\') {
            singleChar = '_';
        }
        else {
            singleChar = (char)toupper(*string);
        }
        *string = singleChar;
        string++;
    }
}

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
        if(*string == '\\' && *(string + 1)) {
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

static void WriteConstant(ofstream* stream, const char* key, const char* value)
{
    Write(stream, "const char* ");
    Write(stream, key);
    Write(stream, " = \"");
    Write(stream, value);
    Write(stream, "\";\n");
}

static void WriteScriptingConstant(ofstream* stream, const char* key, const char* value)
{
    Write(stream, key);
    Write(stream, " = \"");
    Write(stream, value);
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
        return ENTRY_FOLDER;
    }
    else {
        if(strstr(definition->path, ".envt")) {
            return ENTRY_ENVARI_TABLE;
        }
        else {
            return ENTRY_UNKNOWN;
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

    InitializeArena(&stringArena, stringMemorySize, (u8*)stringMemory);
    InitializeArena(&dataArena, dataMemorySize, (u8*)dataMemory);

    char workingDirectory[512];
    _getcwd(workingDirectory, 512);

    i32 workingDirectorySize = strlen(workingDirectory);

    char filePath[512];
    strcpy(filePath, __FILE__);
    // #NOTE (Juan): Get file end string cpp and cut it by setting a zero end in the start of it.
    char* endString = strstr(filePath, "CodeGen.cpp");
    if(!endString) {
        endString = strstr(filePath, "codegen.cpp");
    }
    if(!endString) {
        cout << "No codegen file found" << "\n";
        return -1;
    }
    endString[0] = 0;
    const char* folderName = "CodeGen/";
    strncat(filePath, folderName, 8);
    const char* folderPath = filePath;
    i32 folderPathSize = strlen(folderPath);
    cout << "Codegen folder path: " << folderPath << '\n';

    char scriptingPath[512];
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

    ofstream luaFoldersCodegen;
    ofstream luaFileCodegen;

    StartMapFile(&foldersCodegen, folderPath, "FolderMap.h", "FOLDERMAP_H");
    StartMapFile(&fileCodegen, folderPath, "FileMap.h", "FILEMAP_H");
    
    StartFile(&luaFoldersCodegen, scriptingPath, "FolderMap.lua");
    StartFile(&luaFileCodegen, scriptingPath, "FileMap.lua");

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
        StringToKey(definition->mapKey);
        i32 nameOffset = PathNameOffset(definition->fullPath);
        definition->name = PushString(&stringArena, path + nameOffset);
        definition->type = GetEntryType(&entry, definition);
        
        dataCount++;

        if(definition->type == ENTRY_FOLDER) {
            for (const auto& entry : filesystem::directory_iterator(path)) {
                entryQueue.push(entry);
            }
        }

        entryQueue.pop();
    }

    for(i32 i = 0; i < dataCount; ++i) {
        DataEntry *definition = rootDefinitions + i;
        cout << "Found: " << definition->fullPath <<
            " | filepath: " << definition->name <<
            " | Path: " << definition->path <<
            " | Key: " << definition->mapKey <<
            " | Entry type: " << definition->type << '\n';

        if(definition->type == ENTRY_FOLDER) {
            WriteConstant(&foldersCodegen, definition->mapKey, definition->path);
            WriteScriptingConstant(&luaFoldersCodegen, definition->mapKey, definition->path);
        } else {
            WriteConstant(&fileCodegen, definition->mapKey, definition->path);
            WriteScriptingConstant(&luaFileCodegen, definition->mapKey, definition->path);
        }

        if(definition->type == ENTRY_ENVARI_TABLE) {
            ofstream tableCodegen;
            i32 nameSize = strlen(definition->name) - 5;
            char* name = PushString(&stringArena, definition->name, nameSize);
            *name = (char)toupper(*name);
            PushString(&stringArena, "Map.h", 6);
            char* define = PushString(&stringArena, name, strlen(name) + 1);
            StringToKey(define);
            StartMapFile(&tableCodegen, folderPath, name, define);

            *(define + nameSize) = 0;

            DataTokenizer tokenizer = StartTokenizer(definition->path);
            while(tokenizer.active) {
                char* token = NextToken(&tokenizer);

                if(tokenizer.tokenLineCount == 0) {
                    char* key = PushString(&stringArena, define, strlen(define));
                    PushChar(&stringArena, '_');
                    PushString(&stringArena, token);
                    StringToKey(key);

                    WriteConstant(&tableCodegen, key, token);
                }
            }
            EndTokenizer(&tokenizer);
            
            EndMapFile(&tableCodegen);
        }
    }

    EndMapFile(&foldersCodegen);
    EndMapFile(&fileCodegen);

    EndFile(&luaFoldersCodegen);
    EndFile(&luaFileCodegen);

    return 0;
}