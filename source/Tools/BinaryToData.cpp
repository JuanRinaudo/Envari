#include <fstream>
#include "iostream"

#include "../Defines.h"
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

    // static const char proggy_clean_ttf_compressed_data_base85

    return 0;
}