#if !defined(FILE_H)
#define FILE_H

#define FILE_MODE_READ "r"
#define FILE_MODE_WRITE "w"
#define FILE_MODE_APPEND "a"
#define FILE_MODE_READWRITE "r+"
#define FILE_MODE_READWRITE_CREATE "w+"
#define FILE_MODE_READAPPEND "a+"

#define FILE_MODE_READ_BINARY "rb"
#define FILE_MODE_WRITE_BINARY "wb"
#define FILE_MODE_APPEND_BINARY "ab"
#define FILE_MODE_READWRITE_BINARY "rb+"
#define FILE_MODE_READWRITE_CREATE_BINARY "wb+"
#define FILE_MODE_READAPPEND_BINARY "ab+"

#include <filesystem>
#include <stdio.h>

static void* LoadFileToMemory(const char* filename, const char* mode, u32* fileSize)
{
    FILE* file = fopen(filename, mode);

	void* fileBuffer = 0;
	if (file) {
		fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
		*fileSize = (u32)size;
		rewind(file);

		fileBuffer = malloc(size);
		fread(fileBuffer, 1, size, file);
	}

    return fileBuffer;
}

#endif