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

static void UnloadFileFromMemory(void* fileBuffer)
{
	free(fileBuffer);
}

static void UnloadFileFromMemory(char* fileBuffer)
{
    UnloadFileFromMemory((void*)fileBuffer);
}

#define DATA_MAX_TOKEN_COUNT 128

struct DataTokenizer {
    bool active;
    void* memory;
    u32 memorySize;
    char* dataString;
    u32 dataIndex;
    char currentChar;
    char tokenBuffer[DATA_MAX_TOKEN_COUNT];
    u32 tokenBufferIndex;
    u32 currentLine;
    i32 tokenLineCount;
    bool onComment;
    bool parsingString;
};

static void StartTokenizer(DataTokenizer* tokenizer, const char* filename)
{
    tokenizer->active = true;
    tokenizer->memory = LoadFileToMemory(filename, "rb", &tokenizer->memorySize);
    tokenizer->dataString = (char*)tokenizer->memory;
    tokenizer->dataIndex = 0;
    tokenizer->tokenBufferIndex = 0;
    tokenizer->currentLine = 0;
    tokenizer->tokenLineCount = -1;
    tokenizer->onComment = false;
    tokenizer->parsingString = false;
}

static DataTokenizer StartTokenizer(const char* filename)
{
    DataTokenizer tokenizer;
    StartTokenizer(&tokenizer, filename);
    return tokenizer;
}

static char* NextToken(DataTokenizer* tokenizer)
{
    if(tokenizer->active) {
        tokenizer->tokenBufferIndex = 0;

        while (tokenizer->dataIndex < tokenizer->memorySize) {
            tokenizer->currentChar = tokenizer->dataString[tokenizer->dataIndex];
            
            if(!tokenizer->parsingString && tokenizer->currentChar == '#') {
                tokenizer->onComment = true;
            }

            if (tokenizer->parsingString || (!tokenizer->onComment && tokenizer->currentChar > ' ')) {
                if(tokenizer->currentChar == '"') {
                    if(!tokenizer->parsingString) {
                        tokenizer->dataIndex++;
                        tokenizer->currentChar = tokenizer->dataString[tokenizer->dataIndex];
                    }
                    tokenizer->parsingString = !tokenizer->parsingString;
                }
                
                if (!tokenizer->parsingString && tokenizer->currentChar == ';') {
                    ++tokenizer->dataIndex;
                    ++tokenizer->tokenLineCount;
                    return tokenizer->tokenBuffer;
                }
                else {
                    tokenizer->tokenBuffer[tokenizer->tokenBufferIndex] = tokenizer->currentChar;
                    tokenizer->tokenBufferIndex++;
                    
                    if(tokenizer->tokenBufferIndex == DATA_MAX_TOKEN_COUNT) {
                        // Log(&editorConsole, "Parsing error, max token count reached %d", DATA_MAX_TOKEN_COUNT);
                        return 0;
                    }

                    if(!tokenizer->parsingString) {
                        char nextChar = tokenizer->dataString[tokenizer->dataIndex + 1];
                        if(nextChar == ';' || nextChar == ' ' || (tokenizer->tokenLineCount == -1 && nextChar == ':')) {
                            if(tokenizer->tokenBuffer[tokenizer->tokenBufferIndex - 1] == '"') {
                                tokenizer->tokenBuffer[tokenizer->tokenBufferIndex - 1] = '\0';
                            }
                            else {
                                tokenizer->tokenBuffer[tokenizer->tokenBufferIndex] = '\0';
                                tokenizer->tokenBufferIndex++;
                            }
                            ++tokenizer->dataIndex;
                            ++tokenizer->dataIndex;
                            ++tokenizer->tokenLineCount;
                            return tokenizer->tokenBuffer;
                        }
                    }
                }
            }

            if(tokenizer->currentChar == '\n' || tokenizer->currentChar == '\0') {
                tokenizer->currentLine++;
                tokenizer->tokenLineCount = -1;

                if(tokenizer->onComment) {
                    tokenizer->onComment = false;
                }
            }

            ++tokenizer->dataIndex;

            if(tokenizer->dataIndex == tokenizer->memorySize) {
                tokenizer->active = false;
            }
        }
    }

    return 0;
}

static void EndTokenizer(DataTokenizer* tokenizer)
{
    tokenizer->active = false;
    tokenizer->memory = 0;
    tokenizer->memorySize = 0;
    tokenizer->dataString = 0;
    tokenizer->tokenBuffer[0] = '\0';
    tokenizer->tokenBufferIndex = 0;
    tokenizer->tokenLineCount = 0;
    UnloadFileFromMemory(tokenizer->memory);
}

#endif