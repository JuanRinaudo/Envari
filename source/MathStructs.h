#ifndef MATHSTRUCTS_H
#define MATHSTRUCTS_H

union v2
{
    struct
    {
        f32 x, y;
    };
    f32 e[2];
};

union v2i
{
    struct
    {
        i32 x, y;
    };
    i32 e[2];
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
    f32 e[3];
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
    f32 e[4];
};

union m22
{
    struct
    {
        f32 _00, _01,
            _10, _11;
    };
    f32 e[4];
};

union m33
{
    struct
    {
        f32 _00, _01, _02,
            _10, _11, _12,
            _20, _21, _22;
    };
    f32 e[9];
};

union m44
{
    struct
    {
        f32 _00, _01, _02, _03,
            _10, _11, _12, _13,
            _20, _21, _22, _23,
            _30, _31, _32, _33;
    };
    f32 e[16];
};

struct rectangle2
{
    f32 x;
    f32 y;
    f32 width;
    f32 height;
};

struct transform2D
{
    v2 position;
    v2 scale;
};

#endif