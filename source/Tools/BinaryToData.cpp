#include <direct.h>
#include <fstream>
#include "iostream"
#include <queue>
#include <filesystem>

#define Log printf

#include "../Defines.h"
#include "Templates.h"
#include "../MemoryStructs.h"
#include "../Memory.h"

using namespace std;

static MemoryArena stringArena;
static MemoryArena dataArena;

static void StartFile(ofstream* stream, const char* folderPath, const char* filepath)
{
    char* codegenPath = PushArray(&stringArena, strlen(folderPath) + strlen(filepath), char);
    strcpy(codegenPath, folderPath);
    strcat(codegenPath, filepath);

    stream->open(codegenPath, ios_base::out);
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

i32 main()
{
    cout << "Transform binary files to data" << '\n';

    i32 stringMemorySize = Megabytes(32);
    i32 dataMemorySize = Megabytes(32);

    void* stringMemory = malloc(stringMemorySize);
    void* dataMemory = malloc(dataMemorySize);

    InitializeArena(&stringArena, stringMemorySize, (u8*)stringMemory, 0);
    InitializeArena(&dataArena, dataMemorySize, (u8*)dataMemory, 0);

    char workingDirectory[512];
    _getcwd(workingDirectory, 512);

    i32 workingDirectorySize = strlen(workingDirectory);
    const char* folderName = "/tobinary/";
    strcat(workingDirectory, folderName);
    workingDirectorySize += strlen(folderName);

    // filesystem::exists(workingDirectory);

    queue<filesystem::directory_entry> entryQueue;

    // #NOTE (Juan): Initial root working directory scan
    for (const auto& entry : filesystem::directory_iterator(workingDirectory)) {
        entryQueue.push(entry);
    }

    while(!entryQueue.empty()) {
        auto entry = entryQueue.front();
        
        auto entryPathString = entry.path().string();
        const char* path = entryPathString.c_str();

        if(!strstr(path, ".txt")) {
            FILE* readFile = fopen(path, "rb");

            char* newPath = PushString(&stringArena, path);
            char* extensionChar = strrchr(newPath, '.');
            strcpy(extensionChar, ".txt");

            FILE* writeFile = fopen(newPath, "w");

            char start[512];
            sprintf(start, "u8 %s[] = {", "testFile");
            fputs(start, writeFile);

            fseek(readFile, 0, SEEK_END);
            size_t size = ftell(readFile);
            rewind(readFile);

            char stringBuffer[512];
            for(size_t i = 0; i < size; ++i) {
                i32 value = fgetc(readFile);
                itoa(value, stringBuffer, 10);
                fputs(stringBuffer, writeFile);
                if(i < size - 1) { fputc(',', writeFile); }
            }

            char* end = "};";
            fputs(end, writeFile);

            fclose(readFile);
            fclose(writeFile);
        }

        entryQueue.pop();
    }

    return 0;
}