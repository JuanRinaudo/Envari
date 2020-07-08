#if !defined(RENDER_H)
#define RENDER_H

#include "Game.h"
#include "Defines.h"
#include "GameMath.h"

#define IMAGE_ADAPTATIVE_FIT 0x1
#define IMAGE_KEEP_RATIO_X 0x2
#define IMAGE_KEEP_RATIO_Y 0x4

enum RenderType
{
    type_RenderClear,
    type_RenderColor,
    type_RenderTransparent,
    type_RenderTriangle,
    type_RenderRectangle,
    type_RenderCircle,
    type_RenderImage,
    type_RenderImageUV,
    type_RenderFont,
    type_RenderChar,
    type_RenderText,
};

struct RenderHeader
{
    i32 id;
    RenderType type;
    u32 renderFlags;
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

struct RenderTransparent
{
    RenderHeader header;
    bool enabled;
    u32 modeRGB;
    u32 modeAlpha;
    u32 srcRGB;
    u32 dstRGB;
    u32 srcAlpha;
    u32 dstAlpha;
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
    u32 segments;
};

struct RenderImage
{
    RenderHeader header;
    v2 position;
    v2 size;
    char* filename;
    u32 filenameSize;
};

struct RenderImageUV
{
    RenderHeader header;
    v2 position;
    v2 size;
    rectangle2 uv;
    char* filename;
    u32 filenameSize;
};

struct RenderFont
{
    RenderHeader header;
    char* filename;
    u32 filenameSize;
    f32 fontSize;
    u32 width;
    u32 height;
};

struct RenderChar
{
    RenderHeader header;
    v2 position;
    v2 size;
    char singleChar;
};

struct RenderText
{
    RenderHeader header;
    v2 position;
    v2 size;
    char* string;
    u32 stringSize;
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
    renderTemporaryMemory = BeginTemporaryMemory(&temporalState->arena);
    
    RenderHeader *clearFirstHeader = (RenderHeader *)renderTemporaryMemory.arena->base;
    ZeroSize(sizeof(RenderHeader), clearFirstHeader);
}

#define PushRenderElement(arena, type) (type *)PushRenderElement_(arena, sizeof(type), type_##type);
static RenderHeader *PushRenderElement_(TemporaryMemory *memory, u32 size, RenderType type)
{
    RenderHeader *result = 0;
    renderState.lastRenderID++;

    if(memory->arena->used + size < memory->arena->size) {
        result = (RenderHeader *)PushSize(memory->arena, size);
        result->id = renderState.lastRenderID;
        result->type = type;
        memory->used += size;
    }
    else {
        InvalidCodePath;
    }

    return(result);
}

static char *PushRenderString(TemporaryMemory *memory, const char *string, u32 *stringSize)
{
    char *result = 0;
    renderState.lastRenderID++;

    u32 size = strlen(string) + 1;
    *stringSize = size;

    if(memory->arena->used + size < memory->arena->size) {
        result = (char*)PushSize(memory->arena, sizeof(char) * size);
        strcpy(result, string);
        memory->used += size;
    }
    else {
        InvalidCodePath;
    }

    return(result);
}

static void PushClear(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderClear *clear = PushRenderElement(&renderTemporaryMemory, RenderClear);
    clear->color = V4(red, green, blue, alpha);
}

static void PushColor(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderColor *color = PushRenderElement(&renderTemporaryMemory, RenderColor);
    color->color = V4(red, green, blue, alpha);
}

static void PushTransparentDisable()
{
    RenderTransparent *transparent = PushRenderElement(&renderTemporaryMemory, RenderTransparent);
    transparent->enabled = false;
    transparent->modeRGB = 0;
    transparent->modeAlpha = 0;
    transparent->srcRGB = 0;
    transparent->dstRGB = 0;
    transparent->srcAlpha = 0;
    transparent->dstAlpha = 0;
}

static void PushTransparent(u32 modeRGB, u32 modeAlpha, u32 srcRGB, u32 dstRGB, u32 srcAlpha, u32 dstAlpha)
{
    RenderTransparent *transparent = PushRenderElement(&renderTemporaryMemory, RenderTransparent);
    transparent->enabled = true;
    transparent->modeRGB = modeRGB;
    transparent->modeAlpha = modeAlpha;
    transparent->srcRGB = srcRGB;
    transparent->dstRGB = dstRGB;
    transparent->srcAlpha = srcAlpha;
    transparent->dstAlpha = dstAlpha;
}

static void PushTriangle(v2 position, v2 point1, v2 point2, v2 point3)
{
    RenderTriangle *triangle = PushRenderElement(&renderTemporaryMemory, RenderTriangle);
    triangle->position = position;
    triangle->point1 = point1;
    triangle->point2 = point2;
    triangle->point3 = point3;
}

static void PushRectangle(v2 position, v2 size)
{
    RenderRectangle *rectangle = PushRenderElement(&renderTemporaryMemory, RenderRectangle);
    rectangle->position = position;
    rectangle->size = size;
}

static void PushCircle(v2 position, f32 radius, i32 segments)
{
    RenderCircle *circle = PushRenderElement(&renderTemporaryMemory, RenderCircle);
    circle->position = position;
    circle->radius = radius;
    circle->segments = segments;
}

static void PushImage(v2 position, v2 size, const char* filename, u32 renderFlags)
{
    RenderImage *image = PushRenderElement(&renderTemporaryMemory, RenderImage);
    image->position = position;
    image->size = size;
    image->header.renderFlags = renderFlags;
    image->filename = PushRenderString(&renderTemporaryMemory, filename, &image->filenameSize);
}

static void PushImageUV(v2 position, v2 size, rectangle2 uv, const char* filename)
{
    RenderImageUV *image = PushRenderElement(&renderTemporaryMemory, RenderImageUV);
    image->position = position;
    image->size = size;
    image->uv = uv;
    image->filename = PushRenderString(&renderTemporaryMemory, filename, &image->filenameSize);
}

static void PushFont(const char* filename, f32 fontSize, u32 width, u32 height)
{
    RenderFont *font = PushRenderElement(&renderTemporaryMemory, RenderFont);
    font->filename = PushRenderString(&renderTemporaryMemory, filename, &font->filenameSize);
    font->fontSize = fontSize;
    font->width = width;
    font->height = height;
}

static void PushChar(v2 position, v2 size, const char singleChar)
{
    RenderChar *renderChar = PushRenderElement(&renderTemporaryMemory, RenderChar);
    renderChar->position = position;
    renderChar->size = size;
    renderChar->singleChar = singleChar;
}

static void PushText(v2 position, v2 size, const char* string)
{
    RenderText *text = PushRenderElement(&renderTemporaryMemory, RenderText);
    text->position = position;
    text->size = size;
    text->string = PushRenderString(&renderTemporaryMemory, string, &text->stringSize);
}

static void End2D()
{
    EndTemporaryMemory(&renderTemporaryMemory);
}

static v2 ScreenToViewport(m44 projection, v2 screenPosition)
{
    f32 x = screenPosition.x / gameState->screen.width;
    f32 y = screenPosition.y / gameState->screen.height;
    return V2(x, y);
}

static v2 ViewportToScreen(m44 projection, v2 viewportPosition)
{
    f32 x = viewportPosition.x * gameState->screen.width;
    f32 y = viewportPosition.y * gameState->screen.height;
    return V2(x, y);
}

    // if(projection._33 > 0) { // #NOTE(Juan): Ortographic projection

    // }
    // else { // #NOTE(Juan): Perspective projection
        
    // }

#endif