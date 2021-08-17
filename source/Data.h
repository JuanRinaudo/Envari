#ifndef DATA_H
#define DATA_H

struct DataTokenizer {
    bool active;
    void* memory;
    size_t memorySize;
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

// #TODO (Juan): For now this works only with the ASCII and extended ASCII codes, add more support later

// #NOTE #PERFORMANCE (Juan): This could be changed for a function that returns the correct type and checks the string only once 
static bool TokenIsInt(const char* string)
{
    i32 i = 0;
    while(string[i] != '\0') {
        i32 normalizedChar = (i32)string[i] - 48;
        if(normalizedChar < 0 || normalizedChar > 9) {
            return false;
        }
        ++i;
    }
    return true;
}

static bool TokenIsFloat(const char* string)
{
    i32 i = 0;
    bool hasDot = false;
    while(string[i] != '\0') {
        i32 normalizedChar = (i32)string[i] - 48;
        if(string[i] == '.') {
            if(hasDot) {
                return false;
            }
            else {
                hasDot = true;
            }
        } else if(normalizedChar < 0 || normalizedChar > 9) {
            return false;
        }
        ++i;
    }
    return true;
}

static void StartTokenizer(DataTokenizer* tokenizer, const char* filepath)
{
    tokenizer->memory = LoadFileToMemory(filepath, FILE_MODE_READ_BINARY, &tokenizer->memorySize);
    tokenizer->active = tokenizer->memory != 0;
    if(tokenizer->active) {
        tokenizer->dataString = (char*)tokenizer->memory;
        tokenizer->dataIndex = 0;
        tokenizer->tokenBufferIndex = 0;
        tokenizer->currentLine = 0;
        tokenizer->tokenLineCount = -1;
        tokenizer->onComment = false;
        tokenizer->parsingString = false;
    }
}

static DataTokenizer StartTokenizer(const char* filepath)
{
    DataTokenizer tokenizer;
    StartTokenizer(&tokenizer, filepath);
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

                        if(tokenizer->currentChar == '"') { // #NOTE (Juan): Empty string case
                            tokenizer->parsingString = true;
                            tokenizer->dataIndex++;
                            tokenizer->currentChar = tokenizer->dataString[tokenizer->dataIndex];
                        }
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
                        Log("Parsing error, max token count reached %d", DATA_MAX_TOKEN_COUNT);
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

    tokenizer->active = false;
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