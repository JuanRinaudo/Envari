#if !defined(DEFINES_H)
#define DEFINES_H

#if __EMSCRIPTEN__
    #if GAME_SLOW
        // TODO(Juan): Complete assert macro
        #define Assert(Expression) if(!(Expression)) { *(volatile int *)0 = 0; }
    #else
        #define Assert(Expression)
    #endif
#else
    #if GAME_SLOW
        // TODO(Juan): Complete assert macro
        #define Assert(Expression) if(!(Expression)) { *(int *)0 = 0; }
    #else
        #define Assert(Expression)
    #endif
#endif

#define InvalidCodePath Assert(!"InvalidCodePath");
#define InvalidDefaultCase default: {InvalidCodePath;} break;

// NOTE(Juan): Types

#include <stdint.h>
#include <stddef.h>
#include <float.h>

#define PI32 3.14159265359f

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef i32 b32;

typedef size_t memoryIndex;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define F32Max FLT_MAX
#define F64Max DBL_MAX

/*
    #NOTE (Juan):
    GAME_INTERNAL:
        0 - Build for public release
        1 - Build for development

    GAME_SLOW:
        0 - No slow code allowed
        1 - Slow debug code
*/

inline u32 SafeTruncateU64(u64 Value)
{
    // TODO(Juan): Defines for maximum values
    Assert(Value <= 0xFFFFFFFF);
    u32 Result = (u32)Value;
    return(Result);
}

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#endif