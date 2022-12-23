#ifndef TYPE_DEFINES_H
#define TYPE_DEFINES_H

#include <inttypes.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>

#include <filesystem>
namespace filesystem = std::filesystem;

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

#endif