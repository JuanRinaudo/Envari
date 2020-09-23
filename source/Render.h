#ifndef RENDER_H
#define RENDER_H

void Begin2D(u32 frameBufferID, u32 width, u32 height)
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

void DrawClear(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderClear *clear = RenderPushElement(&renderTemporaryMemory, RenderClear);
    clear->color = V4(red, green, blue, alpha);
}

void DrawColor(f32 red = 0, f32 green = 0, f32 blue = 0, f32 alpha = 1)
{
    RenderColor *color = RenderPushElement(&renderTemporaryMemory, RenderColor);
    color->color = V4(red, green, blue, alpha);
}

void DrawLineWidth(f32 width = 1)
{
    RenderLineWidth *line = RenderPushElement(&renderTemporaryMemory, RenderLineWidth);
    line->width = width;
}

void DrawTransparent(u32 modeRGB, u32 modeAlpha, u32 srcRGB, u32 dstRGB, u32 srcAlpha, u32 dstAlpha)
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

void DrawTransparentDisable()
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

void DrawLine(f32 startX, f32 startY, f32 endX, f32 endY)
{
    RenderLine *line = RenderPushElement(&renderTemporaryMemory, RenderLine);
    line->start = V2(startX, startY);
    line->end = V2(endX, endY);
}

void DrawTriangle(f32 p1X, f32 p1Y, f32 p2X, f32 p2Y, f32 p3X, f32 p3Y)
{
    RenderTriangle *triangle = RenderPushElement(&renderTemporaryMemory, RenderTriangle);
    triangle->point1 = V2(p1X, p1Y);
    triangle->point2 = V2(p2X, p2Y);
    triangle->point3 = V2(p3X, p3Y);
}

void DrawRectangle(f32 posX, f32 posY, f32 scaleX, f32 scaleY)
{
    RenderRectangle *rectangle = RenderPushElement(&renderTemporaryMemory, RenderRectangle);
    rectangle->position = V2(posX, posY);
    rectangle->scale = V2(scaleX, scaleY);
}

void DrawCircle(f32 posX, f32 posY, f32 radius, i32 segments)
{
    RenderCircle *circle = RenderPushElement(&renderTemporaryMemory, RenderCircle);
    circle->position = V2(posX, posY);
    circle->radius = radius;
    circle->segments = segments;
}

void DrawTextureParameters(u32 wrapS, u32 wrapT, u32 minFilter, u32 magFilter)
{
    RenderTextureParameters *textureParameters = RenderPushElement(&renderTemporaryMemory, RenderTextureParameters);
    textureParameters->wrapS = wrapS;
    textureParameters->wrapT = wrapT;
    textureParameters->minFilter = minFilter;
    textureParameters->magFilter = magFilter;
}

void DrawTexture(f32 posX, f32 posY, f32 scaleX, f32 scaleY, u32 textureID)
{
    RenderTexture *texture = RenderPushElement(&renderTemporaryMemory, RenderTexture);
    texture->position = V2(posX, posY);
    texture->scale = V2(scaleX, scaleY);
    texture->textureID = textureID;
}

void DrawImage(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char* filename, u32 renderFlags = 0)
{
    RenderImage *image = RenderPushElement(&renderTemporaryMemory, RenderImage);
    image->header.renderFlags = renderFlags;
    image->position = V2(posX, posY);
    image->scale = V2(scaleX, scaleY);
    image->filename = PushString(&renderTemporaryMemory, filename, &image->filenameSize);
}

void DrawImageUV(f32 posX, f32 posY, f32 scaleX, f32 scaleY, rectangle2 uv, const char* filename)
{
    RenderImageUV *image = RenderPushElement(&renderTemporaryMemory, RenderImageUV);
    image->position = V2(posX, posY);
    image->scale = V2(scaleX, scaleY);
    image->uv = uv;
    image->filename = PushString(&renderTemporaryMemory, filename, &image->filenameSize);
}

void DrawAtlasSprite(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char* filename, const char* atlasName, const char* key)
{
    RenderAtlasSprite *atlas = RenderPushElement(&renderTemporaryMemory, RenderAtlasSprite);
    atlas->position = V2(posX, posY);
    atlas->scale = V2(scaleX, scaleY);
    atlas->filename = PushString(&renderTemporaryMemory, filename, &atlas->filenameSize);
    atlas->atlasName = PushString(&renderTemporaryMemory, atlasName, &atlas->atlasNameSize);
    atlas->spriteKey = PushString(&renderTemporaryMemory, key, &atlas->spriteKeySize);
}

void DrawFont(const char* filename, f32 fontSize, u32 width, u32 height)
{
    RenderFont *font = RenderPushElement(&renderTemporaryMemory, RenderFont);
    font->filename = PushString(&renderTemporaryMemory, filename, &font->filenameSize);
    font->fontSize = fontSize;
    font->width = width;
    font->height = height;
}

void DrawChar(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char singleChar)
{
    RenderChar *renderChar = RenderPushElement(&renderTemporaryMemory, RenderChar);
    renderChar->position = V2(posX, posY);
    renderChar->scale = V2(scaleX, scaleY);
    renderChar->singleChar = singleChar;
}

void DrawString(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char* string)
{
    RenderText *text = RenderPushElement(&renderTemporaryMemory, RenderText);
    text->position = V2(posX, posY);
    text->scale = V2(scaleX, scaleY);
    text->string = PushString(&renderTemporaryMemory, string, &text->stringSize);
}

void DrawOverrideVertices(f32* vertices, u32 count)
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

void DrawOverrideIndices(u32* indices, u32 count)
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

void End2D()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    EndTemporaryMemory(&renderTemporaryMemory);
}

v2 ScreenToViewport(f32 screenX, f32 screenY, f32 size, f32 ratio)
{
    v2 position = V2(0, 0);

    f32 scaleDifference = (f32)gameState->screen.height / (f32)gameState->screen.bufferHeight;
    f32 scaledWidth = (f32)gameState->screen.bufferWidth * scaleDifference;
    f32 offsetX = ((f32)gameState->screen.width - scaledWidth) * 0.5f;
    
    position.x = (((screenX - offsetX) / scaledWidth) - 0.5f) * size * ratio;
    position.y = ((screenY / gameState->screen.height) - 0.5f) * size;

    return position;
}

#endif