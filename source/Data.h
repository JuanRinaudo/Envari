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

#define TableHasKey(table, key) TableHasKey_(&table, key)
static bool TableHasKey_(DataTable** table, const char* key)
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
    return tableString;
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
        char* token = NextToken(&tokenizer);

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
        else if(token) {            
            char* tokenPointer = PushString(arena, tokenizer.tokenBuffer, tokenizer.tokenBufferIndex);

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

static bool TableHasKey_(SerializableTable** table, const char* key)
{
    void* keyPointer = shgetp_null(*table, key);
    return keyPointer != 0;
}

static SerializableValue* TableGetValue(SerializableTable** table, const char* key)
{
    SerializableValue* tableValue = shget(*table, key);
    return tableValue;
}

static void TableSetValueReference(MemoryArena *arena, SerializableTable** table, const char* key, void* value, SerializableType type, u32 count = 1)
{
    char* keyPointer = PushString(arena, key);
    SerializableValue* tableValue = PushStruct(arena, SerializableValue);
    tableValue->value = value;
    tableValue->type = type;
    tableValue->count = count;
    shput(*table, keyPointer, tableValue);
}

#define TableSetValue(arena, table, key, value, type) TableSetValue_(arena, &table, key, (void*)value, sizeof(value), type)
#define TableSetValues(arena, table, key, value, type, count) TableSetValue_(arena, &table, key, (void*)value, sizeof(value), type, count)
static void TableSetValue_(MemoryArena *arena, SerializableTable** table, const char* key, void* value, size_t size, SerializableType type, u32 count = 1)
{
    char* keyPointer = PushString(arena, key);
    SerializableValue* tableValue = PushStruct(arena, SerializableValue);
    tableValue->value = PushSize(arena, size);
    memcpy(tableValue->value, value, size);
    tableValue->type = type;
    tableValue->count = count;
    shput(*table, keyPointer, tableValue);
}

#define TableSetString(arena, table, key, value) TableSetValue_(arena, &table, key, (void*)value, sizeof(value), SerializableType_STRING)

#define GenerateTableGet(POSTFIX, valueType, typeDefault) static valueType TableGet##POSTFIX##(SerializableTable** table, const char* key, valueType defaultValue = typeDefault) \
{ \
    SerializableValue* tableValue = shget(*table, key); \
    if(tableValue) { \
        return *((valueType*)tableValue->value); \
    } \
    else { \
        return defaultValue; \
    } \
}

#define GenerateTableSet(POSTFIX, valueType, serializableType) static void TableSet##POSTFIX##_(MemoryArena *arena, SerializableTable** table, const char* key, valueType value) \
{ \
    char* keyPointer = PushString(arena, key); \
    SerializableValue* tableValue = PushStruct(arena, SerializableValue); \
    tableValue->value = PushSize(arena, sizeof(valueType)); \
    *((valueType*)tableValue->value) = value; \
    tableValue->type = serializableType; \
    tableValue->count = 1; \
    shput(*table, keyPointer, tableValue); \
}

GenerateTableGet(Bool, bool, false)
#define TableSetBool(arena, table, key, value) TableSetBool_(arena, &table, key, value)
GenerateTableSet(Bool, bool, SerializableType_BOOL)

GenerateTableGet(I32, i32, 0)
#define TableSetI32(arena, table, key, value) TableSetI32_(arena, &table, key, value)
GenerateTableSet(I32, i32, SerializableType_I32)

GenerateTableGet(F32, f32, 0)
#define TableSetF32(arena, table, key, value) TableSetF32_(arena, &table, key, value)
GenerateTableSet(F32, f32, SerializableType_F32)

GenerateTableGet(V2, v2, V2(0, 0))
#define TableSetV2(arena, table, key, value) TableSetV2_(arena, &table, key, value)
GenerateTableSet(V2, v2, SerializableType_V2)

static char* TableGetString(SerializableTable** table, const char* key, char* defaultValue = "")
{
    SerializableValue* tableValue = shget(*table, key);
    if(tableValue) {
        return (char*)tableValue->value;
    }
    else {
        return defaultValue;
    }
}

static u32 SerializableValueSize(SerializableType type)
{
    switch(type) {
        case SerializableType_STRING: {
            return 1;
        }
        case SerializableType_I32: case SerializableType_BOOL: case SerializableType_F32: {
            return 4;
        }
        case SerializableType_V2: {
            return 8;
        }
        default: {
            InvalidCodePath;
            break;
        }
    }
    InvalidCodePath;
    return 0;
}

static bool DeserializeTable(MemoryArena *arena, SerializableTable** table, const char* filepath)
{
    DataTokenizer tokenizer = StartTokenizer(filepath);
    
    char* keyPointer = 0;
    while(tokenizer.active) {
        char* token = NextToken(&tokenizer);
        
        if(token) {
            char* keyPointer = PushString(arena, token);

            token = NextToken(&tokenizer);
            SerializableType type = (SerializableType)atoi(token);
            
            token = NextToken(&tokenizer);
            i32 count = atoi(token);

            void* valuePointer = 0;

            i32 valueSize = SerializableValueSize(type);
            i32 valueOffset = 0;
            if(type != SerializableType_STRING) {
                valuePointer = PushSize(arena, valueSize * count);
            }

            for(i32 valueIndex = 0; valueIndex < count; ++valueIndex) {
                switch(type) {
                    case SerializableType_STRING: {
                        token = NextToken(&tokenizer);
                        char* value = PushString(arena, token);
                        if(valueIndex == 0) { valuePointer = (void*)value; }
                        break;
                    }
                    case SerializableType_BOOL: {
                        token = NextToken(&tokenizer);
                        i32 value = atoi(token);
                        *((bool*)valuePointer + valueOffset) = value == 1;
                        break;
                    }
                    case SerializableType_I32: {
                        token = NextToken(&tokenizer);
                        i32 value = atoi(token);
                        *((i32*)valuePointer + valueOffset) = value;
                        break;
                    }
                    case SerializableType_F32: {
                        token = NextToken(&tokenizer);
                        char* endPointer = 0;
                        f32 value = strtof(token, &endPointer);
                        *((f32*)valuePointer + valueOffset) = value;
                        break;
                    }
                    case SerializableType_V2: {
                        token = NextToken(&tokenizer);
                        char* endPointer = 0;
                        f32 x = strtof(token, &endPointer);
                        token = NextToken(&tokenizer);
                        f32 y = strtof(token, &endPointer);
                        *((v2*)valuePointer + valueOffset) = V2(x, y);
                        break;
                    }
                    default: {
                        InvalidCodePath;
                        break;
                    }
                }
                valueOffset += valueSize;
            }

            Assert(valuePointer != 0);
            TableSetValueReference(arena, table, keyPointer, valuePointer, type, count);
        }
    }

    EndTokenizer(&tokenizer);

    return true;
}

static void SerializeTable(SerializableTable** table, const char* filepath)
{
    FILE* file = fopen(filepath, FILE_MODE_WRITE_CREATE_BINARY);

    char stringBuffer[DATA_MAX_TOKEN_COUNT];

    if(file) {
        i32 tableSize = shlen(*table);
        for(i32 index = 0; index < tableSize; ++index) {
            SerializableTable data = (*table)[index];
            fputs(data.key, file);
            fputc(' ', file);
            
            itoa((i32)data.value->type, stringBuffer, 10);
            fputs(stringBuffer, file);
            fputc(' ', file);

            i32 valueCount = data.value->count;
            itoa(valueCount, stringBuffer, 10);
            fputs(stringBuffer, file);
            fputc(' ', file);
            
            for(i32 valueIndex = 0; valueIndex < valueCount; ++valueIndex) {
                if(valueIndex > 0) { fputc(' ', file); }
                switch(data.value->type) {
                    case SerializableType_STRING: {
                        fputc('"', file);
                        fputs((char*)data.value->value, file);
                        fputc('"', file);
                        break;
                    }
                    case SerializableType_BOOL: {
                        bool i = *((bool*)data.value->value);
                        fprintf(file, "%d", i ? 1 : 0);
                        break;
                    }
                    case SerializableType_I32: {
                        i32 i = *((i32*)data.value->value);
                        fprintf(file, "%d", i);
                        break;
                    }
                    case SerializableType_F32: {
                        f32 f = *((f32*)data.value->value);
                        fprintf(file, "%f", f);
                        break;
                    }
                    case SerializableType_V2: {
                        v2 v = *((v2*)data.value->value);
                        fprintf(file, "%f %f", v.x, v.y);
                        break;
                    }
                    default: {
                        InvalidCodePath;
                        break;
                    }
                }
            }

            fputs(";\n", file);
        }
        fclose(file);
    }
}

#endif