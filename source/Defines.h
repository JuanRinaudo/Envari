#ifndef DEFINES_H
#define DEFINES_H

#define InvalidCodePath Assert(!"InvalidCodePath");
#define InvalidDefaultCase default: {InvalidCodePath;} break;

// NOTE(Juan): Types

#include <stdint.h>
#include <stddef.h>
#include <float.h>

#define ENVARI_MAYOR_VERSION 0
#define ENVARI_MINOR_VERSION 1
#define ENVARI_MICRO_VERSION 0

#define PI32 3.14159265359f

#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) < (b) ? (b) : (a)
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define MOD(a, m) ((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m))
#define SQUARE(x) ((x) * (x))

#define KEY_COUNT 512
#define MOUSE_COUNT 8

#define DATA_MAX_TOKEN_COUNT 256

#define CONSOLE_INPUT_BUFFER_COUNT 256

#define TEXT_INPUT_BUFFER_COUNT 256

#define KEY_UP 0
#define KEY_RELEASED 1
#define KEY_PRESSED 2
#define KEY_DOWN 3

#define SPECIAL_ASCII_CHAR_OFFSET 32
#define FONT_CHAR_SIZE 96

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef i32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define F32Max FLT_MAX
#define F64Max DBL_MAX

#ifndef SOUND_MIX_SIZE
#define SOUND_MIX_SIZE       32
#endif
#ifndef SOUND_FORMAT
#define SOUND_FORMAT         ma_format_f32
#endif
#ifndef SOUND_CHANNELS
#define SOUND_CHANNELS       2
#endif
#ifndef SOUND_SAMPLE_RATE
#define SOUND_SAMPLE_RATE    48000
#endif

#ifdef PLATFORM_WASM
#ifndef DEFAULT_WRAP_S
#define DEFAULT_WRAP_S GL_CLAMP_TO_EDGE
#endif
#ifndef DEFAULT_WRAP_T
#define DEFAULT_WRAP_T GL_CLAMP_TO_EDGE
#endif
#ifndef DEFAULT_MIN_FILTER
#define DEFAULT_MIN_FILTER GL_LINEAR
#endif
#ifndef DEFAULT_MAX_FILTER
#define DEFAULT_MAX_FILTER GL_LINEAR
#endif
#else
#ifndef DEFAULT_WRAP_S
#define DEFAULT_WRAP_S GL_CLAMP_TO_EDGE
#endif
#ifndef DEFAULT_WRAP_T
#define DEFAULT_WRAP_T GL_CLAMP_TO_EDGE
#endif
#ifndef DEFAULT_MIN_FILTER
#define DEFAULT_MIN_FILTER GL_LINEAR_MIPMAP_LINEAR
#endif
#ifndef DEFAULT_MAX_FILTER
#define DEFAULT_MAX_FILTER GL_LINEAR_MIPMAP_LINEAR
#endif
#ifndef FRAMEBUFFER_DEFAULT_FILTER
#define FRAMEBUFFER_DEFAULT_FILTER GL_LINEAR
#endif
#endif

#if GAME_EDITOR
#define BUFFER_CHANNEL_TO_SHOW_SIZE SOUND_SAMPLE_RATE * 1
#define BUFFER_TO_SHOW_SIZE BUFFER_CHANNEL_TO_SHOW_SIZE * SOUND_CHANNELS

#define TIME_BUFFER_SIZE 1000
#endif

#if __EMSCRIPTEN__
    #ifdef GAME_SLOW
        // TODO(Juan): Complete assert macro
        #define Assert(Expression) if(!(Expression)) { *(volatile i32 *)0 = 0; }
    #else
        #define Assert(Expression)
    #endif
#else
    #ifdef GAME_SLOW
        // TODO(Juan): Complete assert macro
        #define Assert(Expression) if(!(Expression)) { *(i32 *)0 = 0; }
    #else
        #define Assert(Expression)
    #endif
#endif

/*
    #NOTE (Juan):
    GAME_EDITOR:
        0 - Build for public release
        1 - Build for development

    GAME_SLOW:
        0 - No slow code allowed
        1 - Slow debug code
*/

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

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

#define GenerateRenderTemporaryPush(PREFIX, type) static type* RenderTemporaryPush ## PREFIX (type value) \
{ \
    u32 size = sizeof(type); \
    if(renderTemporaryMemory.arena->used + size < renderTemporaryMemory.arena->size) { \
        type *valuePointer = (type*)PushSize(&renderTemporaryMemory, size); \
        *valuePointer = value; \
        return valuePointer; \
    } \
    else { \
        InvalidCodePath; \
        return 0; \
    } \
}

#endif