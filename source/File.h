#ifndef FILE_H
#define FILE_H

#define FILE_MODE_READ "r"
#define FILE_MODE_WRITE "w"
#define FILE_MODE_APPEND "a"
#define FILE_MODE_READWRITE "r+"
#define FILE_MODE_WRITE_CREATE "w+"
#define FILE_MODE_READAPPEND "a+"

#define FILE_MODE_READ_BINARY "rb"
#define FILE_MODE_WRITE_BINARY "wb"
#define FILE_MODE_APPEND_BINARY "ab"
#define FILE_MODE_READWRITE_BINARY "rb+"
#define FILE_MODE_WRITE_CREATE_BINARY "wb+"
#define FILE_MODE_READAPPEND_BINARY "ab+"

#include <stdio.h>

static void* LoadFileToMemory(const char* filepath, const char* mode, u32* fileSize)
{
    FILE* file = fopen(filepath, mode);

	void* fileBuffer = 0;
	if (file) {
		fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
		*fileSize = (u32)size;
		rewind(file);

		fileBuffer = malloc(size);
		fread(fileBuffer, 1, size, file);
        fclose(file);
	}

    return fileBuffer;
}

static void UnloadFileFromMemory(void* fileBuffer)
{
	free(fileBuffer);
}

static void UnloadFileFromMemory(char* fileBuffer)
{
    UnloadFileFromMemory((void*)fileBuffer);
}

#endif