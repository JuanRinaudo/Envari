#if !defined(RENDER_H)
#define RENDER_H

#include "Game.h"
#include "Defines.h"
#include "GameMath.h"

enum RenderType
{
    type_RenderClear,
    type_RenderColor,
    type_RenderTriangle,
    type_RenderRectangle,
    type_RenderCircle,
    type_RenderImage,
    type_RenderText,
};

struct RenderHeader
{
    i32 id;
    RenderType type;
};

struct RenderClear
{
    RenderHeader header;
    v4 color;
};

struct RenderColor
{
    RenderHeader header;
    v4 color;
};

struct RenderTriangle
{
    RenderHeader header;
    v2 position;
    v2 point1;
    v2 point2;
    v2 point3;
};

struct RenderRectangle
{
    RenderHeader header;
    v2 position;
    v2 size;
};

struct RenderCircle
{
    RenderHeader header;
    v2 position;
    f32 radius;
    i32 segments;
};

struct RenderImage
{
    RenderHeader header;
    v2 position;
    v2 size;
    char* filename;
};

struct RenderText
{
    RenderHeader header;
    v2 position;
};

struct RenderState {
    int lastRenderID;
    v4 renderColor;
};

RenderState renderState;

static void Begin2D()
{
    renderState.lastRenderID = 0;
    renderState.renderColor = V4(1, 1, 1, 1);
    renderTemporaryMemory = BeginTemporaryMemory(&temporalState->temporalArena);
    
    RenderHeader *clearFirstHeader = (RenderHeader *)renderTemporaryMemory.arena->base;
    ZeroSize(sizeof(RenderHeader), clearFirstHeader);
}

#define PushRenderElement(arena, type) (type *)PushRenderElement_(arena, sizeof(type), type_##type);
static RenderHeader *PushRenderElement_(MemoryArena *arena, u32 size, RenderType type)
{
    RenderHeader *result = 0;
    renderState.lastRenderID++;

    if(arena->used + size < arena->size) {
        result = (RenderHeader *)PushSize(arena, size);
        result->id = renderState.lastRenderID;
        result->type = type;
    }
    else {
        InvalidCodePath;
    }

    return(result);
}

static void PushClear(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderClear *clear = PushRenderElement(renderTemporaryMemory.arena, RenderClear);
    clear->color = V4(red, green, blue, alpha);
}

static void PushColor(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderColor *color = PushRenderElement(renderTemporaryMemory.arena, RenderColor);
    color->color = V4(red, green, blue, alpha);
}

static void PushTriangle(v2 position, v2 point1, v2 point2, v2 point3)
{
    RenderTriangle *triangle = PushRenderElement(renderTemporaryMemory.arena, RenderTriangle);
    triangle->position = position;
    triangle->point1 = point1;
    triangle->point2 = point2;
    triangle->point3 = point3;
}

static void PushRectangle(v2 position, v2 size)
{
    RenderRectangle *rectangle = PushRenderElement(renderTemporaryMemory.arena, RenderRectangle);
    rectangle->position = position;
    rectangle->size = size;
}

static void PushCircle(v2 position, f32 radius, i32 segments)
{
    RenderCircle *circle = PushRenderElement(renderTemporaryMemory.arena, RenderCircle);
    circle->position = position;
    circle->radius = radius;
    circle->segments = segments;
}

static void PushImage(v2 position, v2 size, const char* filename)
{
    RenderImage *image = PushRenderElement(renderTemporaryMemory.arena, RenderImage);
    image->position = position;
    image->size = size;
    int namesize = strlen(filename);
    image->filename = (char*)PushSize(renderTemporaryMemory.arena, sizeof(char) * namesize);
    strcpy(image->filename, filename);
}

static void End2D()
{
    EndTemporaryMemory(renderTemporaryMemory);
}

#endif