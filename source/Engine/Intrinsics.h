#ifndef INTRINSICS_H
#define INTRINSICS_H

// TODO(Juan): Intrinsic?
// TODO(Juan): Convert all of this to platform specific math function and remove math.h

#include <math.h>

static i32 Sign(i32 Value)
{
    i32 Result = Value > 0 ? 1 : -1;
    return(Result);
}

static f32 Abs(f32 Value)
{
    f32 Result = fabsf(Value);
    return(Result);
}

static f32 Mod(f32 Value, f32 Mod)
{
    f32 Result = fmodf(Value, Mod);
    return Result;
}

static u32 RotateLeft(u32 Value, i32 Amount)
{
#if COMPILER_MSVC
    u32 Result = _rotl(Value, Amount);
#else
    // TODO(Juan): Port this to other platforms
    Amount &= 31;
    u32 Result = (Value << Amount) | (Value >> (32 - Amount));
#endif

    return(Result);
}

static u32 RotateRight(u32 Value, i32 Amount)
{
#if COMPILER_MSVC
    u32 Result = _rotr(Value, Amount);
#else
    // TODO(Juan): Port this to other platforms
    Amount &= 31;
    u32 Result = (Value >> Amount) | (Value << (32 - Amount));
#endif

    return(Result);
}

static f32 Floor(f32 Value)
{
    f32 Result = floorf(Value);
    return(Result);
}

static f32 Ceil(f32 Value)
{
    f32 Result = ceilf(Value);
    return(Result);
}

static i32 FloorToInt(f32 Value)
{
    i32 Result = (i32)floorf(Value);
    return(Result);
}

static u32 FloorToUInt(f32 Value)
{
    u32 Result = (u32)floorf(Value);
    return(Result);
}

static i32 CeilToInt(f32 Value)
{
    i32 Result = (i32)ceilf(Value);
    return(Result);
}

static i32 TruncateToInt(f32 Value)
{
    i32 Result = (i32)Value;
    return(Result);
}

static f32 Round(f32 Value)
{
    f32 Result = roundf(Value);
    return(Result);
}

static i32 RoundToInt(f32 Value)
{
    i32 Result = (i32)roundf(Value);
    return(Result);
}

static u32 RoundToUInt(f32 Value)
{
    i32 Result = (u32)roundf(Value);
    return(Result);
}

static f32 Sin(f32 Angle)
{
    f32 Result = sinf(Angle);
    return(Result);
}

static f32 Cos(f32 Angle)
{
    f32 Result = cosf(Angle);
    return(Result);
}

static f32 Tan(f32 Angle)
{
    f32 Result = tanf(Angle);
    return(Result);
}

static f32 Atan2(f32 Y, f32 X)
{
    f32 Result = atan2f(Y, X);
    return(Result);
}

static f32 Square(f32 Value)
{
    f32 Result = Value * Value;
    return(Result);
}

static f32 SquareRoot(f32 Value)
{
    f32 Result = sqrtf(Value);

    return(Result);
}

#endif