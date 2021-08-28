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

static void* LoadFileToMemory(const char* filepath, const char* mode, size_t* fileSize)
{
    FILE* file = fopen(filepath, mode);

	void* fileBuffer = 0;
	if (file) {
		fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
		*fileSize = size;
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

static void CreateDirectoryIfNotExists(const char* path)
{	
    filesystem::path directoryPath = filesystem::path(path);
	if(!filesystem::exists(directoryPath)) {
		filesystem::create_directories(directoryPath);
	}
}

// #TODO (Juan): Compression ZSTD
// size_t fSize = sizeof(defaultFont);
// Log("Uncompressed size = %d", fSize);
// size_t cBufferSize = ZSTD_compressBound(fSize);
// void* cBuffer = malloc(cBufferSize);
// LARGE_INTEGER performanceStart;
// LARGE_INTEGER performanceEnd;
// for(i32 i = 0; i < 4; ++i) {
//     QueryPerformanceCounter(&performanceStart);
//     size_t cSize = ZSTD_compress(cBuffer, cBufferSize, (void*)defaultFont, fSize, i);
//     QueryPerformanceCounter(&performanceEnd);
//     Log("Compressed size = %d, level %d, %f ms", cSize, i, (performanceEnd.QuadPart - performanceStart.QuadPart) / 10000.0f);
// }
// free(cBuffer);

#endif