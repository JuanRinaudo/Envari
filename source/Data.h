#ifndef DATA_H
#define DATA_H

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
    tokenizer->active = true;
    tokenizer->memory = LoadFileToMemory(filepath, FILE_MODE_READ_BINARY, &tokenizer->memorySize);
    tokenizer->dataString = (char*)tokenizer->memory;
    tokenizer->dataIndex = 0;
    tokenizer->tokenBufferIndex = 0;
    tokenizer->currentLine = 0;
    tokenizer->tokenLineCount = -1;
    tokenizer->onComment = false;
    tokenizer->parsingString = false;
}

static DataTokenizer StartTokenizer(const char* filepath)
{
    DataTokenizer tokenizer;
    StartTokenizer(&tokenizer, filepath);
    return tokenizer;
}

static char* NextToken(DataTokenizer* tokenizer, bool* isString = 0)
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
                    if(isString) { *isString = true; }
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

// #NOTE (Juan): This does a linear scan of the current data to check the ammount of parameters, dataString must point to the start of the line
static void GetDataLineParameters(char* dataString, i32 index) {
    char currentChar = dataString[index];
    bool parsingString = false;
    bool parsingKey = false;
    i32 parameterCount = 0;
    while(parsingString || (currentChar != ';' && currentChar != '#')) {
        currentChar = dataString[index];
        if (!parsingString && dataString[index - 1] <= ' ' && currentChar > ' ') {
            parameterCount++;
        }
        if(currentChar == '"') {
            parsingString = !parsingString;
        }

        index++;
    }

    currentChar = dataString[index];
}

static bool TableHasKey(DataTable** table, const char* key)
{
    void* keyPointer = shgetp_null(*table, key);
    return keyPointer != 0;
}

static char* TableGetValue(DataTable** table, const char* key)
{
    char* tableValue = shget(*table, key);
    return tableValue;
}

static char* TableGetString(DataTable** table, const char* key)
{
    char* tableString = shget(*table, key);
    return tableString + 1;
}

static i32 TableGetInt(DataTable** table, const char* key)
{
    char* tableString = shget(*table, key);
    return atoi(tableString);
}

static bool TableGetBool(DataTable** table, const char* key)
{
    char* tableString = shget(*table, key);
    return atoi(tableString) != 0;
}

static f32 TableGetFloat(DataTable** table, const char* key)
{
    char* tableString = shget(*table, key);
    char* endPointer = 0;
    return strtof(tableString, &endPointer);
}

static v2 TableGetV2(DataTable** table, const char* key)
{
    char* tableString = shget(*table, key);
    char* endPointer = 0;
    f32 x = strtof(tableString, &endPointer);
    tableString = endPointer;
    tableString++;
    f32 y = strtof(tableString, &endPointer);
    return V2(x, y);
}

static v3 TableGetV3(DataTable** table, const char* key)
{
    char* tableString = shget(*table, key);
    char* endPointer = 0;
    f32 x = strtof(tableString, &endPointer);
    tableString = endPointer;
    tableString++;
    f32 y = strtof(tableString, &endPointer);
    tableString = endPointer;
    tableString++;
    f32 z = strtof(tableString, &endPointer);
    return V3(x, y, z);
}

static bool DeserializeDataTable(MemoryArena *arena, DataTable** table, const char* filepath)
{
    DataTokenizer tokenizer = StartTokenizer(filepath);

    i32 valueIndex = -1;
    char* keyPointer = 0;
    while(tokenizer.active) {
        bool isString = false;
        char* token = NextToken(&tokenizer, &isString);

        if(tokenizer.tokenLineCount == 0) {
            if(valueIndex > -1 && keyPointer == 0) {
                PushChar(arena, '\0');
            }
            valueIndex = 0;
            if(keyPointer != 0) {
                Log(&editorConsole, "Parsing error, parsing a key token when last token was a key, line: %d", tokenizer.currentLine);
                return false;
            }
            
            keyPointer = PushString(arena, tokenizer.tokenBuffer, tokenizer.tokenBufferIndex);
        }
        else if(token != 0) {
            char* tokenPointer = 0;
            if(isString) { tokenPointer = PushChar(arena, '"'); }
            
            char* stringPointer = PushString(arena, tokenizer.tokenBuffer, tokenizer.tokenBufferIndex);
            if(tokenPointer == 0) { tokenPointer = stringPointer; }

            if(tokenizer.tokenLineCount == 1) {
                shput(*table, keyPointer, tokenPointer);
                keyPointer = 0;
            }
            
            valueIndex++;
        }
    }
    PushChar(arena, '\0');

    EndTokenizer(&tokenizer);

    return true;
}

static bool DeserializeDataTable(DataTable** table, const char* filepath)
{
    return DeserializeDataTable(&permanentState->arena, table, filepath);
}

static void SerializeDataTable(DataTable** table, const char* filepath)
{
    FILE* file = fopen(filepath, FILE_MODE_WRITE_CREATE_BINARY);

    if(file) {
        i32 tableSize = shlen(*table);
        for(i32 index = 0; index < tableSize; ++index) {
            DataTable data = (*table)[index];
            fputs(data.key, file);
            fputs(": ", file);
            
            i32 valueIndex = 0;
            char* value = data.value;
            while(value[0] != 0) {
                if(valueIndex > 0) { fputc(' ', file); }

                fputs(value, file);
                if(value[0] == '"') { fputc('"', file); }
                i32 size = strlen(value);
                value += size + 1;
                valueIndex++;
            }

            fputs(";\n", file);
        }
        fclose(file);
    }
}

#endif