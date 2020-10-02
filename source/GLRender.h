#ifndef GLRENDER_H
#define GLRENDER_H

#ifdef GL_PROFILE_GLES3
#include <GLES3/gl3.h>

const char* shaderPath = "shaders/gles";

#else
#include <gl/gl.h>

const char* shaderPath = "shaders/glcore";

#endif

static GLRenderBuffer quadBuffer;
static GLRenderBuffer overrideBuffer;
static GLRenderBuffer customBuffer;

static u32 DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

static u32 coloredProgram;
static u32 fontProgram;
static u32 texturedProgram;

static u32 colorLocation;
static u32 modelLocation;
static u32 viewLocation;
static u32 projectionLocation;
static u32 timeLocation;

static GLTextureCache* textureCache = NULL;

static GLTextureAtlasReference* atlasCache = NULL;

static FontAtlas currentFont;

static GLFontReference* fontCache = NULL;

enum TextStyles {
    style_Normal = 0,
    style_SineX = 1,
    style_SineY = 2,
    style_FadeIn = 3,
    style_FadeOut = 4,
    style_Typewriter = 5
};

enum GLVendor {
    GL_VENDOR_UNKOWN,
    GL_VENDOR_NVIDIA,
    GL_VENDOR_ATI
};
static GLVendor currentVendor = GL_VENDOR_UNKOWN;

#define GL_STRING_VENDOR_ATI "ATI Technologies Inc."
#define GL_STRING_VENDOR_NVIDIA "NVIDIA Corporation"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

// GLuint uNoOfGPUs = wglGetGPUIDsAMD( 0, 0 );
// GLuint* uGPUIDs = new GLuint[uNoOfGPUs];
// wglGetGPUIDsAMD( uNoOfGPUs, uGPUIDs );

#ifdef GAME_INTERNAL
static i32 watchedProgramsCount = 0;
static WatchedProgram watchedPrograms[50];
#endif

static bool GL_CheckVendor(const char* vendor)
{
    const char* glVendor = (char*)glGetString(GL_VENDOR);
    return strcmp(glVendor, vendor) == 0;
}

static i32 GL_TotalGPUMemoryKB()
{
    i32 totalMemoryInKB = 0;
    if(currentVendor == GL_VENDOR_NVIDIA) {
        glGetIntegerv( GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &totalMemoryInKB);
    }
    else if(currentVendor == GL_VENDOR_ATI) {
        // #TODO(Juan): Implement
        // wglGetGPUInfoAMD( uGPUIDs[0], WGL_GPU_RAM_AMD, GL_UNSIGNED_INT, sizeof( GLuint ), &uTotalMemoryInMB );
    }
    return totalMemoryInKB;
}

static i32 GL_AvailableGPUMemoryKB()
{
    i32 availableMemoryInKB = 0;    
    if(currentVendor == GL_VENDOR_NVIDIA) {
        glGetIntegerv( GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &availableMemoryInKB);
    }
    else if(currentVendor == GL_VENDOR_ATI) {
        // #TODO(Juan): Implement
        // glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI, &availableMemoryInKB);
    }
    return availableMemoryInKB;
}

static GLTexture GL_LoadTexture(const char *textureKey)
{
    i32 index = (i32)shgeti(textureCache, textureKey);
    if(index > -1) {
        return shget(textureCache, textureKey);
    } else {
        i32 width, height, channels;
        unsigned char *data = stbi_load(textureKey, &width, &height, &channels, 0);

        u32 textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        GLTexture texture;
        texture.textureID = textureID;
        texture.width = width;
        texture.height = height;
        texture.channels = channels;
        shput(textureCache, textureKey, texture);

        stbi_image_free(data);

        return texture;
    }
}

static TextureAtlas GL_LoadAtlas(const char *atlasKey)
{
    i32 index = (i32)shgeti(atlasCache, atlasKey);
    if(index > -1) {
        return shget(atlasCache, atlasKey);
    } else {
        TextureAtlas atlas;
        atlas.sprites = 0;

        DataTokenizer tokenizer = StartTokenizer(atlasKey);

        char* keyPointer = 0;
        while(tokenizer.active) {
            char* token = NextToken(&tokenizer);

            if(tokenizer.tokenLineCount == 0) {
                keyPointer = PushString(&permanentState->arena, tokenizer.tokenBuffer, tokenizer.tokenBufferIndex);

                i32 x = atoi(NextToken(&tokenizer));
                i32 y = atoi(NextToken(&tokenizer));
                i32 width = atoi(NextToken(&tokenizer));
                i32 height = atoi(NextToken(&tokenizer));
                
                shput(atlas.sprites, keyPointer, Rectangle2((f32)x, (f32)y, (f32)width, (f32)height));
            }
        }

        EndTokenizer(&tokenizer);

        shput(atlasCache, atlasKey, atlas);

        return atlas;
    }
}

static FontAtlas GL_LoadFont(const char *filepath, f32 fontSize, u32 width, u32 height)
{
    i32 index = (i32)shgeti(fontCache, filepath);
    if(index > -1) {
        return shget(fontCache, filepath); 
    } else {
        FontAtlas result;
        result.fontFilepath = PushString(&permanentState->arena, filepath, &result.fontFilepathSize);
        result.fontSize = fontSize;
        result.width = width;
        result.height = height;

        u32 data_size = 0;
        void* data = LoadFileToMemory(filepath, FILE_MODE_READ_BINARY, &data_size);

        u8* tempBitmap = PushArray(&temporalState->arena, width * height, u8);
        stbtt_BakeFontBitmap((u8 *)data, 0, fontSize, tempBitmap, width, height, 32, 96, result.charData); // no guarantee this fits!

        u32 textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tempBitmap);
        glGenerateMipmap(GL_TEXTURE_2D);

        GLTexture texture;
        texture.textureID = textureID;
        texture.width = width;
        texture.height = height;
        texture.channels = 4;
        shput(textureCache, filepath, texture);
        shput(fontCache, filepath, result);

        return result;
    }
}

f32 quadVertices[20];
f32* CreateQuadPosUV(f32 posStartX, f32 posStartY, f32 posEndX, f32 posEndY,
    f32 uvStartX, f32 uvStartY, f32 uvEndX, f32 uvEndY)
{
    quadVertices[0] = posStartX;
    quadVertices[1] = posStartY;
    quadVertices[2] = 0.0f;
    quadVertices[3] = uvStartX;
    quadVertices[4] = uvStartY;
    quadVertices[5] = posEndX;
    quadVertices[6] = posStartY;
    quadVertices[7] = 0.0f;
    quadVertices[8] = uvEndX;
    quadVertices[9] = uvStartY;
    quadVertices[10] = posStartX;
    quadVertices[11] = posEndY;
    quadVertices[12] = 0.0f;
    quadVertices[13] = uvStartX;
    quadVertices[14] = uvEndY;
    quadVertices[15] = posEndX;
    quadVertices[16] = posEndY;
    quadVertices[17] = 0.0f;
    quadVertices[18] = uvEndX;
    quadVertices[19] = uvEndY;
    return quadVertices;
}

u32 quadIndices[] = {
    0, 1, 2,
    1, 2, 3
};

static void GL_Init()
{
    glGenVertexArrays(1, &quadBuffer.vertexArray);
    glGenBuffers(1, &quadBuffer.vertexBuffer);
    glGenBuffers(1, &quadBuffer.indexBuffer);

    glGenVertexArrays(1, &overrideBuffer.vertexArray);
    glGenBuffers(1, &overrideBuffer.vertexBuffer);
    glGenBuffers(1, &overrideBuffer.indexBuffer);

    f32* vertices = CreateQuadPosUV(0, 0, 1, 1, 0, 0, 1, 1);
    glBindVertexArray(quadBuffer.vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, quadBuffer.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadBuffer.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW); 

    glGenVertexArrays(1, &customBuffer.vertexArray);
    glGenBuffers(1, &customBuffer.vertexBuffer);
    glGenBuffers(1, &customBuffer.indexBuffer);

    // #NOTE(Juan): Check GPU vendor
    if(GL_CheckVendor(GL_STRING_VENDOR_NVIDIA)) {
        currentVendor = GL_VENDOR_NVIDIA;
    }
    else if(GL_CheckVendor(GL_STRING_VENDOR_ATI)) {
        currentVendor = GL_VENDOR_ATI;
    }
}

static void GL_InitFramebuffer(i32 bufferWidth, i32 bufferHeight)
{
    glGenFramebuffers(1, &gameState->render.frameBuffer);
    glGenTextures(1, &gameState->render.renderBuffer);
    glGenRenderbuffers(1, &gameState->render.depthrenderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, gameState->render.frameBuffer);

    glBindTexture(GL_TEXTURE_2D, gameState->render.renderBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameState->render.renderBuffer, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, gameState->render.depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferWidth, bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gameState->render.depthrenderbuffer);

    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log(&editorConsole, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_UNSUPPORTED) {
        Log(&editorConsole, "ERROR::FRAMEBUFFER:: Framebuffer is not supported!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

i32 GL_CompileProgram(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    // NOTE(Juan): Shaders
    u32 vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    u32 data_size = 0;
    void* data = LoadFileToMemory(vertexShaderPath, FILE_MODE_READ_BINARY, &data_size);
    if(data == 0) {
        LogError(&editorConsole, "ERROR::VERTEX::FileLoad failed %s", vertexShaderPath);
        return 0;
    }
    SOURCE_TYPE vertexSource = static_cast<SOURCE_TYPE>(data);
    
    i32 size = (i32)data_size;
    glShaderSource(vertexShader, 1, &vertexSource, &size);
    glCompileShader(vertexShader);

    i32 success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LogError(&editorConsole, "ERROR::VERTEX::COMPILATION_FAILED %s", vertexShaderPath);
        LogError(&editorConsole, infoLog);
    }

    u32 fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    data = LoadFileToMemory(fragmentShaderPath, FILE_MODE_READ_BINARY, &data_size);
    if(data == 0) {
        LogError(&editorConsole, "ERROR::FRAGMENT::FileLoad failed %s", fragmentShaderPath);
        return 0;
    }
    SOURCE_TYPE fragmentSource = static_cast<SOURCE_TYPE>(data);

    size = (i32)data_size;
    glShaderSource(fragmentShader, 1, &fragmentSource, &size);
    glCompileShader(fragmentShader);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LogError(&editorConsole, "ERROR::FRAGMENT::COMPILATION_FAILED %s", fragmentShaderPath);
        LogError(&editorConsole, infoLog);
    }

    i32 shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        LogError(&editorConsole, "ERROR::PROGRAM::LINK_FAILED");
        LogError(&editorConsole, infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    #ifdef GAME_INTERNAL
    WatchedProgram watched;
    watched.vertexShader = vertexShader;
    watched.fragmentShader = fragmentShader;
    watched.shaderProgram = shaderProgram;
    strcpy(watched.vertexFilepath, vertexShaderPath);
    strcpy(watched.fragmentFilepath, fragmentShaderPath);
    watched.vertexTime = std::filesystem::last_write_time(vertexShaderPath);
    watched.fragmentTime = std::filesystem::last_write_time(fragmentShaderPath);

    watchedPrograms[watchedProgramsCount] = watched;
    watchedProgramsCount++;
    #endif

    return shaderProgram;
}

static void GL_WatchChanges()
{
    #ifdef GAME_INTERNAL
    for(i32 i = 0; i < watchedProgramsCount; ++i) {
        WatchedProgram watched = watchedPrograms[i];

        std::filesystem::file_time_type vertexTime = std::filesystem::last_write_time(watched.vertexFilepath);
        std::filesystem::file_time_type fragmentTime = std::filesystem::last_write_time(watched.fragmentFilepath);

        if(vertexTime != watched.vertexTime || fragmentTime != watched.fragmentTime) {
            size_t vertexSouceSize = 0;
            void* data = LoadFileToMemory(watched.vertexFilepath, FILE_MODE_READ_BINARY, &vertexSouceSize);
            SOURCE_TYPE vertexSource = static_cast<SOURCE_TYPE>(data);

            size_t fragmentSouceSize = 0;
            data = LoadFileToMemory(watched.fragmentFilepath, FILE_MODE_READ_BINARY, &fragmentSouceSize);
            SOURCE_TYPE fragmentSource = static_cast<SOURCE_TYPE>(data);

            if(vertexSource[0] != '\0' && fragmentSource[0] != '\0') {
                Log(&editorConsole, "Started to reload program %d, vertex %s, fragment %s", watched.shaderProgram, watched.vertexFilepath, watched.fragmentFilepath);
                glDetachShader(watched.shaderProgram, watched.vertexShader);
                glDetachShader(watched.shaderProgram, watched.fragmentShader);
                
                watched.vertexShader = glCreateShader(GL_VERTEX_SHADER);
                
                i32 size = (i32)vertexSouceSize;
                glShaderSource(watched.vertexShader, 1, &vertexSource, &size);
                glCompileShader(watched.vertexShader);

                i32 success;
                char infoLog[512];
                glGetShaderiv(watched.vertexShader, GL_COMPILE_STATUS, &success);

                if (!success)
                {
                    glGetShaderInfoLog(watched.vertexShader, 512, NULL, infoLog);
                    LogError(&editorConsole, "ERROR::VERTEX::COMPILATION_FAILED %s\n", watched.vertexFilepath);
                    LogError(&editorConsole, infoLog);
                }

                watched.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

                size = (i32)fragmentSouceSize;
                glShaderSource(watched.fragmentShader, 1, &fragmentSource, &size);
                glCompileShader(watched.fragmentShader);

                if (!success)
                {
                    glGetShaderInfoLog(watched.vertexShader, 512, NULL, infoLog);
                    LogError(&editorConsole, "ERROR::FRAGMENT::COMPILATION_FAILED %s\n", watched.fragmentFilepath);
                    LogError(&editorConsole, infoLog);
                }

                glAttachShader(watched.shaderProgram, watched.vertexShader);
                glAttachShader(watched.shaderProgram, watched.fragmentShader);
                glLinkProgram(watched.shaderProgram);

                glGetProgramiv(watched.shaderProgram, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(watched.shaderProgram, 512, NULL, infoLog);
                    LogError(&editorConsole, "ERROR::PROGRAM::COMPILATION_FAILED\n");
                    LogError(&editorConsole, infoLog);
                }

                glDeleteShader(watched.vertexShader);
                glDeleteShader(watched.fragmentShader);

                watched.vertexTime = vertexTime;
                watched.fragmentTime = fragmentTime;
                watchedPrograms[i] = watched;
            }
        }
    }
    #endif
}

static void CalculateCharacterOffset(FontAtlas *font, char singleChar, f32 *posX, f32 *posY)
{
    if(singleChar < SPECIAL_ASCII_CHAR_OFFSET) {
        switch(singleChar) {
            case '\n': {
                *posX = 0;
                *posY = *posY + font->fontSize;
                break;
            }
        }
    }
}

// static void DrawStyledText(FontAtlas *font, const char* string, TextStyles* styleList, f32 x, f32 y, v4 color)
// {
//     u32 ActiveStyles = 0;
//     v2 Offset = V2(0, 0);
//     for(i32 index = 0; string[index] != 0; ++index) {
//         char singleChar = string[index];

//         if(singleChar == '%') {
//             ++index;
//             u32 Multiplier = 1;
//             u32 ToActivateIndex = 0;
//             while(String[index] != '%') {
//                 ToActivateIndex = ToActivateIndex * Multiplier + (string[index] - 48); // NOTE(Juan): 48 == 0 in ascii table
//                 Multiplier *= 10;
//                 ++index;
//             }
//             if((ActiveStyles >> ToActivateIndex) & 1) {
//                 ActiveStyles &= ~(1 << ToActivateIndex);
//             }
//             else {
//                 ActiveStyles |= (1 << ToActivateIndex);
//             }
//         }
//         else {
//             stbtt_bakedchar *CharData = CalculateCharacterOffset(font, singleChar, &Offset, Font->FontSize);

//             v2 StyleOffset = V2(0, 0);
//             v4 Color = V4(1, 1, 1, 1);

//             for(uint32 StyleIndex = 0; StyleIndex < 32; ++StyleIndex) {
//                 if((ActiveStyles >> StyleIndex) & 1) {
//                     text_style *CurrentStyle = StyleList + StyleIndex;
//                     if(CurrentStyle->Type == SINE_X) {
//                         StyleOffset.X += Sin(Input->Time.TotalTime * CurrentStyle->TimeScale + CurrentStyle->Offset + index * CurrentStyle->CharOffset) * CurrentStyle->Value;
//                     }
//                     if(CurrentStyle->Type == SINE_Y) {
//                         StyleOffset.Y += Sin(Input->Time.TotalTime * CurrentStyle->TimeScale + CurrentStyle->Offset + index * CurrentStyle->CharOffset) * CurrentStyle->Value;
//                     }
//                     if(CurrentStyle->Type == FADE_IN) {
//                         Color.A = (Input->Time.TotalTime * CurrentStyle->TimeScale) - (index * CurrentStyle->CharOffset + CurrentStyle->Offset);
//                         if(Color.A > 1) {
//                             Color.A = 1;
//                         } else if(Color.A < 0) {
//                             Color.A = 0;
//                         }
//                     }
//                     if(CurrentStyle->Type == FADE_OUT) {
//                         Color.A = (index * CurrentStyle->CharOffset + CurrentStyle->Offset) - (Input->Time.TotalTime * CurrentStyle->TimeScale);
//                         if(Color.A > 1) {
//                             Color.A = 1;
//                         } else if(Color.A < 0) {
//                             Color.A = 0;
//                         }
//                     }
//                     if(CurrentStyle->Type == TYPEWRITER) {
//                         if(Input->Time.TotalTime * CurrentStyle->TimeScale > index * CurrentStyle->CharOffset + CurrentStyle->Offset) {
//                             Color.A = 1;
//                         } else {
//                             Color.A = 0;
//                         }
//                     }
//                 }
//             }

//             if(CharData != 0) {
//                 DrawChar(Buffer, &Font->Bitmap, CharData, Char, RealX + StyleOffset.X + Offset.X, RealY + StyleOffset.Y + Offset.Y, Color);
//                 Offset.X += CharData->xadvance;
//             }
//         }
//     }
// }

static void UseProgram(u32 programID)
{
    if(renderState.overrideProgram) { programID = renderState.overrideProgram; }

    if(programID != renderState.currentProgram) {
        editorRenderDebugger.programChanges++;

        glUseProgram(programID);

        colorLocation = glGetUniformLocation(programID, "color");
        modelLocation = glGetUniformLocation(programID, "model");
        viewLocation = glGetUniformLocation(programID, "view");
        projectionLocation = glGetUniformLocation(programID, "projection");

        timeLocation = glGetUniformLocation(programID, "time");
        glUniform1f(timeLocation, gameState->time.gameTime);

        renderState.currentProgram = programID;
    }
}

static void SetupTextureParameters(u32 textureTarget)
{
    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, renderState.wrapS);
    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, renderState.wrapT);
    glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, renderState.minFilter);
    glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, renderState.magFilter);
}

static void SetupModelUniforms(u32 programID, v4 color, m44 model, m44 view, m44 projection)
{
    if(renderState.overrideProgram) { programID = renderState.overrideProgram; }

    glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

    glUniformMatrix4fv(modelLocation, 1, false, model.e);    
    glUniformMatrix4fv(viewLocation, 1, false, view.e);
    glUniformMatrix4fv(projectionLocation, 1, false, projection.e);
}

static void BindBuffer()
{
    if(renderState.overridingVertices || renderState.overridingIndices) {
        glBindVertexArray(overrideBuffer.vertexArray);
    }
    else {
        glBindVertexArray(quadBuffer.vertexArray);
    }

    if(renderState.overridingVertices) {
        glBindBuffer(GL_ARRAY_BUFFER, overrideBuffer.vertexBuffer);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, quadBuffer.vertexBuffer);
    }

    if(renderState.overridingIndices) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, overrideBuffer.indexBuffer);
    }
    else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadBuffer.indexBuffer);
    }
}

static void GL_Render()
{
    RenderHeader *renderHeader = (RenderHeader *)renderTemporaryMemory.arena->base;

    m44 view = gameState->camera.view;
    view._23 = 1.0f;
    m44 projection = gameState->camera.projection;

    while(renderHeader->id > 0 && (void*)renderHeader < (void*)(renderTemporaryMemory.arena->base + renderTemporaryMemory.used)) {
        m44 model = IdM44();

        i32 size = 0;
        // #TODO (Juan): More render types can be added line, spline, etc
        switch(renderHeader->type) {
            case type_RenderClear: {
                RenderClear *clear = (RenderClear *)renderHeader;
                glClearColor(clear->color.r, clear->color.g, clear->color.b, clear->color.a);
                glClear(GL_COLOR_BUFFER_BIT);

                size = sizeof(RenderClear);
                break;
            }
            case type_RenderColor: {
                RenderColor *color = (RenderColor *)renderHeader;
                renderState.renderColor = color->color;

                size = sizeof(RenderColor);
                break;
            }
            case type_RenderLineWidth: {
                RenderLineWidth *line = (RenderLineWidth *)renderHeader;
                glLineWidth(line->width);

                size = sizeof(RenderLineWidth);
                break;
            }
            case type_RenderTransparent: {
                RenderTransparent *transparent = (RenderTransparent *)renderHeader;
                if(transparent->enabled) {
                    glEnable(GL_BLEND);
                    glDisable(GL_DEPTH_TEST);
                    glBlendEquationSeparate(transparent->modeRGB, transparent->modeAlpha);
                    glBlendFuncSeparate(transparent->srcRGB, transparent->dstRGB, transparent->srcAlpha, transparent->dstAlpha);
                }
                else {
                    glDisable(GL_BLEND);
                    glEnable(GL_DEPTH_TEST);
                }

                size = sizeof(RenderTransparent);
                break;
            }
            case type_RenderLine: {
                RenderLine *line = (RenderLine *)renderHeader;
                
                UseProgram(coloredProgram);

                SetupModelUniforms(coloredProgram, renderState.renderColor, model, view, projection);

                f32 lineVertices[] = {
                    line->start.x, line->start.y, 0.0f,
                    line->end.x, line->end.y, 0.0f
                };

                u32 indices[] = {
                    0, 1
                };

                glBindVertexArray(customBuffer.vertexArray);
                glBindBuffer(GL_ARRAY_BUFFER, customBuffer.vertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, customBuffer.indexBuffer);

                glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glDrawArrays(GL_LINES, 0, 2);

                size = sizeof(RenderLine);
                break;
            }
            case type_RenderTriangle: {
                RenderTriangle *triangle = (RenderTriangle *)renderHeader;
                
                UseProgram(coloredProgram);

                SetupModelUniforms(coloredProgram, renderState.renderColor, model, view, projection);

                f32 triangleVertices[] = {
                    triangle->point1.x, triangle->point1.y, 0.0f,
                    triangle->point2.x, triangle->point2.y, 0.0f,
                    triangle->point3.x, triangle->point3.y, 0.0f
                };

                u32 indices[] = {
                    0, 1, 2
                };

                glBindVertexArray(customBuffer.vertexArray);
                glBindBuffer(GL_ARRAY_BUFFER, customBuffer.vertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, customBuffer.indexBuffer);

                glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glDrawArrays(GL_TRIANGLES, 0, 3);

                size = sizeof(RenderTriangle);
                break;
            }
            case type_RenderRectangle: {
                RenderRectangle *rectangle = (RenderRectangle *)renderHeader;

                UseProgram(coloredProgram);

                model *= ScaleM44(rectangle->scale);
                model *= TranslationM44(rectangle->position);
                SetupModelUniforms(coloredProgram, renderState.renderColor, model, view, projection);

                glBindVertexArray(quadBuffer.vertexArray);
                glBindBuffer(GL_ARRAY_BUFFER, quadBuffer.vertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadBuffer.indexBuffer);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                size = sizeof(RenderRectangle);
                break;
            }
            case type_RenderCircle: {
                RenderCircle *circle = (RenderCircle *)renderHeader;

                UseProgram(coloredProgram);

                SetupModelUniforms(coloredProgram, renderState.renderColor, model, view, projection);

                f32 vertices[300] = {
                    circle->position.x, circle->position.y, 0.0f,
                };

                u32 indices[300] = {

                };

                for(i32 i = 0; i < circle->segments; ++i) {
                    float angle = ((float)i / (float)circle->segments) * PI32 * 2;
                    vertices[i * 3 + 3] = circle->position.x + Sin(angle) * circle->radius;
                    vertices[i * 3 + 4] = circle->position.y + Cos(angle) * circle->radius;
                    vertices[i * 3 + 5] = 0.0f;

                    indices[i * 3 + 0] = 0;
                    indices[i * 3 + 1] = i;
                    indices[i * 3 + 2] = i + 1;
                }

                i32 lastSegment = circle->segments - 1;
                indices[circle->segments * 3 + 0] = 0;
                indices[circle->segments * 3 + 1] = circle->segments;
                indices[circle->segments * 3 + 2] = 1;

                glBindVertexArray(customBuffer.vertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, customBuffer.vertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, customBuffer.indexBuffer);

                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glDrawElements(GL_TRIANGLES, (circle->segments + 1) * 3, GL_UNSIGNED_INT, 0);

                size = sizeof(RenderRectangle);
                break;
            }
            case type_RenderTextureParameters: {
                RenderTextureParameters *textureParameters = (RenderTextureParameters *)renderHeader;
                
                renderState.wrapS = textureParameters->wrapS;
                renderState.wrapT = textureParameters->wrapT;
                renderState.minFilter = textureParameters->minFilter;
                renderState.magFilter = textureParameters->magFilter;

                size = sizeof(RenderTextureParameters);
                break;
            }
            case type_RenderTexture: {
                RenderTexture *texture = (RenderTexture *)renderHeader;

                UseProgram(texturedProgram);

                model *= ScaleM44(texture->scale);
                model *= TranslationM44(texture->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                glBindTexture(GL_TEXTURE_2D, texture->textureID);
                SetupTextureParameters(GL_TEXTURE_2D);

                BindBuffer();

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderTexture);
                break;
            }
            case type_RenderImage: {
                RenderImage *image = (RenderImage *)renderHeader;

                UseProgram(texturedProgram);

                GLTexture texture = GL_LoadTexture(image->filepath);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                SetupTextureParameters(GL_TEXTURE_2D);

                image->scale.x *= texture.width;
                image->scale.y *= texture.height;

                if((image->header.renderFlags & IMAGE_ADAPTATIVE_FIT) > 0) {
                    // #TODO(Juan): Check this and fix errors
                    if(texture.height > texture.width) {
                        f32 oldScaleX = image->scale.x;
                        image->scale.x *= (f32)texture.width / (f32)texture.height;
                        image->position.x += (oldScaleX - image->scale.x) * 0.5f;
                    } else {
                        f32 oldScaleY = image->scale.y;
                        image->scale.y *= (f32)texture.width / (f32)texture.height;
                        image->position.y += (oldScaleY - image->scale.y) * 0.5f;
                    }
                } else if((image->header.renderFlags & IMAGE_KEEP_RATIO_X) > 0) {
                    f32 quadRatio = (f32)image->scale.y / (f32)image->scale.x;
                    f32 textureRatio = (f32)texture.height / (f32)texture.width;
                    f32 oldScaleY = image->scale.y;
                    image->scale.y *= textureRatio / quadRatio;
                    image->position.y += (oldScaleY - image->scale.y) * 0.5f;
                } else if((image->header.renderFlags & IMAGE_KEEP_RATIO_Y) > 0) {
                    f32 quadRatio = (f32)image->scale.x / (f32)image->scale.y;
                    f32 textureRatio = (f32)texture.width / (f32)texture.height;
                    f32 oldScaleX = image->scale.x;
                    image->scale.x *= textureRatio / quadRatio;
                    image->position.x += (oldScaleX - image->scale.x) * 0.5f;
                }

                model *= ScaleM44(image->scale);
                model *= TranslationM44(image->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                BindBuffer();

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderImage) + image->filepathSize;
                break;
            }
            case type_RenderImageUV: {
                RenderImageUV *imageUV = (RenderImageUV *)renderHeader;

                UseProgram(texturedProgram);

                GLTexture texture = GL_LoadTexture(imageUV->filepath);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                SetupTextureParameters(GL_TEXTURE_2D);

                imageUV->scale.x *= texture.width;
                imageUV->scale.y *= texture.height;

                model *= ScaleM44(imageUV->scale);
                model *= TranslationM44(imageUV->position);
                SetupModelUniforms(coloredProgram, renderState.renderColor, model, view, projection);

                BindBuffer();

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderImageUV) + imageUV->filepathSize;
                break;
            }
            case type_RenderAtlasSprite: {
                RenderAtlasSprite *atlas = (RenderAtlasSprite *)renderHeader;

                UseProgram(texturedProgram);

                TextureAtlas textureAtlas = GL_LoadAtlas(atlas->atlasName);
                rectangle2 uvRect = shget(textureAtlas.sprites, atlas->spriteKey);

                GLTexture texture = GL_LoadTexture(atlas->filepath);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 rectSize = GetSize(uvRect);
                atlas->scale.x *= rectSize.x;
                atlas->scale.y *= rectSize.y;

                model *= ScaleM44(atlas->scale);
                model *= TranslationM44(atlas->position);
                SetupModelUniforms(coloredProgram, renderState.renderColor, model, view, projection);

                CreateQuadPosUV(0, 0, 1, 1, 
                    uvRect.min.x / texture.width, uvRect.min.y / texture.height, uvRect.max.x / texture.width, uvRect.max.y / texture.height);

                glBindVertexArray(customBuffer.vertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, customBuffer.vertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, customBuffer.indexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderAtlasSprite) + atlas->filepathSize + atlas->atlasNameSize + atlas->spriteKeySize;
                break;
            }
            case type_RenderFont: {
                RenderFont *font = (RenderFont *)renderHeader;
                currentFont = GL_LoadFont(font->filepath, font->fontSize, font->width, font->height);

                size = sizeof(RenderFont) + font->filepathSize;
                break;
            }
            case type_RenderChar: {
                RenderChar *renderChar = (RenderChar *)renderHeader;

                UseProgram(fontProgram);

                GLTexture texture = GL_LoadTexture(currentFont.fontFilepath);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                SetupTextureParameters(GL_TEXTURE_2D);

                model *= ScaleM44(renderChar->scale);
                model *= TranslationM44(renderChar->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                f32 posX = 0;
                f32 posY = currentFont.fontSize;
                stbtt_aligned_quad quad;
                stbtt_GetBakedQuad(currentFont.charData, currentFont.width, currentFont.height, renderChar->singleChar - SPECIAL_ASCII_CHAR_OFFSET, &posX, &posY, &quad, 1);
                CreateQuadPosUV(quad.x0, quad.y0, quad.x1, quad.y1, quad.s0, quad.t0, quad.s1, quad.t1);

                glBindVertexArray(customBuffer.vertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, customBuffer.vertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, customBuffer.indexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderChar);
                break;
            }
            case type_RenderText: {
                RenderText *text = (RenderText *)renderHeader;

                UseProgram(fontProgram);

                model *= ScaleM44(text->scale);
                model *= TranslationM44(text->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                GLTexture texture = GL_LoadTexture(currentFont.fontFilepath);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 textSize = text->scale;
                v2 textPosition = text->position;
                v2 fontDividers = V2(currentFont.fontSize / currentFont.width, currentFont.fontSize / currentFont.height);

                f32 posX = 0;
                f32 posY = currentFont.fontSize;
                stbtt_aligned_quad quad;

                for(i32 i = 0; i < text->stringSize - 1; ++i) {
                    char currentChar = text->string[i];

                    CalculateCharacterOffset(&currentFont, currentChar, &posX, &posY);
                    stbtt_GetBakedQuad(currentFont.charData, currentFont.width, currentFont.height, currentChar - SPECIAL_ASCII_CHAR_OFFSET, &posX, &posY, &quad, 1);
                    CreateQuadPosUV(quad.x0, quad.y0, quad.x1, quad.y1, quad.s0, quad.t0, quad.s1, quad.t1);

                    glBindVertexArray(customBuffer.vertexArray);

                    glBindBuffer(GL_ARRAY_BUFFER, customBuffer.vertexBuffer);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, customBuffer.indexBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW); 

                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                    glEnableVertexAttribArray(1);

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
                
                size = sizeof(RenderText) + text->stringSize;
                break;
            }
            case type_RenderSetUniform: {
                RenderSetUniform *uniform = (RenderSetUniform *)renderHeader;

                size = sizeof(RenderSetUniform);
                if(uniform->type == UniformType_Float) {
                    glUniform1f(uniform->location, *((f32*)(uniform + 1)));
                }
                else if(uniform->type == UniformType_Vector2) {
                    v2 vector = *((v2*)(uniform + 1));
                    glUniform2f(uniform->location, vector.x, vector.y);
                }
                
                size += uniform->parametersSize;
                break;
            }
            case type_RenderOverrideProgram: {
                RenderOverrideProgram *program = (RenderOverrideProgram *)renderHeader;
 
                renderState.overrideProgram = program->programID;

                size = sizeof(RenderOverrideProgram);
                break;
            }
            case type_RenderOverrideVertices: {
                RenderOverrideVertices *vertices = (RenderOverrideVertices *)renderHeader;
 
                renderState.overridingVertices = vertices->vertices != 0;
                if(renderState.overridingVertices) {
                    glBindVertexArray(overrideBuffer.vertexArray);
                    glBindBuffer(GL_ARRAY_BUFFER, overrideBuffer.vertexBuffer);
                    glBufferData(GL_ARRAY_BUFFER, vertices->size, vertices->vertices, GL_STATIC_DRAW);
                }

                size = sizeof(RenderOverrideVertices) + vertices->size;
                break;
            }
            case type_RenderOverrideIndices: {
                RenderOverrideIndices *indices = (RenderOverrideIndices *)renderHeader;

                renderState.overridingIndices = indices->indices != 0;
                if(renderState.overridingIndices) {
                    glBindVertexArray(overrideBuffer.vertexArray);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, overrideBuffer.indexBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size, indices->indices, GL_STATIC_DRAW);
                }

                size = sizeof(RenderOverrideIndices) + indices->size;
                break;
            }
            default: {
                InvalidCodePath;
                break;
            }
        }

        Assert(size > 0);
        editorRenderDebugger.drawCount++;
        renderHeader = (RenderHeader *)((u8 *)renderHeader + size);
    }
}

static void GL_End()
{
    glDeleteVertexArrays(1, &quadBuffer.vertexArray);
    glDeleteBuffers(1, &quadBuffer.vertexBuffer);
    glDeleteBuffers(1, &quadBuffer.indexBuffer);
    
    glDeleteVertexArrays(1, &customBuffer.vertexArray);
    glDeleteBuffers(1, &customBuffer.vertexBuffer);
    glDeleteBuffers(1, &customBuffer.indexBuffer);
}

#endif