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

bool IsInRectangle(rectangle2 rectangle, v2 test)
{
    bool result = (
        test.x >= rectangle.x && test.y >= rectangle.y &&
        test.x < rectangle.x + rectangle.width && test.y < rectangle.y + rectangle.height
    );

    return(result);
}

bool RectangleOverlap(rectangle2 rectangle, rectangle2 otherRectangle)
{
    bool result = IsInRectangle(rectangle, V2(otherRectangle.x, otherRectangle.y)) ||
        IsInRectangle(rectangle, V2(otherRectangle.x, otherRectangle.y + otherRectangle.height)) ||
        IsInRectangle(rectangle, V2(otherRectangle.x + otherRectangle.width, otherRectangle.y)) ||
        IsInRectangle(rectangle, V2(otherRectangle.x + otherRectangle.width, otherRectangle.y + otherRectangle.height));

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
    result._11 = 1;
    result._22 = 1;

    return result;
}

m33 TranslationM33(f32 x, f32 y)
{
    m33 result = {};

    result._00 = 1;
    result._11 = 1;
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
    result._11 = y;
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
    result._10 = -Sin(angle);
    result._11 = Cos(angle);
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
    result._10 = a._10;
    result._11 = a._11;
    result._12 = a._12;
    result._22 = 1;
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
    result._11 = 1;
    result._22 = 1;
    result._33 = 1;

    return result;
}

m44 TranslationM44(f32 x, f32 y, f32 z)
{
    m44 result = {};

    result._00 = 1;
    result._11 = 1;
    result._22 = 1;
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
    result._11 = y;
    result._22 = z;
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
    m44 result = {};

    result._00 = 2 / (right - left);
    result._03 = -((right + left) / (right - left));
    result._11 = 2 / (top - bottom);
    result._13 = -((top + bottom) / (top - bottom));
    result._22 = -2 / (farPlane - nearPlane);
    result._23 = -((farPlane + nearPlane) / (farPlane - nearPlane));
    result._33 = 1;

    return result;
}

m44 OrtographicProjection(f32 size, f32 aspect, f32 nearPlane, f32 farPlane)
{
    size *= 0.5f;
    return OrtographicProjection(-size * aspect, size * aspect, -size, size, nearPlane, farPlane);
}

m44 PerspectiveProjection(f32 fovY, f32 aspect, f32 nearPlane, f32 farPlane)
{
    m44 result = {};

    result._00 = 1 / (aspect * Tan(fovY / 2));
    result._11 = 1 / Tan(fovY / 2);
    result._22 = -((farPlane + nearPlane) / (farPlane - nearPlane));
    result._23 = -((2 * farPlane * nearPlane) / (farPlane - nearPlane));
    result._32 = -1;

    return result;
}

// #NOTE(Juan): transform2D

transform2D Transform2D(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle)
{
    transform2D transform;
    transform.position = V2(posX, posY);
    transform.scale = V2(scaleX, scaleY);
    return transform;
}

// #NOTE(Juan): noise
// Hash lookup table as defined by Ken Perlin.  This is a randomly arranged array of all numbers from 0-255 inclusive.
static u32 NoiseP[] = {
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// static void MakePermutation(){
// 	for(u32 i = 0; i < 256; ++i){
// 		permutations[i] = i;
// 	}

// 	for(int i = count - 1; i > 0; --i){
// 		u32 index = Round(Random() * (i - 1));

//         u32 temp = array[i];
// 		array[i] = array[index];
// 		array[index] = temp;
// 	}
// }

static v2 NoiseGetConstantVector(u32 val) {
	u32 h = val & 3;
    switch(h) {
        case 0:
            return V2(1.0f, 1.0f);
        case 1:
            return V2(-1.0f, 1.0f);
        case 2:
            return V2(-1.0f, -1.0f);
        case 3:
            return V2(1.0f, -1.0f);
    }
    return V2(0, 0);
}

static f32 NoiseFade(f32 t) {
	return ((6.0f * t - 15.0f) * t + 10.0f) * t * t * t;
}

f32 Perlin2D(f32 x, f32 y) {
	u32 X = FloorToUInt(x) & 255;
	u32 Y = FloorToUInt(y) & 255;

	f32 xf = x - Floor(x);
	f32 yf = y - Floor(y);

	v2 topRight = V2(xf - 1.0f, yf - 1.0f);
	v2 topLeft = V2(xf, yf - 1.0f);
	v2 bottomRight = V2(xf - 1.0f, yf);
	v2 bottomLeft = V2(xf, yf);
	
	//Select a value in the array for each of the 4 corners
	u32 valueTopRight = NoiseP[NoiseP[X+1]+Y+1];
	u32 valueTopLeft = NoiseP[NoiseP[X]+Y+1];
	u32 valueBottomRight = NoiseP[NoiseP[X+1]+Y];
	u32 valueBottomLeft = NoiseP[NoiseP[X]+Y];
	
	f32 dotTopRight = Dot(topRight, NoiseGetConstantVector(valueTopRight));
	f32 dotTopLeft = Dot(topLeft, NoiseGetConstantVector(valueTopLeft));
	f32 dotBottomRight = Dot(bottomRight, NoiseGetConstantVector(valueBottomRight));
	f32 dotBottomLeft = Dot(bottomLeft, NoiseGetConstantVector(valueBottomLeft));
	
	f32 u = NoiseFade(xf);
	f32 v = NoiseFade(yf);
	
    f32 result = Lerp(u, Lerp(v, dotBottomLeft, dotTopLeft), Lerp(v, dotBottomRight, dotTopRight));
	return result;
}

f32 Perlin2D(i32 x, i32 y) {
    return Perlin2D(x + 0.5f, y + 0.5f);
}

f32 Perlin2DOctaves(f32 x, f32 y, u32 octaves = 8, f32 frecuency = 0.005) {
    f32 n = 0.0;
    f32 a = 1.0;

    for(u32 o = 0; o < octaves; ++o){
        f32 v = a * Perlin2D(x * frecuency, y * frecuency);
        n += v;
        
        a *= 0.5;
        frecuency *= 2.0;
    }

    return n;
}

#endif