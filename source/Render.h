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
    type_RenderTextureParameters,
    type_RenderTexture,
    type_RenderImage,
    type_RenderImageUV,
    type_RenderAtlasSprite,
    type_RenderFont,
    type_RenderChar,
    type_RenderText,
    type_RenderOverrideVertices,
    type_RenderOverrideIndices,
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
    v2 scale;
};

struct RenderCircle
{
    RenderHeader header;
    v2 position;
    f32 radius;
    u32 segments;
};

struct RenderTextureParameters
{
    RenderHeader header;
    u32 wrapS;
    u32 wrapT;
    u32 minFilter;
    u32 magFilter;
};

struct RenderTexture
{
    RenderHeader header;
    v2 position;
    v2 scale;
    u32 textureID;
};

struct RenderImage
{
    RenderHeader header;
    v2 position;
    v2 scale;
    char* filename;
    u32 filenameSize;
};

struct RenderImageUV
{
    RenderHeader header;
    v2 position;
    v2 scale;
    rectangle2 uv;
    char* filename;
    u32 filenameSize;
};

struct RenderAtlasSprite
{
    RenderHeader header;
    v2 position;
    v2 scale;
    char* filename;
    u32 filenameSize;
    char* atlasName;
    u32 atlasNameSize;
    char* spriteKey;
    u32 spriteKeySize;
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
    v2 scale;
    char singleChar;
};

struct RenderText
{
    RenderHeader header;
    v2 position;
    v2 scale;
    char* string;
    u32 stringSize;
};

struct RenderOverrideVertices
{
    RenderHeader header;
    f32* vertices;
    u32 size;
};

struct RenderOverrideIndices
{
    RenderHeader header;
    u32* indices;
    u32 size;
};

struct RenderState {
    i32 lastRenderID;
    v4 renderColor;
    u32 wrapS;
    u32 wrapT;
    u32 minFilter;
    u32 magFilter;
    bool overridingVertices;
    bool overridingIndices;
};

RenderState renderState;

static void Begin2D(u32 frameBufferID, u32 width, u32 height)
{
    renderState.lastRenderID = 0;
    renderState.renderColor = V4(1, 1, 1, 1);
    renderTemporaryMemory = BeginTemporaryMemory(&temporalState->arena);
    
    RenderHeader *clearFirstHeader = (RenderHeader *)renderTemporaryMemory.arena->base;
    ZeroSize(sizeof(RenderHeader), clearFirstHeader);
    
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	glViewport(0,0, width, height);
}

#define RenderPushElement(arena, type) (type *)RenderPushElement_(arena, sizeof(type), type_##type);
static RenderHeader *RenderPushElement_(TemporaryMemory *memory, u32 size, RenderType type)
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

static void PushRenderClear(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderClear *clear = RenderPushElement(&renderTemporaryMemory, RenderClear);
    clear->color = V4(red, green, blue, alpha);
}

static void PushRenderColor(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderColor *color = RenderPushElement(&renderTemporaryMemory, RenderColor);
    color->color = V4(red, green, blue, alpha);
}

static void PushRenderTransparent(u32 modeRGB, u32 modeAlpha, u32 srcRGB, u32 dstRGB, u32 srcAlpha, u32 dstAlpha)
{
    RenderTransparent *transparent = RenderPushElement(&renderTemporaryMemory, RenderTransparent);
    transparent->enabled = true;
    transparent->modeRGB = modeRGB;
    transparent->modeAlpha = modeAlpha;
    transparent->srcRGB = srcRGB;
    transparent->dstRGB = dstRGB;
    transparent->srcAlpha = srcAlpha;
    transparent->dstAlpha = dstAlpha;
}

static void PushRenderTransparentDisable()
{
    RenderTransparent *transparent = RenderPushElement(&renderTemporaryMemory, RenderTransparent);
    transparent->enabled = false;
    transparent->modeRGB = 0;
    transparent->modeAlpha = 0;
    transparent->srcRGB = 0;
    transparent->dstRGB = 0;
    transparent->srcAlpha = 0;
    transparent->dstAlpha = 0;
}

static void PushRenderTriangle(v2 position, v2 point1, v2 point2, v2 point3)
{
    RenderTriangle *triangle = RenderPushElement(&renderTemporaryMemory, RenderTriangle);
    triangle->position = position;
    triangle->point1 = point1;
    triangle->point2 = point2;
    triangle->point3 = point3;
}

static void PushRenderRectangle(v2 position, v2 scale)
{
    RenderRectangle *rectangle = RenderPushElement(&renderTemporaryMemory, RenderRectangle);
    rectangle->position = position;
    rectangle->scale = scale;
}

static void PushRenderCircle(v2 position, f32 radius, i32 segments)
{
    RenderCircle *circle = RenderPushElement(&renderTemporaryMemory, RenderCircle);
    circle->position = position;
    circle->radius = radius;
    circle->segments = segments;
}

static void PushRenderTextureParameters(u32 wrapS, u32 wrapT, u32 minFilter, u32 magFilter)
{
    RenderTextureParameters *textureParameters = RenderPushElement(&renderTemporaryMemory, RenderTextureParameters);
    textureParameters->wrapS = wrapS;
    textureParameters->wrapT = wrapT;
    textureParameters->minFilter = minFilter;
    textureParameters->magFilter = magFilter;
}

static void PushRenderTexture(v2 position, v2 scale, u32 textureID)
{
    RenderTexture *texture = RenderPushElement(&renderTemporaryMemory, RenderTexture);
    texture->position = position;
    texture->scale = scale;
    texture->textureID = textureID;
}

static void PushRenderImage(v2 position, v2 scale, const char* filename, u32 renderFlags = 0)
{
    RenderImage *image = RenderPushElement(&renderTemporaryMemory, RenderImage);
    image->header.renderFlags = renderFlags;
    image->position = position;
    image->scale = scale;
    image->filename = PushString(&renderTemporaryMemory, filename, &image->filenameSize);
}

static void PushRenderImageUV(v2 position, v2 scale, rectangle2 uv, const char* filename)
{
    RenderImageUV *image = RenderPushElement(&renderTemporaryMemory, RenderImageUV);
    image->position = position;
    image->scale = scale;
    image->uv = uv;
    image->filename = PushString(&renderTemporaryMemory, filename, &image->filenameSize);
}

static void PushRenderAtlasSprite(v2 position, v2 scale, const char* filename, const char* atlasName, const char* key)
{
    RenderAtlasSprite *atlas = RenderPushElement(&renderTemporaryMemory, RenderAtlasSprite);
    atlas->position = position;
    atlas->scale = scale;
    atlas->filename = PushString(&renderTemporaryMemory, filename, &atlas->filenameSize);
    atlas->atlasName = PushString(&renderTemporaryMemory, atlasName, &atlas->atlasNameSize);
    atlas->spriteKey = PushString(&renderTemporaryMemory, key, &atlas->spriteKeySize);
}

static void PushRenderFont(const char* filename, f32 fontSize, u32 width, u32 height)
{
    RenderFont *font = RenderPushElement(&renderTemporaryMemory, RenderFont);
    font->filename = PushString(&renderTemporaryMemory, filename, &font->filenameSize);
    font->fontSize = fontSize;
    font->width = width;
    font->height = height;
}

static void PushRenderChar(v2 position, v2 scale, const char singleChar)
{
    RenderChar *renderChar = RenderPushElement(&renderTemporaryMemory, RenderChar);
    renderChar->position = position;
    renderChar->scale = scale;
    renderChar->singleChar = singleChar;
}

static void PushRenderText(v2 position, v2 scale, const char* string)
{
    RenderText *text = RenderPushElement(&renderTemporaryMemory, RenderText);
    text->position = position;
    text->scale = scale;
    text->string = PushString(&renderTemporaryMemory, string, &text->stringSize);
}

static void PushRenderOverrideVertices(f32* vertices, u32 count)
{
    RenderOverrideVertices *override = RenderPushElement(&renderTemporaryMemory, RenderOverrideVertices);
    if(vertices) {
        override->size = sizeof(f32) * count;
        override->vertices = (f32*)PushSize(&renderTemporaryMemory, override->size);
        for(i32 i = 0; i < override->size; ++i) {
            override->vertices[i] = vertices[i];
        }
    }
    else {
        override->size = 0;
        override->vertices = 0;
    }
}

static void PushRenderOverrideIndices(u32* indices, u32 count)
{
    RenderOverrideIndices *override = RenderPushElement(&renderTemporaryMemory, RenderOverrideIndices);
    if(indices) {
        override->size = sizeof(u32) * count;
        override->indices = (u32*)PushSize(&renderTemporaryMemory, override->size);
        for(i32 i = 0; i < override->size; ++i) {
            override->indices[i] = indices[i];
        }
    }
    else { 
        override->size = 0;
        override->indices = 0;
    }
}

static void End2D()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    EndTemporaryMemory(&renderTemporaryMemory);
}

static v2 ScreenToViewport(v2 screenPosition, f32 size, f32 ratio)
{
    v2 position = V2(0, 0);

    f32 scaleDifference = (f32)gameState->screen.height / (f32)gameState->screen.bufferHeight;
    f32 scaledWidth = (f32)gameState->screen.bufferWidth * scaleDifference;
    f32 offsetX = ((f32)gameState->screen.width - scaledWidth) * 0.5f;
    
    position.x = (((screenPosition.x - offsetX) / scaledWidth) - 0.5f) * size * ratio;
    position.y = ((screenPosition.y / gameState->screen.height) - 0.5f) * size;

    return position;
}

#endif