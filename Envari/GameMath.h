#if !defined(GAMEMATH_H)
#define GAMEMATH_H

#include "Defines.h"
#include "Intrinsics.h"

union v2
{
    struct
    {
        f32 x, y;
    };
    f32 E[2];
};

union v3
{
    struct
    {
        f32 x, y, z;
    };
    struct
    {
        f32 r, g, b;
    };
    struct
    {
        v2 xy;
        f32 IgnoredZ_;
    };
    f32 E[3];
};

union v4
{
    struct
    {
        f32 x, y, z, w;
    };
    struct
    {
        f32 r, g, b, a;
    };
    f32 E[4];
};

static v2 V2(f32 X, f32 Y)
{
    v2 Result;

    Result.x = X;
    Result.y = Y;

    return(Result);
}

static v2 V2i(i32 X, i32 Y)
{
    v2 Result = V2((f32)X, (f32)Y);

    return(Result);
}

static v2 V2i(u32 X, u32 Y)
{
    v2 Result = V2((f32)X, (f32)Y);

    return(Result);
}

static v3 V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

static v4 V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;

    return(Result);
}

// NOTE(Juan): Scalar operations

static f32 Lerp(f32 A, f32 B, f32 t)
{
    f32 Result = (1.0f - t) * A + t * B;

    return(Result);
}

static f32 Clamp(f32 Value, f32 Min, f32 Max)
{
    f32 Result = Value;

    if(Result < Min) {
        Result = Min;
    } else if(Result > Max) {
        Result = Max;
    }

    return(Result);
}

static f32 Clamp01(f32 Value)
{
    f32 Result = Clamp(Value, 0, 1);

    return(Result);
}

static f32 SafeRatioN(f32 Numerator, f32 Divisor, f32 N)
{
    f32 Result = N;

    if(Divisor != 0.0f) {
        Result = Numerator / Divisor;
    }

    return(Result);
}

static f32 SafeRatio0(f32 Numerator, f32 Divisor)
{
    f32 Result = SafeRatioN(Numerator, Divisor, 0);

    return(Result);
}

static f32 SafeRatio1(f32 Numerator, f32 Divisor)
{
    f32 Result = SafeRatioN(Numerator, Divisor, 1);

    return(Result);
}

// NOTE(Juan): v2 operations

static v2 operator * (f32 A, v2 B)
{
    v2 Result;

    Result.x = A * B.x;
    Result.y = A * B.y;

    return(Result);
}

static v2 operator * (v2 B, f32 A)
{
    v2 Result = A * B;

    return(Result);
}

static v2 &operator *= (v2 &A, f32 B)
{
    A = B * A;

    return(A);
}

static v2 operator - (v2 A)
{
    v2 Result;

    Result.x = -A.x;
    Result.y = -A.y;

    return(Result);
}

static v2 operator + (v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return(Result);
}

static v2 &operator += (v2 &A, v2 B)
{
    A = A + B;

    return(A);
}

static v2 operator - (v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return(Result);
}

static v2 &operator -= (v2 &A, v2 B)
{
    A = A - B;

    return(A);
}

static v2 Hadamard(v2 A, v2 B)
{
    v2 Result = {A.x * B.x, A.y * B.y};

    return(Result);
}

static v2 Perpendicular(v2 A) 
{
    v2 Result = V2(-A.y, A.x);

    return(Result);
}

static f32 Dot(v2 A, v2 B)
{
    f32 Result = A.x * B.x + A.y * B.y;

    return(Result);
}

static f32 LengthSq(v2 A)
{
    f32 Result = Dot(A, A);

    return(Result);
}

static f32 Length(v2 A)
{
    f32 Result = SquareRoot(LengthSq(A));

    return(Result);
}

static v2 Clamp01(v2 Value)
{
    v2 Result;

    Result.x = Clamp01(Value.x);
    Result.y = Clamp01(Value.y);

    return(Result);
}

// NOTE(Juan): v3 operations

static v3 operator * (f32 A, v3 B)
{
    v3 Result;

    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;

    return(Result);
}

static v3 operator * (v3 B, f32 A)
{
    v3 Result = A * B;

    return(Result);
}

static v3 &operator *= (v3 &A, f32 B)
{
    A = B * A;

    return(A);
}

static v3 operator - (v3 A)
{
    v3 Result;

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;

    return(Result);
}

static v3 operator + (v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;

    return(Result);
}

static v3 &operator += (v3 &A, v3 B)
{
    A = A + B;

    return(A);
}

static v3 operator - (v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;

    return(Result);
}

static v3 &operator -= (v3 &A, v3 B)
{
    A = A - B;

    return(A);
}

static v3 Hadamard(v3 A, v3 B)
{
    v3 Result = {A.x * B.x, A.y * B.y, A.z * B.z};

    return(Result);
}

static f32 Dot(v3 A, v3 B)
{
    f32 Result = A.x * B.x + A.y * B.y + A.z * B.z;

    return(Result);
}

static f32 LengthSq(v3 A)
{
    f32 Result = Dot(A, A);

    return(Result);
}

static f32 Length(v3 A)
{
    f32 Result = SquareRoot(LengthSq(A));

    return(Result);
}

static v3 Clamp01(v3 Value)
{
    v3 Result;

    Result.x = Clamp01(Value.x);
    Result.y = Clamp01(Value.y);
    Result.z = Clamp01(Value.z);

    return(Result);
}

// NOTE(Juan): rectangle2

struct rectangle2
{
    v2 Min;
    v2 Max;
};

static v2 GetMinCorner(rectangle2 Rect)
{
    v2 Result = Rect.Min;
    return(Result);
}

static v2 GetMaxCorner(rectangle2 Rect)
{
    v2 Result = Rect.Max;    
    return(Result);
}

static v2 GetCenter(rectangle2 Rect)
{
    v2 Result = 0.5f * (Rect.Min + Rect.Max);
    return(Result);
}

static rectangle2 RectMinDim(v2 Min, v2 Dim)
{
    rectangle2 Result;

    Result.Min = Min;
    Result.Max = Min + Dim;

    return(Result);
}

static rectangle2 RectMinMax(v2 Min, v2 Max)
{
    rectangle2 Result;

    Result.Min = Min;
    Result.Max = Max;

    return(Result);
}

static rectangle2 RectCenterHalfDim(v2 Center, v2 HalfDim)
{
    rectangle2 Result;

    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;

    return(Result);
}

static rectangle2 RectCenterDim(v2 Center, v2 Dim)
{
    rectangle2 Result = RectCenterHalfDim(Center, Dim * 0.5f);

    return(Result);
}

static rectangle2 AddRadiusTo(rectangle2 Rectangle, v2 Radius)
{
    rectangle2 Result;

    Result.Min = Rectangle.Min - Radius;
    Result.Max = Rectangle.Max + Radius;

    return(Result);
}

static rectangle2 Offset(rectangle2 Rectangle, v2 Offset)
{
    rectangle2 Result;

    Result.Min = Rectangle.Min + Offset;
    Result.Max = Rectangle.Max + Offset;

    return(Result);
}

static b32 IsInRectangle(rectangle2 Rectangle, v2 Test)
{
    b32 Result = (
        Test.x >= Rectangle.Min.x && Test.y >= Rectangle.Min.y &&
        Test.x < Rectangle.Max.x && Test.y < Rectangle.Max.y
    );

    return(Result);
}

static v2 GetBarycentric(rectangle2 A, v2 Position)
{
    v2 Result;

    Result.x = SafeRatio0(Position.x - A.Min.x, A.Max.x - A.Min.x);
    Result.y = SafeRatio0(Position.y - A.Min.y, A.Max.y - A.Min.y);
    
    return(Result);
}

// NOTE(Juan): rectangle3

struct rectangle3
{
    v3 Min;
    v3 Max;
};

static v3 GetMinCorner(rectangle3 Rect)
{
    v3 Result = Rect.Min;
    return(Result);
}

static v3 GetMaxCorner(rectangle3 Rect)
{
    v3 Result = Rect.Max;    
    return(Result);
}

static v3 GetCenter(rectangle3 Rect)
{
    v3 Result = 0.5f * (Rect.Min + Rect.Max);
    return(Result);
}

static rectangle3 RectMinDim(v3 Min, v3 Dim)
{
    rectangle3 Result;

    Result.Min = Min;
    Result.Max = Min + Dim;

    return(Result);
}

static rectangle3 RectMinMax(v3 Min, v3 Max)
{
    rectangle3 Result;

    Result.Min = Min;
    Result.Max = Max;

    return(Result);
}

static rectangle3 RectCenterHalfDim(v3 Center, v3 HalfDim)
{
    rectangle3 Result;

    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;

    return(Result);
}

static rectangle3 RectCenterDim(v3 Center, v3 Dim)
{
    rectangle3 Result = RectCenterHalfDim(Center, Dim * 0.5f);

    return(Result);
}

static rectangle3 AddRadiusTo(rectangle3 Rectangle, v3 Radius)
{
    rectangle3 Result;

    Result.Min = Rectangle.Min - Radius;
    Result.Max = Rectangle.Max + Radius;

    return(Result);
}

static rectangle3 Offset(rectangle3 Rectangle, v3 Offset)
{
    rectangle3 Result;

    Result.Min = Rectangle.Min + Offset;
    Result.Max = Rectangle.Max + Offset;

    return(Result);
}

static b32 IsInRectangle(rectangle3 Rectangle, v3 Test)
{
    b32 Result = (
        Test.x >= Rectangle.Min.x && Test.y >= Rectangle.Min.y && Test.z >= Rectangle.Min.z &&
        Test.x < Rectangle.Max.x && Test.y < Rectangle.Max.y && Test.z < Rectangle.Max.z
    );

    return(Result);
}

static v3 GetBarycentric(rectangle3 A, v3 Position)
{
    v3 Result;

    Result.x = SafeRatio0(Position.x - A.Min.x, A.Max.x - A.Min.x);
    Result.y = SafeRatio0(Position.y - A.Min.y, A.Max.y - A.Min.y);
    Result.z = SafeRatio0(Position.z - A.Min.z, A.Max.z - A.Min.z);
    
    return(Result);
}

static rectangle2 ToRectangleXY(rectangle3 A)
{
    rectangle2 Result;

    Result.Min = A.Min.xy;
    Result.Max = A.Max.xy;

    return(Result);
}

static b32 RectanglesIntersect(rectangle3 A, rectangle3 B)
{
    b32 Result = !(B.Max.x <= A.Min.x || B.Min.x >= A.Max.x ||
        B.Max.y <= A.Min.y || B.Min.y >= A.Max.y || 
        B.Max.z <= A.Min.z || B.Min.z >= A.Max.z);

    return(Result);
}

// NOTE(Juan): matrix2x2

union m22
{
    struct
    {
        f32 _00, _01,
            _10, _11;
    };
    f32 E[4];
};

static m22 M22(
    f32 _00, f32 _01,
    f32 _10, f32 _11)
{
    m22 Result = {};

    Result._00 = _00;
    Result._01 = _01;
    Result._10 = _10;
    Result._11 = _11;

    return Result;
}

static m22 IdM22()
{
    m22 Result = {};

    Result._00 = 1;
    Result._01 = 0;
    Result._10 = 0;
    Result._11 = 1;

    return Result;
}

// NOTE(Juan): matrix3x3

union m33
{
    struct
    {
        f32 _00, _01, _02,
            _10, _11, _12,
            _20, _21, _22;
    };
    f32 E[9];
};

static m33 M33(
    f32 _00, f32 _01, f32 _02,
    f32 _10, f32 _11, f32 _12,
    f32 _20, f32 _21, f32 _22)
{
    m33 Result = {};

    Result._00 = _00;
    Result._01 = _01;
    Result._02 = _02;
    Result._10 = _10;
    Result._11 = _11;
    Result._12 = _12;
    Result._20 = _20;
    Result._21 = _21;
    Result._22 = _22;

    return Result;
}

static m33 IdM33()
{
    m33 Result = {};

    Result._00 = 1;
    Result._01 = 0;
    Result._02 = 0;
    Result._10 = 0;
    Result._11 = 1;
    Result._12 = 0;
    Result._20 = 0;
    Result._21 = 0;
    Result._22 = 1;

    return Result;
}

// NOTE(Juan): matrix4x4

union m44
{
    struct
    {
        f32 _00, _01, _02, _03,
            _10, _11, _12, _13,
            _20, _21, _22, _23,
            _30, _31, _32, _33;
    };
    f32 E[16];
};

static m44 M44(
    f32 _00, f32 _01, f32 _02, f32 _03,
    f32 _10, f32 _11, f32 _12, f32 _13,
    f32 _20, f32 _21, f32 _22, f32 _23,
    f32 _30, f32 _31, f32 _32, f32 _33)
{
    m44 Result = {};

    Result._00 = _00;
    Result._01 = _01;
    Result._02 = _02;
    Result._03 = _03;
    Result._10 = _10;
    Result._11 = _11;
    Result._12 = _12;
    Result._13 = _13;
    Result._20 = _20;
    Result._21 = _21;
    Result._22 = _22;
    Result._23 = _23;
    Result._30 = _30;
    Result._31 = _31;
    Result._32 = _32;
    Result._33 = _33;

    return Result;
}

static m44 IdM44()
{
    m44 Result = {};

    Result._00 = 1;
    Result._01 = 0;
    Result._02 = 0;
    Result._03 = 0;
    Result._10 = 0;
    Result._11 = 1;
    Result._12 = 0;
    Result._13 = 0;
    Result._20 = 0;
    Result._21 = 0;
    Result._22 = 1;
    Result._23 = 0;
    Result._30 = 0;
    Result._31 = 0;
    Result._32 = 0;
    Result._33 = 1;

    return Result;
}

static m44 OrtographicProyection(f32 left, f32 right, f32 top, f32 bottom, f32 nearPlane, f32 farPlane)
{
    return M44(
        2 / (right - left), 0, 0, -((right + left) / (right - left)),
        0, 2 / (top - bottom), 0, -((top + bottom) / (top - bottom)),
        0, 0, -2 / (farPlane - nearPlane), -((farPlane + nearPlane) / (farPlane - nearPlane)),
        0, 0, 0, 1
    );
}

static m44 OrtographicProyection(f32 size, f32 aspect, f32 nearPlane, f32 farPlane)
{
    return OrtographicProyection(-size * aspect, size * aspect, -size, size, nearPlane, farPlane);
}

static m44 PerspectiveProyection(f32 fovY, f32 aspect, f32 nearPlane, f32 farPlane)
{
    return M44(
        1 / (aspect * Tan(fovY / 2)), 0, 0, 0,
        0, 1 / Tan(fovY / 2), 0, 0,
        0, 0, -((farPlane + nearPlane) / (farPlane - nearPlane)), -((2 * farPlane * nearPlane) / (farPlane - nearPlane)),
        0, 0, -1, 0
    );
}

#endif