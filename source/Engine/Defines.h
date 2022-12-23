#ifndef DEFINES_H
#define DEFINES_H

#include <ConfigDefines.h>

#include <TypeDefines.h>

#include <MathDefines.h>

#include <InputDefines.h>

#include <RenderDefines.h>

#include <SoundDefines.h>

#define InvalidCodePath Assert(!"InvalidCodePath");
#define InvalidDefaultCase default: {InvalidCodePath;} break;

#define TO_STRING_INTERNAL(s) #s
#define TO_STRING(s) TO_STRING_INTERNAL(s)

#define MACRO_DEFINED(name) (#name [0] != TO_STRING_INTERNAL(name) [0])

#define MAX_SAVE_DIGITS 3

#ifndef DATA_SAVE_PATH
#define DATA_SAVE_PATH "save/savedata_%0" TO_STRING(MAX_SAVE_DIGITS) "d.save"
#endif
#ifndef CONFIG_SAVE_PATH
#define CONFIG_SAVE_PATH "save/config.save"
#endif
#ifndef EDITOR_SAVE_PATH
#define EDITOR_SAVE_PATH "save/editor.save"
#endif

#define ENVARI_MAYOR_VERSION 0
#define ENVARI_MINOR_VERSION 1
#define ENVARI_MICRO_VERSION 0

#define F32Max FLT_MAX
#define F64Max DBL_MAX

#if __EMSCRIPTEN__
    #ifdef GAME_SLOW
        #ifndef Assert
        #define Assert(Expression) if(!(Expression)) { *(volatile i32 *)0 = 0; }
        #endif
        #ifndef AssertMessage
        #define AssertMessage(Expression, Message) if(!(Expression)) { *(volatile i32 *)0 = 0; }
        #endif
    #else
        #define Assert(Expression)
        #define AssertMessage(Expression, Message)
    #endif
#else
    #ifdef GAME_SLOW
        #ifndef Assert
        #define Assert(Expression) if(!(Expression)) { *(volatile i32 *)0 = 0; }
        #endif
        #ifndef AssertMessage
        #define AssertMessage(Expression, Message) if(!(Expression)) { *(i32 *)0 = 0; }
        #endif
    #else
        #define Assert(Expression)
        #define AssertMessage(Expression, Message)
    #endif
#endif

#ifdef PLATFORM_EDITOR
    #if defined(PLATFORM_LINUX)
        #define ENVARI_PLATFORM_NAME "EditorLinux"
    #elif defined(PLATFORM_WASM)
        #define ENVARI_PLATFORM_NAME "EditorWASM"
    #elif defined(PLATFORM_WINDOWS)
        #define ENVARI_PLATFORM_NAME "EditorWindows"
    #elif defined(PLATFORM_ANDROID)
        #define ENVARI_PLATFORM_NAME "EditorAndroid"
    #endif
#else
    #if defined(PLATFORM_LINUX)
        #define ENVARI_PLATFORM_NAME "RuntimeLinux"
    #elif defined(PLATFORM_WASM)
        #define ENVARI_PLATFORM_NAME "RuntimeWASM"
    #elif defined(PLATFORM_WINDOWS)
        #define ENVARI_PLATFORM_NAME "RuntimeWindows"
    #elif defined(PLATFORM_ANDROID)
        #define ENVARI_PLATFORM_NAME "RuntimeAndroid"
    #endif
#endif

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define StringToInt(String) strtol(String, 0, 10)
#define StringToInt64(String) strtoll(String, 0, 10)

#define GenerateTableGetExtern(POSTFIX, valueType) extern valueType TableGet ## POSTFIX (SerializableTable** table, const char* key, valueType defaultValue);
#define GenerateTableGet(POSTFIX, valueType, typeDefault) valueType TableGet ## POSTFIX (SerializableTable** table, const char* key, valueType defaultValue = typeDefault) \
{ \
    SerializableValue* tableValue = shget(*table, key); \
    if(tableValue) { \
        return *((valueType*)tableValue->value); \
    } \
    else { \
        return defaultValue; \
    } \
}

#define GenerateTableSetExtern(POSTFIX, valueType, serializableType) extern void TableSet ## POSTFIX ## _(MemoryArena *arena, SerializableTable** table, const char* key, valueType value);
#define GenerateTableSet(POSTFIX, valueType, serializableType) void TableSet ## POSTFIX ## _(MemoryArena *arena, SerializableTable** table, const char* key, valueType value) \
{ \
    SerializableValue* tableValue = shget(*table, key); \
    if(tableValue) { \
        *((valueType*)tableValue->value) = value; \
    } \
    else { \
        char* keyPointer = PushString(arena, key); \
        SerializableValue* tableValue = PushStruct(arena, SerializableValue); \
        tableValue->value = PushSize(arena, sizeof(valueType)); \
        *((valueType*)tableValue->value) = value; \
        tableValue->type = serializableType; \
        tableValue->count = 1; \
        shput(*table, keyPointer, tableValue); \
    } \
}

#define TableSetString(arena, table, key, value) TableSetString_(arena, table, key, value)
#define TableSetBool(arena, table, key, value) TableSetBool_(arena, table, key, value)
#define TableSetI32(arena, table, key, value) TableSetI32_(arena, table, key, value)
#define TableSetF32(arena, table, key, value) TableSetF32_(arena, table, key, value)
#define TableSetV2(arena, table, key, value) TableSetV2_(arena, table, key, value)

#endif