#ifndef GAMEMATH_H
#define GAMEMATH_H

#include "Intrinsics.h"

v2 V2(f32 x, f32 y)
{
    v2 result;

    result.x = x;
    result.y = y;

    return(result);
}

v2i V2I(i32 x, i32 y)
{
    v2i result;

    result.x = x;
    result.y = y;

    return(result);
}

v3 V3(f32 x, f32 y, f32 z)
{
    v3 result;

    result.x = x;
    result.y = y;
    result.z = z;

    return(result);
}

v4 V4(f32 x, f32 y, f32 z, f32 w)
{
    v4 result;

    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;

    return(result);
}

// #NOTE(Juan): Scalar operations

f32 Lerp(f32 a, f32 b, f32 t)
{
    f32 result = (1.0f - t) * a + t * b;

    return(result);
}

f32 Clamp(f32 value, f32 min, f32 max)
{
    f32 result = value;

    if(result < min) {
        result = min;
    } else if(result > max) {
        result = max;
    }

    return(result);
}

f32 Clamp01(f32 value)
{
    f32 result = Clamp(value, 0, 1);

    return(result);
}

f32 SafeRatioN(f32 numerator, f32 divisor, f32 n)
{
    f32 result = n;

    if(divisor != 0.0f) {
        result = numerator / divisor;
    }

    return(result);
}

f32 SafeRatio0(f32 numerator, f32 divisor)
{
    f32 result = SafeRatioN(numerator, divisor, 0);

    return(result);
}

f32 SafeRatio1(f32 numerator, f32 divisor)
{
    f32 result = SafeRatioN(numerator, divisor, 1);

    return(result);
}

// #NOTE(Juan): v2 operations

v2 operator * (f32 a, v2 b)
{
    v2 result;

    result.x = a * b.x;
    result.y = a * b.y;

    return(result);
}

v2 operator * (v2 b, f32 a)
{
    v2 result = a * b;

    return(result);
}

v2 &operator *= (v2 &a, f32 b)
{
    a = b * a;

    return(a);
}

v2 operator - (v2 a)
{
    v2 result;

    result.x = -a.x;
    result.y = -a.y;

    return(result);
}

v2 operator + (v2 a, v2 b)
{
    v2 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return(result);
}

v2 &operator += (v2 &a, v2 b)
{
    a = a + b;

    return(a);
}

v2 operator - (v2 a, v2 b)
{
    v2 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return(result);
}

v2 &operator -= (v2 &a, v2 b)
{
    a = a - b;

    return(a);
}

v2 Hadamard(v2 a, v2 b)
{
    v2 result = {a.x * b.x, a.y * b.y};

    return(result);
}

v2 Perpendicular(v2 a) 
{
    v2 result = V2(-a.y, a.x);

    return(result);
}

f32 Dot(v2 a, v2 b)
{
    f32 result = a.x * b.x + a.y * b.y;

    return(result);
}

f32 LengthSq(v2 a)
{
    f32 result = Dot(a, a);

    return(result);
}

f32 Length(v2 a)
{
    f32 result = SquareRoot(LengthSq(a));

    return(result);
}

v2 Normalize(v2 a)
{
    f32 length = Length(a);
    v2 result = V2(a.x / length, a.y / length);

    return(result);
}

v2 Clamp01(v2 value)
{
    v2 result;

    result.x = Clamp01(value.x);
    result.y = Clamp01(value.y);

    return(result);
}

v2 Floor(v2 value) {
    v2 result;

    result.x = Floor(value.x);
    result.y = Floor(value.y);

    return result;
}

// #NOTE(Juan): v3 operations

v3 operator * (f32 a, v3 b)
{
    v3 result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;

    return(result);
}

v3 operator * (v3 b, f32 a)
{
    v3 result = a * b;

    return(result);
}

v3 &operator *= (v3 &a, f32 b)
{
    a = b * a;

    return(a);
}

v3 operator - (v3 a)
{
    v3 result;

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return(result);
}

v3 operator + (v3 a, v3 b)
{
    v3 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return(result);
}

v3 &operator += (v3 &a, v3 b)
{
    a = a + b;

    return(a);
}

v3 operator - (v3 a, v3 b)
{
    v3 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return(result);
}

v3 &operator -= (v3 &a, v3 b)
{
    a = a - b;

    return(a);
}

v3 Hadamard(v3 a, v3 b)
{
    v3 result = {a.x * b.x, a.y * b.y, a.z * b.z};

    return(result);
}

f32 Dot(v3 a, v3 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z;

    return(result);
}

f32 LengthSq(v3 a)
{
    f32 result = Dot(a, a);

    return(result);
}

f32 Length(v3 a)
{
    f32 result = SquareRoot(LengthSq(a));

    return(result);
}

v3 Normalize(v3 a)
{
    f32 length = Length(a);
    v3 result = V3(a.x / length, a.y / length, a.z / length);

    return(result);
}

v3 Clamp01(v3 value)
{
    v3 result;

    result.x = Clamp01(value.x);
    result.y = Clamp01(value.y);
    result.z = Clamp01(value.z);

    return(result);
}

// #NOTE(Juan): rectangle2

rectangle2 Rectangle2(f32 x, f32 y, f32 width, f32 height)
{
    rectangle2 result;

    result.x = x;
    result.y = y;
    result.width = width;
    result.height = height;

    return result;
}

rectangle2 Rectangle2MinMax(f32 minX, f32 minY, f32 maxX, f32 maxY)
{
    rectangle2 result;

    result.x = minX;
    result.y = minY;
    result.width = maxX - minX;
    result.height = maxY - minY;

    return(result);
}

v2 GetMinCorner(rectangle2 rectangle)
{
    v2 result = V2(rectangle.x, rectangle.y);
    return(result);
}

v2 GetMaxCorner(rectangle2 rectangle)
{
    v2 result = V2(rectangle.x + rectangle.width, rectangle.y + rectangle.height);
    return(result);
}

v2 GetSize(rectangle2 rectangle)
{
    v2 result = V2(rectangle.width, rectangle.height);
    return(result);
}

v2 GetCenter(rectangle2 rectangle)
{
    v2 result = V2(rectangle.x + rectangle.width * 0.5f, rectangle.y + rectangle.height * 0.5f);
    return(result);
}

b32 IsInRectangle(rectangle2 rectangle, v2 test)
{
    b32 result = (
        test.x >= rectangle.x && test.y >= rectangle.y &&
        test.x < rectangle.x + rectangle.width && test.y < rectangle.y + rectangle.height
    );

    return(result);
}

// #NOTE(Juan): matrix2x2

m22 M22(
    f32 _00, f32 _01,
    f32 _10, f32 _11)
{
    m22 result = {};

    result._00 = _00;
    result._01 = _01;
    result._10 = _10;
    result._11 = _11;

    return result;
}

m22 IdM22()
{
    m22 result = {};

    result._00 = 1;
    result._01 = 0;
    result._10 = 0;
    result._11 = 1;

    return result;
}

// #NOTE(Juan): matrix3x3

m33 M33(
    f32 _00, f32 _01, f32 _02,
    f32 _10, f32 _11, f32 _12,
    f32 _20, f32 _21, f32 _22)
{
    m33 result = {};

    result._00 = _00;
    result._01 = _01;
    result._02 = _02;
    result._10 = _10;
    result._11 = _11;
    result._12 = _12;
    result._20 = _20;
    result._21 = _21;
    result._22 = _22;

    return result;
}

m33 IdM33()
{
    m33 result = {};

    result._00 = 1;
    result._01 = 0;
    result._02 = 0;
    result._10 = 0;
    result._11 = 1;
    result._12 = 0;
    result._20 = 0;
    result._21 = 0;
    result._22 = 1;

    return result;
}

m33 TranslationM33(f32 x, f32 y)
{
    m33 result = {};

    result._00 = 1;
    result._01 = 0;
    result._02 = 0;
    result._10 = 0;
    result._11 = 1;
    result._12 = 0;
    result._20 = x;
    result._21 = y;
    result._22 = 1;

    return result;
}

m33 TranslationM33(v2 vector)
{
    return TranslationM33(vector.x, vector.y);
}

m33 ScaleM33(f32 x, f32 y)
{
    m33 result = {};

    result._00 = x;
    result._01 = 0;
    result._02 = 0;
    result._10 = 0;
    result._11 = y;
    result._12 = 0;
    result._20 = 0;
    result._21 = 0;
    result._22 = 1;

    return result;
}

m33 ScaleM33(v2 vector)
{
    return ScaleM33(vector.x, vector.y);
}

m33 operator * (m33 b, m33 a)
{
    m33 result;

    for(i32 x = 0; x < 3; ++x) {
        for(i32 y = 0; y < 3; ++y) {
            f32 value = 0;

            for(i32 i = 0; i < 3; ++i) {
                value += a.e[y * 3 + i] * b.e[3 * i + x];
            }  

            result.e[y * 3 + x] = value;
        }
    }

    return(result);
}

m33 &operator *= (m33 &a, m33 b)
{
    a = b * a;

    return(a);
}

m33 RotateM33(f32 angle)
{
    m33 result = {};

    result._00 = Cos(angle);
    result._01 = Sin(angle);
    result._02 = 0;
    result._10 = -Sin(angle);
    result._11 = Cos(angle);
    result._12 = 0;
    result._20 = 0;
    result._21 = 0;
    result._22 = 1;

    return result;
}

// #NOTE(Juan): matrix4x4

m44 M44(
    f32 _00, f32 _01, f32 _02, f32 _03,
    f32 _10, f32 _11, f32 _12, f32 _13,
    f32 _20, f32 _21, f32 _22, f32 _23,
    f32 _30, f32 _31, f32 _32, f32 _33)
{
    m44 result = {};

    result._00 = _00;
    result._01 = _01;
    result._02 = _02;
    result._03 = _03;
    result._10 = _10;
    result._11 = _11;
    result._12 = _12;
    result._13 = _13;
    result._20 = _20;
    result._21 = _21;
    result._22 = _22;
    result._23 = _23;
    result._30 = _30;
    result._31 = _31;
    result._32 = _32;
    result._33 = _33;

    return result;
}

m44 M44(m33 a)
{
    m44 result = {};

    result._00 = a._00;
    result._01 = a._01;
    result._02 = a._02;
    result._03 = 0;
    result._10 = a._10;
    result._11 = a._11;
    result._12 = a._12;
    result._13 = 0;
    result._20 = 0;
    result._21 = 0;
    result._22 = 1;
    result._23 = 0;
    result._30 = a._20;
    result._31 = a._21;
    result._32 = a._22;
    result._33 = 1;

    return result;
}

m44 IdM44()
{
    m44 result = {};

    result._00 = 1;
    result._01 = 0;
    result._02 = 0;
    result._03 = 0;
    result._10 = 0;
    result._11 = 1;
    result._12 = 0;
    result._13 = 0;
    result._20 = 0;
    result._21 = 0;
    result._22 = 1;
    result._23 = 0;
    result._30 = 0;
    result._31 = 0;
    result._32 = 0;
    result._33 = 1;

    return result;
}

m44 TranslationM44(f32 x, f32 y, f32 z)
{
    m44 result = {};

    result._00 = 1;
    result._01 = 0;
    result._02 = 0;
    result._03 = 0;
    result._10 = 0;
    result._11 = 1;
    result._12 = 0;
    result._13 = 0;
    result._20 = 0;
    result._21 = 0;
    result._22 = 1;
    result._23 = 0;
    result._30 = x;
    result._31 = y;
    result._32 = z;
    result._33 = 1;

    return result;
}

m44 TranslationM44(v2 vector)
{
    return TranslationM44(vector.x, vector.y, 0);
}

m44 TranslationM44(v3 vector)
{
    return TranslationM44(vector.x, vector.y, vector.z);
}

m44 ScaleM44(f32 x, f32 y, f32 z)
{
    m44 result = {};

    result._00 = x;
    result._01 = 0;
    result._02 = 0;
    result._03 = 0;
    result._10 = 0;
    result._11 = y;
    result._12 = 0;
    result._13 = 0;
    result._20 = 0;
    result._21 = 0;
    result._22 = z;
    result._23 = 0;
    result._30 = 0;
    result._31 = 0;
    result._32 = 0;
    result._33 = 1;

    return result;
}

m44 ScaleM44(v2 vector)
{
    return ScaleM44(vector.x, vector.y, 1);
}

m44 ScaleM44(v3 vector)
{
    return ScaleM44(vector.x, vector.y, vector.z);
}

m44 operator * (m44 b, m44 a)
{
    m44 result;

    for(i32 x = 0; x < 4; ++x) {
        for(i32 y = 0; y < 4; ++y) {
            f32 value = 0;

            for(i32 i = 0; i < 4; ++i) {
                value += a.e[y * 4 + i] * b.e[4 * i + x];
            }  

            result.e[y * 4 + x] = value;
        }
    }

    return(result);
}

m44 &operator *= (m44 &a, m44 b)
{
    a = b * a;

    return(a);
}

m44 OrtographicProjection(f32 left, f32 right, f32 top, f32 bottom, f32 nearPlane, f32 farPlane)
{
    return M44(
        2 / (right - left), 0, 0, -((right + left) / (right - left)),
        0, 2 / (top - bottom), 0, -((top + bottom) / (top - bottom)),
        0, 0, -2 / (farPlane - nearPlane), -((farPlane + nearPlane) / (farPlane - nearPlane)),
        0, 0, 0, 1
    );
}

m44 OrtographicProjection(f32 size, f32 aspect, f32 nearPlane, f32 farPlane)
{
    size *= 0.5f;
    return OrtographicProjection(-size * aspect, size * aspect, -size, size, nearPlane, farPlane);
}

m44 PerspectiveProjection(f32 fovY, f32 aspect, f32 nearPlane, f32 farPlane)
{
    return M44(
        1 / (aspect * Tan(fovY / 2)), 0, 0, 0,
        0, 1 / Tan(fovY / 2), 0, 0,
        0, 0, -((farPlane + nearPlane) / (farPlane - nearPlane)), -((2 * farPlane * nearPlane) / (farPlane - nearPlane)),
        0, 0, -1, 0
    );
}

// #NOTE(Juan): transform2D

transform2D Transform2D(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle)
{
    transform2D transform;
    transform.position = V2(posX, posY);
    transform.scale = V2(scaleX, scaleY);
    return transform;
}

#endif