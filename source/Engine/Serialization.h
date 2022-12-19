#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#define TableHasKey(table, key) TableHasKey_(&table, key)
static bool TableHasKey_(DataTable** table, const char* key)
{
    void* keyPointer = shgetp_null(*table, key);
    return keyPointer != 0;
}

static char* TableGetValue(DataTable** table, const char* key)
{
    char* tableValue = shget(*table, key);
    if(tableValue) {
        return tableValue;
    }
    else {
        return 0;
    }
}

static char* TableGetString(DataTable** table, const char* key)
{
    char* tableString = shget(*table, key);
    if(tableString) {
        return tableString;
    }
    else {
        return (char*)"";
    }
}

static char* TableGetString(DataTable** table, const char* key, char* defaultValue)
{
    char* tableString = shget(*table, key);
    if(tableString) {
        return tableString;
    }
    else {
        return defaultValue;
    }
}

static i32 TableGetInt(DataTable** table, const char* key, i32 defaultValue = 0)
{
    char* tableString = shget(*table, key);
    if(tableString) {
        return StringToInt(tableString);
    }
    else {
        return defaultValue;
    }
}

static bool TableGetBool(DataTable** table, const char* key, bool defaultValue = false)
{
    char* tableString = shget(*table, key);
    if(tableString) {
        return StringToInt(tableString) != 0;
    }
    else {
        return defaultValue;
    }
}

static f32 TableGetFloat(DataTable** table, const char* key, f32 defaultValue = 0)
{
    char* tableString = shget(*table, key);
    if(tableString) {
        char* endPointer = 0;
        return strtof(tableString, &endPointer);
    }
    else {
        return defaultValue;
    }
}

static v2 TableGetV2(DataTable** table, const char* key, v2 defaultValue = V2(0, 0))
{
    char* tableString = shget(*table, key);
    if(tableString) {
        char* endPointer = 0;
        f32 x = strtof(tableString, &endPointer);
        tableString = endPointer;
        tableString++;
        f32 y = strtof(tableString, &endPointer);
        return V2(x, y);
    }
    else {
        return defaultValue;
    }
}

static v3 TableGetV3(DataTable** table, const char* key, v3 defaultValue = V3(0, 0, 0))
{
    char* tableString = shget(*table, key);
    if(tableString) {
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
    else {
        return defaultValue;
    }
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
                PushChar(arena, 0);
            }
            valueIndex = 0;
            if(keyPointer != 0) {
                Log("Parsing error, parsing a key token when last token was a key, line: %d", tokenizer.currentLine);
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
    PushChar(arena, 0);

    EndTokenizer(&tokenizer);

    return true;
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

void TableSetString_(MemoryArena *arena, SerializableTable** table, const char* key, const char* value)
{
    size_t stringSize = strlen(value);

    SerializableValue* tableValue = shget(*table, key);
    if(tableValue) {
        if(stringSize < tableValue->count) {
            strcpy((char*)tableValue->value, value);
        }
        else {
            tableValue->value = PushString(arena, value);
        }
    }
    else {
        char* keyPointer = PushString(arena, key);
        SerializableValue* tableValue = PushStruct(arena, SerializableValue);
        tableValue->value = PushString(arena, value);
        tableValue->type = SerializableType_STRING;
        tableValue->count = stringSize;
        shput(*table, keyPointer, tableValue);
    }
}

char* TableGetString (SerializableTable** table, const char* key)
{
    SerializableValue* tableValue = shget(*table, key);
    if(tableValue) {
        return (char*)tableValue->value;
    }
    else {
        return (char*)"";
    }
}

char* TableGetString (SerializableTable** table, const char* key, char* defaultValue)
{
    SerializableValue* tableValue = shget(*table, key);
    if(tableValue) {
        return (char*)tableValue->value;
    }
    else {
        return defaultValue;
    }
}

GenerateTableGet(Bool, bool, false)
GenerateTableSet(Bool, bool, SerializableType_BOOL)

GenerateTableGet(U8, u8, 0)
GenerateTableSet(U8, u8, SerializableType_U8)
GenerateTableGet(U16, u16, 0)
GenerateTableSet(U16, u16, SerializableType_U16)
GenerateTableGet(U32, u32, 0)
GenerateTableSet(U32, u32, SerializableType_U32)
GenerateTableGet(U64, u64, 0)
GenerateTableSet(U64, u64, SerializableType_U64)

GenerateTableGet(I8, i8, 0)
GenerateTableSet(I8, i8, SerializableType_I8)
GenerateTableGet(I16, i16, 0)
GenerateTableSet(I16, i16, SerializableType_I16)
GenerateTableGet(I32, i32, 0)
GenerateTableSet(I32, i32, SerializableType_I32)
GenerateTableGet(I64, i64, 0)
GenerateTableSet(I64, i64, SerializableType_I64)

GenerateTableGet(F32, f32, 0)
GenerateTableSet(F32, f32, SerializableType_F32)
GenerateTableGet(F64, f64, 0)
GenerateTableSet(F64, f64, SerializableType_F64)

GenerateTableGet(V2, v2, V2(0, 0))
GenerateTableSet(V2, v2, SerializableType_V2)
GenerateTableGet(V3, v3, V3(0, 0, 0))
GenerateTableSet(V3, v3, SerializableType_V3)

static u32 SerializableValueSize(SerializableType type)
{
    switch(type) {
        case SerializableType_U8: case SerializableType_I8: case SerializableType_STRING: {
            return 1;
        }
        case SerializableType_U16: case SerializableType_I16: {
            return 2;
        }
        case SerializableType_U32: case SerializableType_I32: case SerializableType_BOOL: case SerializableType_F32: {
            return 4;
        }
        case SerializableType_U64: case SerializableType_I64: case SerializableType_F64: case SerializableType_V2: {
            return 8;
        }
        case SerializableType_V3: {
            return 12;
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
            SerializableType type = (SerializableType)StringToInt(token);
            
            token = NextToken(&tokenizer);
            i32 count = StringToInt(token);

            // #FIX (Juan): Fix for 0 value saves like empty strings
            if(count == 0) {
                token = NextToken(&tokenizer);
                continue;
            }

            void* valuePointer = 0;

            i32 valueSize = SerializableValueSize(type);
            i32 valueOffset = 0;
            if(type != SerializableType_STRING) {
                valuePointer = PushSize(arena, valueSize * count);
            }

            if(type == SerializableType_STRING) {
                token = NextToken(&tokenizer);
                char* value = PushString(arena, token);

                // #NOTE (Juan): Changes back from ' to "
                for(u32 i = 0; i < count; ++i) {
                    if(value[i] == '\'') {
                        value[i] = '"';
                    }
                }

                valuePointer = (void*)value;
            }
            else {
                for(i32 valueIndex = 0; valueIndex < count; ++valueIndex) {
                    switch(type) {
                        case SerializableType_STRING: {
                            InvalidCodePath;
                        }
                        case SerializableType_BOOL: {
                            token = NextToken(&tokenizer);
                            i32 value = StringToInt(token);
                            *((bool*)valuePointer + valueOffset) = value == 1;
                            break;
                        }
                        case SerializableType_U8: {
                            token = NextToken(&tokenizer);
                            u8 value = (u8)StringToInt(token);
                            *((u8*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_U16: {
                            token = NextToken(&tokenizer);
                            u16 value = (u16)StringToInt(token);
                            *((u16*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_U32: {
                            token = NextToken(&tokenizer);
                            u32 value = StringToInt(token);
                            *((u32*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_U64: {
                            token = NextToken(&tokenizer);
                            u64 value = (u64)StringToInt64(token);
                            *((u64*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_I8: {
                            token = NextToken(&tokenizer);
                            i8 value = (i8)StringToInt(token);
                            *((i8*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_I16: {
                            token = NextToken(&tokenizer);
                            i16 value = (i16)StringToInt(token);
                            *((i16*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_I32: {
                            token = NextToken(&tokenizer);
                            i32 value = StringToInt(token);
                            *((i32*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_I64: {
                            token = NextToken(&tokenizer);
                            i64 value = StringToInt64(token);
                            *((i64*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_F32: {
                            token = NextToken(&tokenizer);
                            char* endPointer = 0;
                            f32 value = strtof(token, &endPointer);
                            *((f32*)valuePointer + valueOffset) = value;
                            break;
                        }
                        case SerializableType_F64: {
                            token = NextToken(&tokenizer);
                            char* endPointer = 0;
                            f64 value = strtod(token, &endPointer);
                            *((f64*)valuePointer + valueOffset) = value;
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
                        case SerializableType_V3: {
                            token = NextToken(&tokenizer);
                            char* endPointer = 0;
                            f32 x = strtof(token, &endPointer);
                            token = NextToken(&tokenizer);
                            f32 y = strtof(token, &endPointer);
                            token = NextToken(&tokenizer);
                            f32 z = strtof(token, &endPointer);
                            *((v3*)valuePointer + valueOffset) = V3(x, y, z);
                            break;
                        }
                        default: {
                            InvalidCodePath;
                            break;
                        }
                    }
                    valueOffset += valueSize;
                }
            }

            AssertMessage(valuePointer != 0, "No value found");
            TableSetValueReference(arena, table, keyPointer, valuePointer, type, count);
        }
    }

    EndTokenizer(&tokenizer);

    return true;
}

void SerializeTable(SerializableTable** table, const char* filepath)
{
    FILE* file = fopen(filepath, FILE_MODE_WRITE_CREATE_BINARY);

    char stringBuffer[DATA_MAX_TOKEN_COUNT];

    if(file) {
        size_t tableSize = shlen(*table);
        fputs("#version", file);
        fputc(' ', file);
        sprintf(stringBuffer, "%d", 1);
        fputs(stringBuffer, file);
        fputs(";\n", file);

        for(i32 index = 0; index < tableSize; ++index) {
            SerializableTable data = (*table)[index];
            fputs(data.key, file);
            fputc(' ', file);
            
            sprintf(stringBuffer, "%d", (i32)data.value->type);
            fputs(stringBuffer, file);
            fputc(' ', file);
            
            if(data.value->type == SerializableType_STRING) {
                size_t valueCount = strlen(((char*)data.value->value));
                sprintf(stringBuffer, "%d", (i32)valueCount);
                fputs(stringBuffer, file);
                fputc(' ', file);

                fputc('"', file);

                // #NOTE (Juan): Changes " for ' because of deserialization, I might need to find a better solution later
                for(u32 i = 0; i < valueCount; ++i) {
                    if(((char*)data.value->value)[i] == '"') {
                        ((char*)data.value->value)[i] = '\'';
                    }
                }
                
                fputs((char*)data.value->value, file);
                fputc('"', file);
            }
            else {
                size_t valueCount = data.value->count;
                sprintf(stringBuffer, "%d", (i32)valueCount);
                fputs(stringBuffer, file);
                fputc(' ', file);
            
                for(i32 valueIndex = 0; valueIndex < valueCount; ++valueIndex) {
                    if(valueIndex > 0) { fputc(' ', file); }
                    switch(data.value->type) {
                        case SerializableType_STRING: {
                            InvalidCodePath;
                        }
                        case SerializableType_BOOL: {
                            bool i = *((bool*)data.value->value);
                            fprintf(file, "%d", i ? 1 : 0);
                            break;
                        }
                        case SerializableType_U8: {
                            u8 i = *((u8*)data.value->value);
                            fprintf(file, "%" PRIu8, i);
                            break;
                        }
                        case SerializableType_U16: {
                            u16 i = *((u16*)data.value->value);
                            fprintf(file, "%" PRIu16, i);
                            break;
                        }
                        case SerializableType_U32: {
                            u32 i = *((u32*)data.value->value);
                            fprintf(file, "%" PRIu32, i);
                            break;
                        }
                        case SerializableType_U64: {
                            u64 i = *((u64*)data.value->value);
                            fprintf(file, "%" PRIu64, i);
                            break;
                        }
                        case SerializableType_I8: {
                            i8 i = *((i8*)data.value->value);
                            fprintf(file, "%" PRId8, i);
                            break;
                        }
                        case SerializableType_I16: {
                            i16 i = *((i16*)data.value->value);
                            fprintf(file, "%" PRId16, i);
                            break;
                        }
                        case SerializableType_I32: {
                            i32 i = *((i32*)data.value->value);
                            fprintf(file, "%" PRId32, i);
                            break;
                        }
                        case SerializableType_I64: {
                            i64 i = *((i64*)data.value->value);
                            fprintf(file, "%" PRId64, i);
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
            }

            fputs(";\n", file);
        }
        fclose(file);
    }
}

#endif