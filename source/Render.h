#ifndef RENDER_H
#define RENDER_H

#ifdef GAME_EDITOR
void RenderDebugStart()
{
    editorRenderDebugger.drawCount = 0;
    editorRenderDebugger.programChanges = 0;
}

void RenderDebugEnd()
{
    editorRenderDebugger.renderMemory = renderTemporaryMemory.used;
}
#endif

#define RenderPushElement(arena, type) (type *)RenderPushElement_(arena, sizeof(type), RenderType_##type);
static RenderHeader *RenderPushElement_(TemporaryMemory *memory, u32 size, RenderType type)
{
    RenderHeader *result = 0;
    renderState.lastRenderID++;

    if(memory->arena->used + size < memory->arena->size) {
        result = (RenderHeader *)PushSize(memory, size);
        result->id = renderState.lastRenderID;
        result->type = type;
        result->size = size;
    }
    else {
        InvalidCodePath;
    }

    return(result);
}

static char *RenderPushString(TemporaryMemory *memory, const char* string, u32 size)
{
    RenderHeader *header = RenderPushElement_(memory, sizeof(RenderTempData) + size + 1, RenderType_RenderTempData);
    char *data = ((char *)header) + sizeof(RenderTempData);
    strncpy(data, string, size);
    data[size] = 0;
    return data;
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

void DrawTransparent()
{
    RenderTransparent *transparent = RenderPushElement(&renderTemporaryMemory, RenderTransparent);
    transparent->enabled = true;
    transparent->modeRGB = GL_FUNC_ADD;
    transparent->modeAlpha = GL_FUNC_ADD;
    transparent->srcRGB = GL_SRC_ALPHA;
    transparent->dstRGB = GL_ONE_MINUS_SRC_ALPHA;
    transparent->srcAlpha = GL_ONE;
    transparent->dstAlpha = GL_ONE_MINUS_SRC_ALPHA;
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

void DrawSetLayer(u32 targetLayer, bool transparent)
{
    RenderLayer *layer = RenderPushElement(&renderTemporaryMemory, RenderLayer);
    layer->layer = targetLayer;

    renderState.usedLayers |= 1 << targetLayer;
    renderState.transparentLayers |= (transparent ? 1 : 0) << targetLayer;
}

void DrawLineWidth(f32 width = 1)
{
    RenderLineWidth *line = RenderPushElement(&renderTemporaryMemory, RenderLineWidth);
    line->width = width;
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

void DrawImage(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char* filepath, u32 renderFlags = 0)
{
    RenderImage *image = RenderPushElement(&renderTemporaryMemory, RenderImage);
    image->header.renderFlags = renderFlags;
    image->position = V2(posX, posY);
    image->scale = V2(scaleX, scaleY);
    image->filepath = PushString(&renderTemporaryMemory, filepath, &image->filepathSize);
    image->header.size += image->filepathSize;
}

void DrawImageUV(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 uvX, f32 uvY, f32 uvEndX, f32 uvEndY, const char* filepath)
{
    RenderImageUV *image = RenderPushElement(&renderTemporaryMemory, RenderImageUV);
    image->position = V2(posX, posY);
    image->scale = V2(scaleX, scaleY);
    image->uvMin = V2(uvX, uvY);
    image->uvMax = V2(uvEndX, uvEndY);
    image->filepath = PushString(&renderTemporaryMemory, filepath, &image->filepathSize);
    image->header.size += image->filepathSize;
}

void DrawImage9Slice(f32 posX, f32 posY, f32 endX, f32 endY, f32 slice, const char* filepath)
{
    RenderImage9Slice *image = RenderPushElement(&renderTemporaryMemory, RenderImage9Slice);
    image->position = V2(posX, posY);
    image->endPosition = V2(endX, endY);
    image->slice = slice;
    image->filepath = PushString(&renderTemporaryMemory, filepath, &image->filepathSize);
    image->header.size += image->filepathSize;
}

void DrawAtlasSprite(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char* filepath, const char* atlasName, const char* key)
{
    RenderAtlasSprite *atlas = RenderPushElement(&renderTemporaryMemory, RenderAtlasSprite);
    atlas->position = V2(posX, posY);
    atlas->scale = V2(scaleX, scaleY);
    atlas->filepath = PushString(&renderTemporaryMemory, filepath, &atlas->filepathSize);
    atlas->atlasName = PushString(&renderTemporaryMemory, atlasName, &atlas->atlasNameSize);
    atlas->spriteKey = PushString(&renderTemporaryMemory, key, &atlas->spriteKeySize);
    atlas->header.size += atlas->filepathSize + atlas->atlasNameSize + atlas->spriteKeySize;
}

void DrawSetFont(i32 fontID)
{
    RenderFont *font = RenderPushElement(&renderTemporaryMemory, RenderFont);
    font->fontID = fontID;
}

void DrawChar(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char singleChar)
{
    RenderChar *renderChar = RenderPushElement(&renderTemporaryMemory, RenderChar);
    renderChar->position = V2(posX, posY);
    renderChar->scale = V2(scaleX, scaleY);
    renderChar->singleChar = singleChar;
}

void DrawString(f32 posX, f32 posY, const char* string, u32 renderFlags)
{
    RenderText *text = RenderPushElement(&renderTemporaryMemory, RenderText);
    text->position = V2(posX, posY);
    text->string = PushString(&renderTemporaryMemory, string, &text->stringSize);
    text->header.size += text->stringSize;
}

void DrawStyledString(f32 posX, f32 posY, f32 endX, f32 endY, char* string, u32 renderFlags = 0)
{    
    RenderStyledText *text = RenderPushElement(&renderTemporaryMemory, RenderStyledText);
    text->header.renderFlags = renderFlags;
    text->position = V2(posX, posY);
    text->endPosition = V2(endX, endY);
    text->string = PushString(&renderTemporaryMemory, string, &text->stringSize);
    text->header.size += text->stringSize;
}

void DrawStyledString(f32 posX, f32 posY, f32 endX, f32 endY, const char* string, u32 renderFlags = 0)
{
    RenderStyledText *text = RenderPushElement(&renderTemporaryMemory, RenderStyledText);
    text->header.renderFlags = renderFlags;
    text->position = V2(posX, posY);
    text->endPosition = V2(endX, endY);
    text->string = PushString(&renderTemporaryMemory, string, &text->stringSize);
    text->header.size += text->stringSize;
}

void ClearInputBuffer()
{
    gameState->input.textInputIndex = 0;
    ZeroSize(TEXT_INPUT_BUFFER_COUNT, gameState->input.textInputBuffer);
}

bool DrawStringInput(f32 posX, f32 posY, f32 endX, f32 endY, const char* baseText, i32 maxSize = 0)
{
    if(maxSize == 0) {
        maxSize = TEXT_INPUT_BUFFER_COUNT;
    }

    if(gameState->input.textInputBuffer[0] != 0) {
        char* text = RenderPushString(&renderTemporaryMemory, gameState->input.textInputBuffer, maxSize);
        DrawStyledString(posX, posY, endX, endY, text, 0);
    }
    else {
        DrawColor(0.75f, 0.75f, 0.75f, 0.75f);
        DrawStyledString(posX, posY, endX, endY, baseText, 0);
    }

    bool capsLock = (SDL_GetModState() & KMOD_CAPS) > 0;

    char textShift = 0;
    if(capsLock || gameState->input.keyState[SDL_SCANCODE_LSHIFT] > KEY_PRESSED || gameState->input.keyState[SDL_SCANCODE_RSHIFT] > KEY_PRESSED) {
        textShift = -32;
    }

    int scancode = SDL_SCANCODE_A;
    while(scancode <= SDL_SCANCODE_KP_PERIOD) {
        if(gameState->input.keyState[scancode] == KEY_PRESSED) {
            char keyCode = (char)SDL_GetKeyFromScancode((SDL_Scancode)scancode);
            if(keyCode == '\b' && gameState->input.textInputIndex > 0) {
                --gameState->input.textInputIndex;
                gameState->input.textInputBuffer[gameState->input.textInputIndex] = 0;
            }
            else if(gameState->input.textInputIndex < maxSize && keyCode >= ' ' && keyCode <= '~') {
                if(keyCode >= 'a' && keyCode <= 'z') {
                    gameState->input.textInputBuffer[gameState->input.textInputIndex] = keyCode + textShift;
                }
                else {
                    gameState->input.textInputBuffer[gameState->input.textInputIndex] = keyCode;
                }
                ++gameState->input.textInputIndex;
            }
        }

        scancode++;
        if(scancode == SDL_SCANCODE_SLASH) { // #NOTE (Juan): Jump after slash to KP_DIVIDE
            scancode = SDL_SCANCODE_KP_DIVIDE;
        }
    }

    if(gameState->input.keyState[SDL_SCANCODE_RETURN]) {
        return true;
    }
    else {
        return false;
    }
}

void DrawSetUniform(u32 locationID, UniformType type)
{
    RenderSetUniform *uniform = RenderPushElement(&renderTemporaryMemory, RenderSetUniform);
    uniform->location = locationID;
    uniform->type = type;
    u32 size = 0;
    switch(type) {
        case UniformType_Float: { size = 4; break; }
        case UniformType_Vector2: { size = 8; break; }
    }
    uniform->header.size += size;
}

void DrawOverrideProgram(u32 programID)
{
    RenderOverrideProgram *program = RenderPushElement(&renderTemporaryMemory, RenderOverrideProgram);
    program->programID = programID;
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
    override->header.size += override->size;
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
    override->header.size += override->size;
}

bool DrawButton(f32 posX, f32 posY, f32 endX, f32 endY, f32 slice, const char* string, const char* buttonUp, const char* buttonDown)
{
    bool mouseOver = MouseOverRectangle(Rectangle2MinMax(posX, posY, endX, endY));
    bool mouseDown = gameState->input.mouseState[1] == KEY_DOWN;
    bool mouseReleased = gameState->input.mouseState[1] == KEY_RELEASED;
    DrawImage9Slice(posX, posY, endX, endY, slice, mouseOver && mouseDown ? buttonDown : buttonUp);
    DrawStyledString(posX, posY, endX, endY, string, TextRenderFlag_Center | TextRenderFlag_WordWrap);
    return mouseOver && mouseReleased;
}

i32 DrawMultibutton(f32 posX, f32 posY, f32 endX, f32 height, f32 slice, f32 yPadding, const char* options, const char* buttonUp, const char* buttonDown)
{
    i32 optionIndex = 0;
    i32 buttonCount = 0;
    i32 pressedIndex = -1;
    char* text = 0;
    while(options[optionIndex] != 0) {
        f32 buttonY = buttonCount * (height + yPadding);
        
        i32 searchIndex = optionIndex;
        while(options[searchIndex] != 0 && options[searchIndex] != '\r') {
            ++searchIndex;
        }
        u32 size = searchIndex - optionIndex;
        text = RenderPushString(&renderTemporaryMemory, options + optionIndex, size);
        optionIndex = searchIndex + 1;

        if(DrawButton(posX, posY + buttonY, endX, posY + buttonY + height, slice, text, buttonUp, buttonDown)) {
            pressedIndex = buttonCount;
        }

        ++buttonCount;
    }

    return pressedIndex;
}

void Begin2D(u32 frameBufferID, u32 width, u32 height)
{
    renderState.lastRenderID = 0;
    renderState.usedLayers = 0;
    renderState.transparentLayers = 0;
    renderState.renderColor = V4(1, 1, 1, 1);
    renderState.overrideProgram = 0;
    renderState.overridingVertices = false;
    renderState.overridingIndices = false;
    renderTemporaryMemory = BeginTemporaryMemory(&temporalState->arena);
    
    RenderHeader *clearFirstHeader = (RenderHeader *)renderTemporaryMemory.arena->base;
    ZeroSize(sizeof(RenderHeader), clearFirstHeader);
    
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	glViewport(0,0, width, height);
    
    DrawTextureParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    DrawSetLayer(0, true);
    if(gameState->render.defaultFontID != 0) {
        DrawSetFont(gameState->render.defaultFontID);
    }
}

void End2D()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    EndTemporaryMemory(&renderTemporaryMemory);
}

v2 RenderToViewport(f32 renderX, f32 renderY, f32 size, f32 ratio)
{
    v2 position = V2(0, 0);
    
    f32 scaleDifference = (f32)gameState->render.size.y / (f32)gameState->render.bufferSize.y;
    f32 scaledWidth = (f32)gameState->render.bufferSize.x * scaleDifference;
    f32 offsetX = ((f32)gameState->render.size.x - scaledWidth) * 0.5f;
    
    position.x = (((renderX - offsetX) / scaledWidth) - 0.5f) * size * ratio + size * 0.5f;
    position.y = ((renderY / gameState->render.size.y) - 0.5f) * size + size * 0.5f;

    return position;
}

#endif