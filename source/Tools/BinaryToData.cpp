#include <fstream>
#include <iostream>
#include <queue>
#include <filesystem>
#include <string.h>
#include <stdlib.h>

#define Log printf

#include <Defines.h>
#include <Templates.h>
#include <MemoryStructs.h>
#include <Memory.h>
#include <ASCII85.h>
#include "../ZSTD/zstd.c"
#include "binary_to_compressed_c.cpp"

#include <File.h>

#include <zstd.c>

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
    string workingDirectoryString = filesystem::current_path().string();
    strcpy(workingDirectory, workingDirectoryString.c_str());

    size_t workingDirectorySize = strlen(workingDirectory);
    const char* folderName = "/tobinary/";
    strcat(workingDirectory, folderName);
    workingDirectorySize += strlen(folderName);

    queue<filesystem::directory_entry> entryQueue;

    // #NOTE (Juan): Initial root working directory scan
    for (const auto& entry : filesystem::directory_iterator(workingDirectory)) {
        entryQueue.push(entry);
    }

    while(!entryQueue.empty()) {
        auto entry = entryQueue.front();
        
        auto entryPath = entry.path();
        auto entryPathString = entry.path().string();
        auto entryNameString = entry.path().filename().stem().string();
        const char* path = entryPathString.c_str();
        const char* name = entryNameString.c_str();

        cout << path << endl;

        if(!strstr(path, ".txt")) {
            FILE* readFile = fopen(path, "rb");

            string filenameString = entryPath.filename().string();
            const char* filename = filenameString.c_str();
            char entryFilename[512];
            strcpy(entryFilename, filename);
            NormalizeFilename(entryFilename);

            Log("%s\n", entryFilename);

            char* newPath = PushString(&stringArena, path);
            char* extensionChar = strrchr(newPath, '.');
            strcpy(extensionChar, ".txt");

            FILE* writeFile = fopen(newPath, "w");

            fseek(readFile, 0, SEEK_END);
            size_t size = ftell(readFile);
            size_t compressedSizeBound = ZSTD_compressBound(size);
            void* file = malloc(size);
            void* compressed = malloc(compressedSizeBound);
            ZeroSize(compressedSizeBound, compressed);

            rewind(readFile);
            char* data = (char*)malloc(size);
            ZeroSize(size, data);
            fread(file, 1, size, readFile);

            fread(data, 1, size, readFile);

            size_t cBufferSize = ZSTD_compressBound(size);
            u8* cBuffer = (u8*)malloc(cBufferSize);
            size_t cSize = ZSTD_compress(cBuffer, cBufferSize, (void*)data, size, 0);

            size_t encodedSize = GetEncoded85Size(cSize);

            char start[512];
            sprintf(start, "static const char %s[%d + 1] = \"", entryFilename, encodedSize);
            fputs(start, writeFile);

            u32 k = 0;

            char prev_c = 0;
            for(size_t i = 0; i < cSize; i += 4) { 
                u32 d = *((u32*)(cBuffer + i));
                for (u32 n5 = 0; n5 < 5; n5++, d /= 85)
                {
                    char c = Encode85Byte(d);
                    fprintf(writeFile, (c == '?' && prev_c == '?') ? "\\%c" : "%c", c);
                    k++;
                    prev_c = c;
                }
            size_t compressedSize = ZSTD_compress(compressed, compressedSizeBound, file, (u32)size, 3);
            cout << compressedSize << endl;

            FILE* writeFile = fopen(newPath, "w");

            fprintf(writeFile, "static const char %s_data_base85[%d+1] =\n    \"", name, (i32)((compressedSize + 3) / 4)*5);
            char prev_c = 0;
            for (i32 src_i = 0; src_i < compressedSize; src_i += 4)
            {
                // This is made a little more complicated by the fact that ??X sequences are interpreted as trigraphs by old C/C++ compilers. So we need to escape pairs of ??.
                u32 d = *(u32*)((char*)compressed + src_i);
                for (u32 n5 = 0; n5 < 5; n5++, d /= 85)
                {
                    char c = Encode85Byte(d);
                    fprintf(writeFile, (c == '?' && prev_c == '?') ? "\\%c" : "%c", c);
                    prev_c = c;
                }
                if ((src_i % 112) == 112 - 4)
                    fprintf(writeFile, "\"\n    \"");
            }

            const char* end = "\";";
            fputs(end, writeFile);
            fputs("\";", writeFile);

            fclose(readFile);
            fclose(writeFile);
        }

        entryQueue.pop();
    }

    return 0;
}