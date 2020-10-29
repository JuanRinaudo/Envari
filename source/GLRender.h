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
static u32 textured9SliceProgram;

static u32 colorLocation;
static u32 mvpLocation;

static u32 textureSizeLocation;
static u32 dimensionsLocation;
static u32 borderLocation;

static u32 timeLocation;

static GLTextureCache* textureCache = NULL;

static GLTextureAtlasReference* atlasCache = NULL;

static FontAtlas currentFont;

static GLFontReference* fontCache = NULL;

enum TextStyle {
    TextStyle_Normal = 0,
    TextStyle_SineX = 1,
    TextStyle_SineY = 2,
    TextStyle_FadeIn = 3,
    TextStyle_FadeOut = 4,
    TextStyle_Typewriter = 5
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

#ifdef GAME_EDITOR
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
    GLTexture texture;
    if(index > -1) {
        texture = shget(textureCache, textureKey);
    } else {
        i32 width, height, channels;
        unsigned char *data = stbi_load(textureKey, &width, &height, &channels, 0);

        u32 textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        texture.textureID = textureID;
        texture.width = width;
        texture.height = height;
        texture.channels = channels;
        shput(textureCache, PushString(&sceneState->arena, textureKey), texture);

        stbi_image_free(data);
    }
    
    glBindTexture(GL_TEXTURE_2D, texture.textureID);
    glUniform2f(textureSizeLocation, (f32)texture.width, (f32)texture.height);
    
    return texture;
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
                keyPointer = PushString(&sceneState->arena, tokenizer.tokenBuffer, tokenizer.tokenBufferIndex);

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

static i32 GL_LoadFont(i32 fontID, FontAtlas *atlas)
{
    i32 index = (i32)hmgeti(fontCache, fontID);
    if(index > -1) {
        *atlas = fontCache[index].value;
        return 1;
    } else {
        return 0;
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
        Log("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_UNSUPPORTED) {
        Log("ERROR::FRAMEBUFFER:: Framebuffer is not supported!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"

i32 GL_GenerateFont(const char *filepath, f32 fontSize, u32 width, u32 height)
{
    FontAtlas result;
    result.fontFilepath = PushString(&sceneState->arena, filepath, &result.fontFilepathSize);
    result.fontSize = fontSize;
    result.lineHeight = fontSize;
    result.tabSize = (i32)fontSize * 2;
    result.width = width;
    result.height = height;

    u32 data_size = 0;
    void* data = LoadFileToMemory(filepath, FILE_MODE_READ_BINARY, &data_size);

    u8* tempBitmap = PushArray(&temporalState->arena, width * height, u8);
    stbtt_BakeFontBitmap((u8 *)data, 0, fontSize, tempBitmap, width, height, SPECIAL_ASCII_CHAR_OFFSET, FONT_CHAR_SIZE, result.charData); // no guarantee this fits!

    // stbi_write_png("bakedFont.png", width, height, 1, tempBitmap, width);

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
    hmput(fontCache, textureID, result);

    return textureID;
}

i32 GL_CompileProgram(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    // NOTE(Juan): Shaders
    u32 vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    u32 data_size = 0;
    void* data = LoadFileToMemory(vertexShaderPath, FILE_MODE_READ_BINARY, &data_size);
    if(data == 0) {
        LogError("ERROR::VERTEX::FileLoad failed %s", vertexShaderPath);
        return 0;
    }
    SOURCE_TYPE vertexSource = static_cast<SOURCE_TYPE>(data);
    
    i32 size = (i32)data_size;
    glShaderSource(vertexShader, 1, &vertexSource, &size);
    glCompileShader(vertexShader);

    UnloadFileFromMemory(data);

    i32 success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LogError("ERROR::VERTEX::COMPILATION_FAILED %s", vertexShaderPath);
        LogError(infoLog);
    }

    u32 fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    data = LoadFileToMemory(fragmentShaderPath, FILE_MODE_READ_BINARY, &data_size);
    if(data == 0) {
        LogError("ERROR::FRAGMENT::FileLoad failed %s", fragmentShaderPath);
        return 0;
    }
    SOURCE_TYPE fragmentSource = static_cast<SOURCE_TYPE>(data);

    size = (i32)data_size;
    glShaderSource(fragmentShader, 1, &fragmentSource, &size);
    glCompileShader(fragmentShader);

    UnloadFileFromMemory(data);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LogError("ERROR::FRAGMENT::COMPILATION_FAILED %s", fragmentShaderPath);
        LogError(infoLog);
    }

    i32 shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        LogError("ERROR::PROGRAM::LINK_FAILED");
        LogError(infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    #ifdef GAME_EDITOR
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

i32 GL_CompileProgramPlatform(const char *vertexShaderPlatform, const char *fragmentShaderPlatform)
{
    char* vertexShaderPath = PushString(&temporalState->arena, SHADER_PREFIX, sizeof(SHADER_PREFIX) - 1);
    PushString(&temporalState->arena, vertexShaderPlatform);
    
    char* fragmentShaderPath = PushString(&temporalState->arena, SHADER_PREFIX, sizeof(SHADER_PREFIX) - 1);
    PushString(&temporalState->arena, fragmentShaderPlatform);
    
    return GL_CompileProgram(vertexShaderPath, fragmentShaderPath);
}

static void GL_WatchChanges()
{
    #ifdef GAME_EDITOR
    for(i32 i = 0; i < watchedProgramsCount; ++i) {
        WatchedProgram watched = watchedPrograms[i];

        std::filesystem::file_time_type vertexTime = std::filesystem::last_write_time(watched.vertexFilepath);
        std::filesystem::file_time_type fragmentTime = std::filesystem::last_write_time(watched.fragmentFilepath);

        if(vertexTime != watched.vertexTime || fragmentTime != watched.fragmentTime) {
            Log("Started to reload program %d, vertex %s, fragment %s", watched.shaderProgram, watched.vertexFilepath, watched.fragmentFilepath);
            glDetachShader(watched.shaderProgram, watched.vertexShader);
            glDetachShader(watched.shaderProgram, watched.fragmentShader);
            
            watched.vertexShader = glCreateShader(GL_VERTEX_SHADER);
            
            size_t vertexSouceSize = 0;
            void* data = LoadFileToMemory(watched.vertexFilepath, FILE_MODE_READ_BINARY, &vertexSouceSize);
            SOURCE_TYPE vertexSource = static_cast<SOURCE_TYPE>(data);

            i32 size = (i32)vertexSouceSize;
            glShaderSource(watched.vertexShader, 1, &vertexSource, &size);
            glCompileShader(watched.vertexShader);

            UnloadFileFromMemory(data);

            i32 success;
            char infoLog[512];
            glGetShaderiv(watched.vertexShader, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                glGetShaderInfoLog(watched.vertexShader, 512, NULL, infoLog);
                LogError("ERROR::VERTEX::COMPILATION_FAILED %s\n", watched.vertexFilepath);
                LogError(infoLog);
            }

            watched.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

            size_t fragmentSouceSize = 0;
            data = LoadFileToMemory(watched.fragmentFilepath, FILE_MODE_READ_BINARY, &fragmentSouceSize);
            SOURCE_TYPE fragmentSource = static_cast<SOURCE_TYPE>(data);

            size = (i32)fragmentSouceSize;
            glShaderSource(watched.fragmentShader, 1, &fragmentSource, &size);
            glCompileShader(watched.fragmentShader);

            UnloadFileFromMemory(data);

            if (!success)
            {
                glGetShaderInfoLog(watched.vertexShader, 512, NULL, infoLog);
                LogError("ERROR::FRAGMENT::COMPILATION_FAILED %s\n", watched.fragmentFilepath);
                LogError(infoLog);
            }

            glAttachShader(watched.shaderProgram, watched.vertexShader);
            glAttachShader(watched.shaderProgram, watched.fragmentShader);
            glLinkProgram(watched.shaderProgram);

            glGetProgramiv(watched.shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(watched.shaderProgram, 512, NULL, infoLog);
                LogError("ERROR::PROGRAM::COMPILATION_FAILED\n");
                LogError(infoLog);
            }

            glDeleteShader(watched.vertexShader);
            glDeleteShader(watched.fragmentShader);

            watched.vertexTime = vertexTime;
            watched.fragmentTime = fragmentTime;
            watchedPrograms[i] = watched;
        }
    }
    #endif
}

static void GetBakedQuad(FontAtlas *font, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q)
{
   f32 ipw = 1.0f / font->width;
   f32 iph = 1.0f / font->height;
   const stbtt_bakedchar *b = font->charData + char_index;
   f32 round_x = Floor((*xpos + b->xoff) + 0.5f);
   f32 round_y = Floor((*ypos + b->yoff) + 0.5f);

   q->x0 = round_x;
   q->y0 = round_y;
   q->x1 = round_x + b->x1 - b->x0;
   q->y1 = round_y + b->y1 - b->y0;

   q->s0 = b->x0 * ipw;
   q->t0 = b->y0 * iph;
   q->s1 = b->x1 * ipw;
   q->t1 = b->y1 * iph;

   *xpos += b->xadvance;
}

static void CalculateCharacterOffset(FontAtlas *font, char singleChar, f32 *posX, f32 *posY, u32 *lineCharacterCount)
{
    switch(singleChar) {
        case '\n': {
            *lineCharacterCount = 0;
            *posX = 0;
            *posY = *posY + font->lineHeight;
            break;
        }
        case '\t': {
            *posX = (f32)(CeilToInt(*posX / font->tabSize) * font->tabSize);
            break;
        }
        default: {
            (*lineCharacterCount)++;
            break;
        }
    }
}

static v2 CalculateTextSize(FontAtlas *font, const char* string, i32 stringSize, f32 containerWidth = -1)
{
    f32 lineWidth = 0;
    f32 lineHeight = 0;
    f32 width = 0;
    f32 height = 0;
    u32 lineCharacterCount = 0;
    for(i32 i = 0; i < stringSize; ++i) {
        char singleChar = string[i];
        
        stbtt_bakedchar charData = font->charData[singleChar - SPECIAL_ASCII_CHAR_OFFSET];
        lineHeight = MAX(lineHeight, charData.y1 - charData.y0);
        if(singleChar > SPECIAL_ASCII_CHAR_OFFSET) {
            lineWidth += charData.xadvance;
        }
        else {
            CalculateCharacterOffset(font, singleChar, &lineWidth, &height, &lineCharacterCount);
        }

        if(containerWidth > 0 && lineWidth > containerWidth) {
            lineCharacterCount = 0;
            width = MAX(width, lineWidth - charData.xadvance);
            lineWidth = charData.xadvance;
            height = height + font->lineHeight;
        }
    }

    width = MAX(width, lineWidth);
    height = MAX(height, lineHeight);

    return V2(width, height);
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
    #if GAME_EDITOR
        if(editorRenderDebugger.wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            programID = coloredProgram;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    #endif

    if(renderState.overrideProgram) { programID = renderState.overrideProgram; }

    if(programID != renderState.currentProgram) {
        #ifdef GAME_EDITOR
        editorRenderDebugger.programChanges++;
        #endif

        glUseProgram(programID);

        colorLocation = glGetUniformLocation(programID, "color");
        mvpLocation = glGetUniformLocation(programID, "mvp");

        textureSizeLocation = glGetUniformLocation(programID, "textureSize");

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

    m44 mvp = (projection * view) * model;

    glUniformMatrix4fv(mvpLocation, 1, false, mvp.e);
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

static void GL_Init()
{
    coloredProgram = GL_CompileProgramPlatform(COLORED_VERT, COLORED_FRAG);
    fontProgram = GL_CompileProgramPlatform(TEXTURED_VERT, FONT_FRAG);
    texturedProgram = GL_CompileProgramPlatform(TEXTURED_VERT, TEXTURED_FRAG);
    textured9SliceProgram = GL_CompileProgramPlatform(TEXTURED_VERT, TEXTURED9SLICE_FRAG);
    
    dimensionsLocation = glGetUniformLocation(textured9SliceProgram, "dimensions");
    borderLocation = glGetUniformLocation(textured9SliceProgram, "border");

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

static void GL_Render()
{
    RenderHeader *renderHeader = (RenderHeader *)renderTemporaryMemory.arena->base;

    // #NOTE (Juan): Check if there are layers setted and sort commands by layer
    if(renderState.usedLayers > 0) {
        void *sortedRenderStart = renderTemporaryMemory.arena->base + renderTemporaryMemory.arena->used;
        
        for(u32 layerIndex = 31; layerIndex < 32; --layerIndex) {
            if((renderState.usedLayers & 1 << layerIndex) > 0) {
                RenderHeader *searchHeader = (RenderHeader *)renderTemporaryMemory.arena->base;
                u32 searchLayer = 0;
                while(searchHeader->id > 0 && (void*)searchHeader < sortedRenderStart) {
                    if(searchHeader->type == RenderType_RenderLayer) {
                        searchLayer = ((RenderLayer *)searchHeader)->layer;
                    }
                    else if(searchLayer == layerIndex) {
                        void* copyDestination = PushSize(&renderTemporaryMemory, searchHeader->size);
                        memcpy(copyDestination, (void *)searchHeader, searchHeader->size);
                    }
                    searchHeader = (RenderHeader *)((u8 *)searchHeader + searchHeader->size);
                }
            }
        }

        renderHeader = (RenderHeader *)sortedRenderStart;
    }

    m44 view = gameState->camera.view;
    view._30 = -gameState->camera.size * 0.5f;
    view._31 = view._30;
    view._23 = 1.0f;
    m44 projection = gameState->camera.projection;

    while(renderHeader->id > 0 && (void*)renderHeader < (void*)(renderTemporaryMemory.arena->base + renderTemporaryMemory.used)) {
        m44 model = IdM44();

        switch(renderHeader->type) {
            case RenderType_RenderTempData: {
                break;
            }
            case RenderType_RenderClear: {
                RenderClear *clear = (RenderClear *)renderHeader;
                glClearColor(clear->color.r, clear->color.g, clear->color.b, clear->color.a);
                glClear(GL_COLOR_BUFFER_BIT);
                break;
            }
            case RenderType_RenderColor: {
                RenderColor *color = (RenderColor *)renderHeader;
                renderState.renderColor = color->color;
                break;
            }
            case RenderType_RenderTransparent: {
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
                break;
            }
            case RenderType_RenderLineWidth: {
                RenderLineWidth *line = (RenderLineWidth *)renderHeader;
                glLineWidth(line->width);
                break;
            }
            case RenderType_RenderLine: {
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
                break;
            }
            case RenderType_RenderTriangle: {
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
                break;
            }
            case RenderType_RenderRectangle: {
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
                break;
            }
            case RenderType_RenderCircle: {
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
                break;
            }
            case RenderType_RenderTextureParameters: {
                RenderTextureParameters *textureParameters = (RenderTextureParameters *)renderHeader;
                
                renderState.wrapS = textureParameters->wrapS;
                renderState.wrapT = textureParameters->wrapT;
                renderState.minFilter = textureParameters->minFilter;
                renderState.magFilter = textureParameters->magFilter;
                break;
            }
            case RenderType_RenderTexture: {
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
                break;
            }
            case RenderType_RenderImage: {
                RenderImage *image = (RenderImage *)renderHeader;

                UseProgram(texturedProgram);

                GLTexture texture = GL_LoadTexture(image->filepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                image->scale.x *= texture.width;
                image->scale.y *= texture.height;

                if((image->header.renderFlags & ImageRenderFlag_Fit) > 0) {
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
                } else if((image->header.renderFlags & ImageRenderFlag_KeepRatioX) > 0) {
                    f32 quadRatio = (f32)image->scale.y / (f32)image->scale.x;
                    f32 textureRatio = (f32)texture.height / (f32)texture.width;
                    f32 oldScaleY = image->scale.y;
                    image->scale.y *= textureRatio / quadRatio;
                    image->position.y += (oldScaleY - image->scale.y) * 0.5f;
                } else if((image->header.renderFlags & ImageRenderFlag_KeepRatioY) > 0) {
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
                break;
            }
            case RenderType_RenderImageUV: {
                RenderImageUV *imageUV = (RenderImageUV *)renderHeader;

                UseProgram(texturedProgram);

                GLTexture texture = GL_LoadTexture(imageUV->filepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                imageUV->scale.x *= texture.width;
                imageUV->scale.y *= texture.height;

                model *= ScaleM44(imageUV->scale);
                model *= TranslationM44(imageUV->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                CreateQuadPosUV(0, 0, 1, 1, imageUV->uvMin.x, imageUV->uvMin.y, imageUV->uvMax.x, imageUV->uvMax.y);

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
                break;
            }
            case RenderType_RenderImage9Slice: {
                RenderImage9Slice *image9Slice = (RenderImage9Slice *)renderHeader;

                UseProgram(textured9SliceProgram);

                GLTexture texture = GL_LoadTexture(image9Slice->filepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 box = V2((f32)(image9Slice->endPosition.x - image9Slice->position.x), (f32)(image9Slice->endPosition.y - image9Slice->position.y));

                glUniform2f(dimensionsLocation, image9Slice->slice / box.x, image9Slice->slice / box.y);
                glUniform2f(borderLocation, image9Slice->slice / texture.width, image9Slice->slice / texture.height);

                model *= ScaleM44(box.x, box.y, 1);
                model *= TranslationM44(image9Slice->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                BindBuffer();

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                break;
            }
            case RenderType_RenderAtlasSprite: {
                RenderAtlasSprite *atlas = (RenderAtlasSprite *)renderHeader;

                UseProgram(texturedProgram);

                TextureAtlas textureAtlas = GL_LoadAtlas(atlas->atlasName);
                rectangle2 spriteRect = shget(textureAtlas.sprites, atlas->spriteKey);

                GLTexture texture = GL_LoadTexture(atlas->filepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                atlas->scale.x *= spriteRect.width;
                atlas->scale.y *= spriteRect.height;

                model *= ScaleM44(atlas->scale);
                model *= TranslationM44(atlas->position);
                SetupModelUniforms(coloredProgram, renderState.renderColor, model, view, projection);

                CreateQuadPosUV(0, 0, 1, 1, spriteRect.x / texture.width, spriteRect.y / texture.height, 
                    (spriteRect.x + spriteRect.width) / texture.width, (spriteRect.y + spriteRect.height) / texture.height);

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
                break;
            }
            case RenderType_RenderFont: {
                RenderFont *font = (RenderFont *)renderHeader;
                GL_LoadFont(font->fontID, &currentFont);
                break;
            }
            case RenderType_RenderChar: {
                RenderChar *renderChar = (RenderChar *)renderHeader;

                UseProgram(fontProgram);

                GLTexture texture = GL_LoadTexture(currentFont.fontFilepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                model *= ScaleM44(renderChar->scale);
                model *= TranslationM44(renderChar->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                f32 posX = 0;
                f32 posY = currentFont.fontSize;
                stbtt_aligned_quad quad;
                GetBakedQuad(&currentFont, renderChar->singleChar - SPECIAL_ASCII_CHAR_OFFSET, &posX, &posY, &quad);
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
                break;
            }
            case RenderType_RenderText: {
                RenderText *text = (RenderText *)renderHeader;

                UseProgram(fontProgram);

                GLTexture texture = GL_LoadTexture(currentFont.fontFilepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 textSize = CalculateTextSize(&currentFont, text->string, text->stringSize);

                model *= ScaleM44(1, 1, 1);
                model *= TranslationM44(text->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                f32 posX = 0;
                f32 posY = currentFont.fontSize;
                stbtt_aligned_quad quad;

                u32 lineCharacterCount = 0;
                for(i32 i = 0; i < text->stringSize - 1; ++i) {
                    char currentChar = text->string[i];

                    CalculateCharacterOffset(&currentFont, currentChar, &posX, &posY, &lineCharacterCount);
                    GetBakedQuad(&currentFont, currentChar - SPECIAL_ASCII_CHAR_OFFSET, &posX, &posY, &quad);
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
                break;
            }
            case RenderType_RenderStyledText: {
                RenderStyledText *styledText = (RenderStyledText *)renderHeader;

                UseProgram(fontProgram);

                GLTexture texture = GL_LoadTexture(currentFont.fontFilepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 containerSize = V2(styledText->endPosition.x - styledText->position.x, styledText->endPosition.y - styledText->position.y);
                v2 textSize = CalculateTextSize(&currentFont, styledText->string, styledText->stringSize, containerSize.x);

                if((styledText->header.renderFlags & TextRenderFlag_Center) > 0) {
                    styledText->position.x += (containerSize.x - textSize.x) * 0.5f;
                    styledText->position.y -= (containerSize.y - textSize.y) * 0.5f;
                }

                model *= ScaleM44(1, 1, 1);
                model *= TranslationM44(styledText->position);
                SetupModelUniforms(texturedProgram, renderState.renderColor, model, view, projection);

                f32 posX = 0;
                f32 posY = currentFont.fontSize;
                stbtt_aligned_quad quad;

                bool letterWrap = (styledText->header.renderFlags & TextRenderFlag_LetterWrap) > 0;
                bool wordWrap = (styledText->header.renderFlags & TextRenderFlag_WordWrap) > 0;

                u32 lastWordIndex;

                u32 lineCharacterCount = 0;
                for(u32 i = 0; i < styledText->stringSize - 1; ++i) {
                    char currentChar = styledText->string[i];
                    
                    CalculateCharacterOffset(&currentFont, currentChar, &posX, &posY, &lineCharacterCount);

                    char normalizedChar = currentChar - SPECIAL_ASCII_CHAR_OFFSET;
                    if(normalizedChar >= 0) {
                        if(wordWrap) {
                            f32 wordEndPosition = posX;
                            
                            for(u32 j = i; j < styledText->stringSize - 1 && styledText->string[j] > SPECIAL_ASCII_CHAR_OFFSET; ++j) {
                                wordEndPosition += currentFont.charData[styledText->string[j] - SPECIAL_ASCII_CHAR_OFFSET].xadvance;
                            }

                            if(wordEndPosition + styledText->position.x > styledText->endPosition.x) {
                                lineCharacterCount = 0;
                                posX = 0;
                                posY = posY + currentFont.lineHeight;
                            }
                        }

                        if(letterWrap && posX + currentFont.charData[normalizedChar].xadvance + styledText->position.x > styledText->endPosition.x) {
                            lineCharacterCount = 0;
                            posX = 0;
                            posY = posY + currentFont.lineHeight;
                        }

                        GetBakedQuad(&currentFont, normalizedChar, &posX, &posY, &quad);

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
                    } else {
                        lastWordIndex = i + 1;
                    }
                }
                break;
            }
            case RenderType_RenderSetUniform: {
                RenderSetUniform *uniform = (RenderSetUniform *)renderHeader;

                if(uniform->type == UniformType_Float) {
                    glUniform1f(uniform->location, *((f32*)(uniform + 1)));
                }
                else if(uniform->type == UniformType_Vector2) {
                    v2 vector = *((v2*)(uniform + 1));
                    glUniform2f(uniform->location, vector.x, vector.y);
                }
                break;
            }
            case RenderType_RenderOverrideProgram: {
                RenderOverrideProgram *program = (RenderOverrideProgram *)renderHeader;
 
                renderState.overrideProgram = program->programID;

                UseProgram(program->programID);
                break;
            }
            case RenderType_RenderOverrideVertices: {
                RenderOverrideVertices *vertices = (RenderOverrideVertices *)renderHeader;
 
                renderState.overridingVertices = vertices->vertices != 0;
                if(renderState.overridingVertices) {
                    glBindVertexArray(overrideBuffer.vertexArray);
                    glBindBuffer(GL_ARRAY_BUFFER, overrideBuffer.vertexBuffer);
                    glBufferData(GL_ARRAY_BUFFER, vertices->size, vertices->vertices, GL_STATIC_DRAW);
                }
                break;
            }
            case RenderType_RenderOverrideIndices: {
                RenderOverrideIndices *indices = (RenderOverrideIndices *)renderHeader;

                renderState.overridingIndices = indices->indices != 0;
                if(renderState.overridingIndices) {
                    glBindVertexArray(overrideBuffer.vertexArray);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, overrideBuffer.indexBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size, indices->indices, GL_STATIC_DRAW);
                }
                break;
            }
            default: {
                InvalidCodePath;
                break;
            }
        }

        Assert(renderHeader->size > 0);
        #ifdef GAME_EDITOR
        editorRenderDebugger.drawCount++;
        #endif
        renderHeader = (RenderHeader *)((u8 *)renderHeader + renderHeader->size);
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