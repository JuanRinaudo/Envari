#ifndef GLRENDER_H
#define GLRENDER_H

#define DEFAULT_FONT_ATLAS_WIDTH 1024
#define DEFAULT_FONT_ATLAS_HEIGHT 1024

#define INFO_LOG_BUFFER_SIZE 512

#ifdef GL_PROFILE_GLES3
#include <GLES3/gl3.h>
const char* shaderPath = "shaders/gles";
#else
#include <gl/gl.h>
const char* shaderPath = "shaders/glcore";
#endif

#ifndef NO_DEFAULT_FONT
#include "Default/Font.h"
#endif

static m44 view;
static m44 projection;
static m33 model;

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

static u32 bufferSizeLocation;
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

static MemoryArena* GetTargetArena(bool permanentAsset)
{
    return permanentAsset ? &permanentState->arena : &sceneState->arena;
}

static bool CheckVendor(const char* vendor)
{
    const char* glVendor = (char*)glGetString(GL_VENDOR);
    return strcmp(glVendor, vendor) == 0;
}

static void CleanCache()
{
    // #TODO (Juan): Implement
}

static i32 TotalGPUMemoryKB()
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

static i32 AvailableGPUMemoryKB()
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

static u32 TextureFromMemory(u8 *data, i32 width, i32 height, u32 channels)
{
    AssertMessage(width > 0 && height > 0, "Texture loading error: Image has no width or height");

    u32 textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    u32 format = GL_RGBA;
    if(channels == 3) {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#ifndef MIPMAP_DISABLED
    if(renderState->generateMipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
#endif

    return textureID;
}

void BindTextureID(u32 textureID, f32 width, f32 height)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform2f(textureSizeLocation, width, height);
}

GLTexture LoadTextureFile(const char *texturePath, bool permanentAsset = false)
{
    i32 index = (i32)shgeti(textureCache, texturePath);
    GLTexture texture;
    if(index > -1) {
        texture = shget(textureCache, texturePath);
    } else {
        i32 width, height, channels;
        u8 *data = stbi_load(texturePath, &width, &height, &channels, 0);

        u32 textureID = TextureFromMemory(data, width, height, channels);

        texture.textureID = textureID;
        texture.width = width;
        texture.height = height;
        texture.channels = channels;
        shput(textureCache, PushString(GetTargetArena(permanentAsset), texturePath), texture);

        stbi_image_free(data);
    }
    
    glBindTexture(GL_TEXTURE_2D, texture.textureID);
    glUniform2f(textureSizeLocation, (f32)texture.width, (f32)texture.height);
    
    return texture;
}

v2 TextureSize(const char* texturePath)
{
    i32 index = (i32)shgeti(textureCache, texturePath);
    if(index > -1) {
        GLTexture texture = shget(textureCache, texturePath);
        return V2((f32)texture.width, (f32)texture.height);
    }
    else {
        LogWarning("Texture (%s) is not found", texturePath);
    }

    return V2(0, 0);
}

// #NOTE (Juan): Doesn't work on WASM platform, should look for an alternative
// v2 TextureSize(u32 textureID)
// {
//     glBindTexture(GL_TEXTURE_2D, textureID);
//     f32 width = 0;
//     f32 height = 0;
//     // glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
//     // glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

//     return V2(width, height);
// }

void UnloadTextureFile(const char *texturePath)
{
    i32 index = (i32)shgeti(textureCache, texturePath);
    GLTexture texture;
    if(index > -1) {
        texture = shget(textureCache, texturePath);
        glDeleteTextures(1, &texture.textureID);
        shdel(textureCache, texturePath);
    } else {
        LogWarning("Trying to unload a texture (%s) that is not loaded", texturePath);
    }
}

static TextureAtlas LoadAtlas(const char *atlasKey, bool permanentAsset = false)
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
                keyPointer = PushString(GetTargetArena(permanentAsset), tokenizer.tokenBuffer, tokenizer.tokenBufferIndex);

                i32 x = StringToInt(NextToken(&tokenizer));
                i32 y = StringToInt(NextToken(&tokenizer));
                i32 width = StringToInt(NextToken(&tokenizer));
                i32 height = StringToInt(NextToken(&tokenizer));
                
                shput(atlas.sprites, keyPointer, Rectangle2((f32)x, (f32)y, (f32)width, (f32)height));
            }
        }

        EndTokenizer(&tokenizer);

        shput(atlasCache, atlasKey, atlas);

        return atlas;
    }
}

static u32 LoadFont(u32 fontID, FontAtlas *atlas)
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

static void InitFramebuffer(i32 bufferWidth, i32 bufferHeight)
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

void WritePNG(char *filename, i32 x, i32 y, i32 comp, void *data, i32 stride_bytes)
{
    stbi_write_png(filename, x, y, comp, data, stride_bytes);
    free(filename);
    free(data);
}

#ifndef PLATFORM_WASM
void DumpTexture(const char *filepath, i32 textureID, u32 width, u32 height)
{
    u8* data = (u8*)malloc(width * height * 3);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    char* savePath = (char*)malloc(strlen(filepath));
    strcpy(savePath, filepath);
    std::thread saveImage(WritePNG, savePath, width, height, 3, data, width * 3);
    saveImage.detach();
}
#endif

u32 GenerateBitmapFontStrip(const char *filepath, const char* glyphs, u32 glyphWidth, u32 glyphHeight)
{
    size_t data_size = 0;
    GLTexture texture = LoadTextureFile(filepath);

    size_t glyphCount = strlen(glyphs);

    FontAtlas result;
    result.lineHeight = (f32)glyphHeight;
    result.tabSize = (i32)glyphHeight * 2;
    result.width = glyphWidth * (i32)glyphCount;
    result.height = glyphHeight;
    result.fontSize = (f32)glyphHeight;
    result.fontTextureID = texture.textureID;

    for(i32 i = 0; i < glyphCount; ++i) {
        if(glyphs[i] != '\r') {
            char offsetedGlyph = glyphs[i] - SPECIAL_ASCII_CHAR_OFFSET;
            stbtt_bakedchar* charData = &result.charData[offsetedGlyph];
            charData->x0 = (u16)(i * glyphWidth);
            charData->x1 = (u16)((i + 1) * glyphWidth);
            charData->y0 = 0;
            charData->y1 = (u16)(glyphHeight);
            charData->xadvance = (f32)glyphWidth;
            charData->xoff = 0;
            charData->yoff = 0;
        }
    }

    hmput(fontCache, result.fontTextureID, result);

    return result.fontTextureID;
}

i32 GenerateFont(void* data, size_t data_size, const char *filepath, f32 fontSize, u32 width, u32 height)
{
    FontAtlas result;
    result.lineHeight = fontSize;
    result.tabSize = (i32)fontSize * 2;
    result.width = width;
    result.height = height;
    result.fontSize = fontSize;

    u8* tempBitmap = PushArray(&temporalState->arena, width * height, u8);
    stbtt_BakeFontBitmap((u8 *)data, 0, fontSize, tempBitmap, width, height, SPECIAL_ASCII_CHAR_OFFSET, FONT_CHAR_SIZE, result.charData); // no guarantee this fits!

    glGenTextures(1, &result.fontTextureID);
    glBindTexture(GL_TEXTURE_2D, result.fontTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tempBitmap);
    glGenerateMipmap(GL_TEXTURE_2D);

    hmput(fontCache, result.fontTextureID, result);

    return result.fontTextureID;
}

u32 GenerateFont(const char *filepath, f32 fontSize, u32 width, u32 height)
{
    size_t data_size = 0;
    void* data = LoadFileToMemory(filepath, FILE_MODE_READ_BINARY, &data_size);

    return GenerateFont(data, data_size, filepath, fontSize, width, height);
}

//GL_VERTEX_SHADER GL_FRAGMENT_SHADER
static bool LoadShader(u32 shaderType, const char* filepath, u32* shaderID, size_t* sourceSize)
{
    *shaderID = glCreateShader(shaderType);
            
    *sourceSize = 0;
    void* data = LoadFileToMemory(filepath, FILE_MODE_READ_BINARY, sourceSize);
    SOURCE_TYPE source = static_cast<SOURCE_TYPE>(data);

    i32 size = (i32)*sourceSize;
    glShaderSource(*shaderID, 1, &source, &size);
    glCompileShader(*shaderID);
    
    i32 success;
    char infoLog[INFO_LOG_BUFFER_SIZE];
    glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(*shaderID, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
        if(shaderType == GL_VERTEX_SHADER) {
            LogError("ERROR::VERTEX::COMPILATION_FAILED %s\n", filepath);
        }
        else if(shaderType == GL_FRAGMENT_SHADER) {
            LogError("ERROR::FRAGMENT::COMPILATION_FAILED %s\n", filepath);
        }
        LogError(infoLog);
        return 0;
    }

    UnloadFileFromMemory(data);

    return success;
}

u32 CompileProgram(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    // NOTE(Juan): Shaders
    u32 vertexShader, fragmentShader;
    size_t vertexShaderSize, fragmentShaderSize;
    LoadShader(GL_VERTEX_SHADER, vertexShaderPath, &vertexShader, &vertexShaderSize);
    LoadShader(GL_FRAGMENT_SHADER, fragmentShaderPath, &fragmentShader, &fragmentShaderSize);

    i32 shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    i32 success;
    char infoLog[INFO_LOG_BUFFER_SIZE];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
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
    watched.vertexTime = filesystem::last_write_time(vertexShaderPath);
    watched.fragmentTime = filesystem::last_write_time(fragmentShaderPath);

    watchedPrograms[watchedProgramsCount] = watched;
    watchedProgramsCount++;
    #endif

    return shaderProgram;
}

u32 CompileProgramPlatform(const char *vertexShaderPlatform, const char *fragmentShaderPlatform)
{
    char* vertexShaderPath = PushString(&temporalState->arena, SHADER_PREFIX, sizeof(SHADER_PREFIX) - 1);
    PushString(&temporalState->arena, vertexShaderPlatform);
    
    char* fragmentShaderPath = PushString(&temporalState->arena, SHADER_PREFIX, sizeof(SHADER_PREFIX) - 1);
    PushString(&temporalState->arena, fragmentShaderPlatform);
    
    return CompileProgram(vertexShaderPath, fragmentShaderPath);
}

static void WatchChanges()
{
    #ifdef GAME_EDITOR
    for(i32 i = 0; i < watchedProgramsCount; ++i) {
        WatchedProgram watched = watchedPrograms[i];

        filesystem::file_time_type vertexTime = filesystem::last_write_time(watched.vertexFilepath);
        filesystem::file_time_type fragmentTime = filesystem::last_write_time(watched.fragmentFilepath);

        if(vertexTime != watched.vertexTime || fragmentTime != watched.fragmentTime) {
            watched.vertexTime = vertexTime;
            watched.fragmentTime = fragmentTime;
            watchedPrograms[i] = watched;

            Log("Started to reload program %d, vertex %s (%d), fragment %s (%d)", watched.shaderProgram, watched.vertexFilepath, watched.vertexShader, watched.fragmentFilepath, watched.fragmentShader);
            glDetachShader(watched.shaderProgram, watched.vertexShader);
            glDetachShader(watched.shaderProgram, watched.fragmentShader);
            
            size_t vertexSouceSize, fragmentSouceSize;
            
            i32 success;

            success = LoadShader(GL_VERTEX_SHADER, watched.vertexFilepath, &watched.vertexShader, &vertexSouceSize);

            if(success) {
                success = LoadShader(GL_FRAGMENT_SHADER, watched.fragmentFilepath, &watched.fragmentShader, &fragmentSouceSize);
            }

            if(success) {
                glAttachShader(watched.shaderProgram, watched.vertexShader);
                glAttachShader(watched.shaderProgram, watched.fragmentShader);
                glLinkProgram(watched.shaderProgram);
            }

            char infoLog[INFO_LOG_BUFFER_SIZE];
            glGetProgramiv(watched.shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glDeleteShader(watched.vertexShader);
                glDeleteShader(watched.fragmentShader);
                glGetProgramInfoLog(watched.shaderProgram, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
                LogError("ERROR::PROGRAM::LINK_FAILED\n");
                LogError(infoLog);
                return;
            }

            glDeleteShader(watched.vertexShader);
            glDeleteShader(watched.fragmentShader);
        }
    }
    #endif
}

static bool GetBakedQuad(FontAtlas *font, u32 charIndex, float *xpos, float *ypos, stbtt_aligned_quad *q)
{
    if(charIndex >= 0 && charIndex < FONT_CHAR_SIZE) {
        f32 ipw = 1.0f / font->width;
        f32 iph = 1.0f / font->height;
        const stbtt_bakedchar *b = font->charData + charIndex;
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
        return true;
    }
    else {
        return false;
    }
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

static v2 CalculateTextSize(FontAtlas *font, const char* string, size_t stringSize, f32 containerWidth = -1)
{
    f32 lineWidth = 0;
    f32 lineHeight = 0;
    f32 width = 0;
    f32 height = 0;
    u32 lineCharacterCount = 0;
    for(i32 i = 0; i < stringSize; ++i) {
        char singleChar = string[i];
        
        stbtt_bakedchar charData = font->charData[singleChar - SPECIAL_ASCII_CHAR_OFFSET];
        if(singleChar >= SPECIAL_ASCII_CHAR_OFFSET) {
            lineHeight = MAX(lineHeight, (f32)charData.y1 - (f32)charData.y0);
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

    if(renderState->overrideProgram) { programID = renderState->overrideProgram; }

    if(programID != renderState->currentProgram) {
        #ifdef GAME_EDITOR
        editorRenderDebugger.programChanges++;
        #endif

        glUseProgram(programID);

        colorLocation = glGetUniformLocation(programID, "color");
        mvpLocation = glGetUniformLocation(programID, "mvp");

        textureSizeLocation = glGetUniformLocation(programID, "textureSize");

        bufferSizeLocation = glGetUniformLocation(programID, "bufferSize");
        glUniform2f(bufferSizeLocation, gameState->render.bufferSize.x, gameState->render.bufferSize.y);
        timeLocation = glGetUniformLocation(programID, "time");

        renderState->currentProgram = programID;
    }

    if(timeLocation != 0)
    {
        glUniform1f(timeLocation, gameState->time.gameTime);
    }
}

static void SetupTextureParameters(u32 textureTarget)
{
    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, renderState->wrapS);
    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, renderState->wrapT);
    glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, renderState->minFilter);
    glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, renderState->magFilter);
}

static m33 SetupModelMatrix(v2 origin = V2(0, 0), v2 size = V2(1, 1), f32 angle = 0)
{
    m33 matrix = IdM33();
    matrix *= ScaleM33(size);
    matrix *= RotateM33(angle);
    matrix *= TranslationM33(origin);
    m33* parentTransform = gameState->render.transformStack + gameState->render.transformIndex;
    matrix *= *parentTransform;
    return matrix;
}

static void SetupMVPUniforms(u32 programID, v4 color)
{
    if(renderState->overrideProgram) { programID = renderState->overrideProgram; }

    glUniform4f(colorLocation, renderState->renderColor.r, renderState->renderColor.g, renderState->renderColor.b, renderState->renderColor.a);

    m44 mvp = (projection * view) * M44(model);

    glUniformMatrix4fv(mvpLocation, 1, false, mvp.e);
}

static void BindBuffer()
{
    if(renderState->overridingVertices || renderState->overridingIndices) {
        glBindVertexArray(overrideBuffer.vertexArray);
    }
    else {
        glBindVertexArray(quadBuffer.vertexArray);
    }

    if(renderState->overridingVertices) {
        glBindBuffer(GL_ARRAY_BUFFER, overrideBuffer.vertexBuffer);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, quadBuffer.vertexBuffer);
    }

    if(renderState->overridingIndices) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, overrideBuffer.indexBuffer);
    }
    else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadBuffer.indexBuffer);
    }
}

static void InitGL()
{
    coloredProgram = CompileProgramPlatform(COLORED_VERT, COLORED_FRAG);
    fontProgram = CompileProgramPlatform(TEXTURED_VERT, FONT_FRAG);
    texturedProgram = CompileProgramPlatform(TEXTURED_VERT, TEXTURED_FRAG);
    textured9SliceProgram = CompileProgramPlatform(TEXTURED_VERT, TEXTURED9SLICE_FRAG);
    
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
    if(CheckVendor(GL_STRING_VENDOR_NVIDIA)) {
        currentVendor = GL_VENDOR_NVIDIA;
    }
    else if(CheckVendor(GL_STRING_VENDOR_ATI)) {
        currentVendor = GL_VENDOR_ATI;
    }
}

static void DefaultAssets()
{
#ifndef NO_DEFAULT_FONT
    gameState->render.defaultFontID = GenerateFont(defaultFont, sizeof(defaultFont), "defaultFont", 64, DEFAULT_FONT_ATLAS_WIDTH, DEFAULT_FONT_ATLAS_HEIGHT);
#endif
}

static void RenderStyledText_(RenderStyledText* styledText)
{
    UseProgram(fontProgram);

    BindTextureID(currentFont.fontTextureID, (f32)currentFont.width, (f32)currentFont.height);
    SetupTextureParameters(GL_TEXTURE_2D);

    v2 containerSize = V2(styledText->endOrigin.x - styledText->origin.x, styledText->endOrigin.y - styledText->origin.y);
    v2 textSize = CalculateTextSize(&currentFont, styledText->string, styledText->stringSize, containerSize.x);

    v2 origin = V2(styledText->origin.x, styledText->origin.y);

    if((styledText->header.renderFlags & TextRenderFlag_Left) > 0) {
        origin.y -= (containerSize.y - textSize.y) * 0.5f;  
    }
    else if((styledText->header.renderFlags & TextRenderFlag_Center) > 0) {
        origin.x += (containerSize.x - textSize.x) * 0.5f;
        origin.y -= (containerSize.y - textSize.y) * 0.5f;
    }
    else if((styledText->header.renderFlags & TextRenderFlag_Right) > 0) {
        origin.x += (containerSize.x - textSize.x);
        origin.y -= (containerSize.y - textSize.y) * 0.5f;
    }                

    model = SetupModelMatrix(origin);
    SetupMVPUniforms(texturedProgram, renderState->renderColor);

    f32 posX = 0;
    f32 posY = currentFont.lineHeight;
    stbtt_aligned_quad quad;

    bool letterWrap = (styledText->header.renderFlags & TextRenderFlag_LetterWrap) > 0;
    bool wordWrap = (styledText->header.renderFlags & TextRenderFlag_WordWrap) > 0;

    u32 lastWordIndex;

    u32 lineCharacterCount = 0;
    u32 utfSize = 1;
    for(u32 i = 0; i < styledText->stringSize - 1; i += utfSize) {
        u32 currentChar = GetUTF8Char(styledText->string + i, &utfSize);
        
        CalculateCharacterOffset(&currentFont, (char)currentChar, &posX, &posY, &lineCharacterCount);

        u32 normalizedChar = currentChar - SPECIAL_ASCII_CHAR_OFFSET;
        if(normalizedChar >= 0) {
            if(wordWrap) {
                f32 wordEndOrigin = posX;
                
                for(u32 j = i; j < styledText->stringSize - 1 && styledText->string[j] > SPECIAL_ASCII_CHAR_OFFSET; ++j) {
                    wordEndOrigin += currentFont.charData[styledText->string[j] - SPECIAL_ASCII_CHAR_OFFSET].xadvance;
                }

                if(wordEndOrigin + styledText->origin.x > styledText->endOrigin.x) {
                    lineCharacterCount = 0;
                    posX = 0;
                    posY = posY + currentFont.lineHeight;
                }
            }

            if(letterWrap && posX + currentFont.charData[normalizedChar].xadvance + styledText->origin.x > styledText->endOrigin.x) {
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
}

static void RenderImage9Slice_(RenderImage9Slice* image9Slice)
{
    UseProgram(textured9SliceProgram);

    GLTexture texture = LoadTextureFile(image9Slice->filepath);
    SetupTextureParameters(GL_TEXTURE_2D);

    v2 size = V2((f32)(image9Slice->endOrigin.x - image9Slice->origin.x), (f32)(image9Slice->endOrigin.y - image9Slice->origin.y));

    glUniform2f(dimensionsLocation, image9Slice->slice / size.x, image9Slice->slice / size.y);
    glUniform2f(borderLocation, image9Slice->slice / texture.width, image9Slice->slice / texture.height);

    model = SetupModelMatrix(image9Slice->origin, size);
    SetupMVPUniforms(texturedProgram, renderState->renderColor);

    BindBuffer();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static void RenderPass()
{
    RenderHeader *renderHeader = (RenderHeader *)renderTemporaryMemory.arena->base;

    // #NOTE (Juan): Check if there are layers setted and sort commands by layer
    bool hasLayers = renderState->usedLayers > 0;
#if GAME_EDITOR
    hasLayers = hasLayers && (editorState->editorFrameRunning || editorState->playNextFrame);
#endif
    if(hasLayers) {
        void *sortedRenderStart = renderTemporaryMemory.arena->base + renderTemporaryMemory.arena->used;
        
        for(u32 layerIndex = 31; layerIndex < 32; --layerIndex) {
            if((renderState->usedLayers & (1 << layerIndex)) > 0) {
                RenderHeader* layerStartHeader = (RenderHeader*)(renderTemporaryMemory.arena->base + renderTemporaryMemory.arena->used);
                if((renderState->transparentLayers & (1 << layerIndex)) > 0) {
                    DrawTransparent();
                } else {
                    DrawTransparentDisable();
                }

                RenderHeader *searchHeader = (RenderHeader *)renderTemporaryMemory.arena->base;
                u32 searchLayer = 0;
                while(searchHeader->id > 0 && (void*)searchHeader < sortedRenderStart) {
                    if(searchHeader->type == RenderType_RenderLayer) {
                        searchLayer = ((RenderLayer *)searchHeader)->layer;
                    }
                    else if(searchLayer == layerIndex) {
                        void* copyDestination = PushSize(&renderTemporaryMemory, searchHeader->size);
                        memcpy(copyDestination, (void *)searchHeader, searchHeader->size);
                        RenderHeader* copyHeader = (RenderHeader*)copyDestination;
                        copyHeader->id += layerStartHeader->id;
                    }
                    searchHeader = (RenderHeader *)((u8 *)searchHeader + searchHeader->size);
                }
            }
        }

        renderHeader = (RenderHeader*)sortedRenderStart;
#if GAME_EDITOR
        editorState->savedRenderHeader = renderHeader;
#endif
    }
    RenderHeader* sortedRenderStart = renderHeader;

#if GAME_EDITOR
    if(!(editorState->editorFrameRunning || editorState->playNextFrame)) {
        renderHeader = editorState->savedRenderHeader;
    }
#endif

    view = gameState->camera.view;
    view._30 = -gameState->camera.size * 0.5f;
    view._31 = view._30;
    view._23 = 1.0f;
    projection = gameState->camera.projection;

    while(renderHeader->id > 0 && (void*)renderHeader < (void*)(renderTemporaryMemory.arena->base + renderTemporaryMemory.used)) {
        model = IdM33();

#if GAME_EDITOR
        if(!renderHeader->enabled) {
            renderHeader = (RenderHeader *)((u8 *)renderHeader + renderHeader->size);
            continue;
        }
#endif

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
            case RenderType_RenderSetStyle: {
                RenderSetStyle *setStyle = (RenderSetStyle *)renderHeader;
                renderState->style = setStyle->style;
                break;
            }
            case RenderType_RenderColor: {
                RenderColor *color = (RenderColor *)renderHeader;
                renderState->renderColor = color->color;
                break;
            }
            case RenderType_RenderSetTransform: {
                RenderSetTransform *setTransform = (RenderSetTransform *)renderHeader;
                m33 matrix = IdM33();
                matrix *= ScaleM33(setTransform->transform.scale);
                matrix *= RotateM33(setTransform->transform.angle);
                matrix *= TranslationM33(setTransform->transform.position);
                gameState->render.transformStack[0] = matrix;
                gameState->render.transformIndex = 0;
                break;
            }
            case RenderType_RenderPushTransform: {
                RenderPushTransform *pushTransform = (RenderPushTransform *)renderHeader;
                m33 lastMatrix = gameState->render.transformStack[gameState->render.transformIndex];
                gameState->render.transformIndex++;
                lastMatrix *= ScaleM33(pushTransform->transform.scale);
                lastMatrix *= RotateM33(pushTransform->transform.angle);
                lastMatrix *= TranslationM33(pushTransform->transform.position);
                gameState->render.transformStack[gameState->render.transformIndex] = lastMatrix;
                break;
            }
            case RenderType_RenderPopTransform: {
                RenderPopTransform *popTransform = (RenderPopTransform *)renderHeader;
                gameState->render.transformIndex--;
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
                    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
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

                model = SetupModelMatrix();
                SetupMVPUniforms(coloredProgram, renderState->renderColor);

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

                model = SetupModelMatrix();
                SetupMVPUniforms(coloredProgram, renderState->renderColor);

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

                model = SetupModelMatrix(rectangle->origin, rectangle->size);
                SetupMVPUniforms(coloredProgram, renderState->renderColor);

                glBindVertexArray(quadBuffer.vertexArray);
                glBindBuffer(GL_ARRAY_BUFFER, quadBuffer.vertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadBuffer.indexBuffer);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                break;
            }
            case RenderType_RenderCircle: {
                RenderCircle *circle = (RenderCircle *)renderHeader;

                UseProgram(coloredProgram);

                model = SetupModelMatrix(circle->origin);
                SetupMVPUniforms(coloredProgram, renderState->renderColor);

                f32 vertices[300] = {
                    0.0f, 0.0f, 0.0f,
                };

                u32 indices[300] = {

                };

                for(i32 i = 0; i < circle->segments; ++i) {
                    float angle = ((float)i / (float)circle->segments) * PI32 * 2;
                    vertices[i * 3 + 3] = Sin(angle) * circle->radius;
                    vertices[i * 3 + 4] = Cos(angle) * circle->radius;
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
                
                renderState->wrapS = textureParameters->wrapS;
                renderState->wrapT = textureParameters->wrapT;
                renderState->minFilter = textureParameters->minFilter;
                renderState->magFilter = textureParameters->magFilter;
                break;
            }
            case RenderType_RenderTexture: {
                RenderTexture *texture = (RenderTexture *)renderHeader;

                UseProgram(texturedProgram);

                model = SetupModelMatrix(texture->origin, texture->size);
                SetupMVPUniforms(texturedProgram, renderState->renderColor);

                glBindTexture(GL_TEXTURE_2D, texture->textureID);
                glUniform2f(textureSizeLocation, texture->size.x, texture->size.y);
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

                bool generateMipMapsTemp = renderState->generateMipMaps;
                // if((image->header.renderFlags & ImageRenderFlag_NoMipMaps) > 0) {
                //     renderState->generateMipMaps = false;
                // }

                GLTexture texture = LoadTextureFile(image->filepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                renderState->generateMipMaps = generateMipMapsTemp;

                v2 origin = V2(-image->origin.x * texture.width, -image->origin.y * texture.height);
                v2 size = V2((f32)texture.width, (f32)texture.height);

                f32 quadRatio = size.y / size.x;
                f32 textureRatio = (f32)texture.height / (f32)texture.width;

                // #TODO (Juan): Fix this!
                // if((image->header.renderFlags & ImageRenderFlag_Fit) > 0) {
                //     // #TODO(Juan): Check this and fix errors
                //     if(texture.height > texture.width) {
                //         scale.x *= (f32)texture.width / (f32)texture.height;
                //         origin.x += (image->size.x - scale.x) * 0.5f;
                //     } else {
                //         scale.y *= (f32)texture.width / (f32)texture.height;
                //         origin.y += (image->size.y - scale.y) * 0.5f;
                //     }
                // } else if((image->header.renderFlags & ImageRenderFlag_KeepRatioX) > 0) {
                //     scale.y *= textureRatio / quadRatio;
                //     origin.y += (image->size.y - scale.y) * 0.5f;
                // } else if((image->header.renderFlags & ImageRenderFlag_KeepRatioY) > 0) {
                //     scale.x *= textureRatio / quadRatio;
                //     origin.x += (image->size.x - scale.x) * 0.5f;
                // }

                model = SetupModelMatrix(origin, size);
                SetupMVPUniforms(texturedProgram, renderState->renderColor);

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

                GLTexture texture = LoadTextureFile(imageUV->filepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 origin = V2(-imageUV->origin.x * texture.width, -imageUV->origin.y * texture.height);
                v2 size = V2((f32)texture.width, (f32)texture.height);

                model = SetupModelMatrix(origin, size);
                SetupMVPUniforms(texturedProgram, renderState->renderColor);

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
                RenderImage9Slice_(image9Slice);
                break;
            }
            case RenderType_RenderAtlasSprite: {
                RenderAtlasSprite *atlas = (RenderAtlasSprite *)renderHeader;

                UseProgram(texturedProgram);

                TextureAtlas textureAtlas = LoadAtlas(atlas->atlasName);
                rectangle2 spriteRect = shget(textureAtlas.sprites, atlas->spriteKey);

                GLTexture texture = LoadTextureFile(atlas->filepath);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 origin = V2(-atlas->origin.x * texture.width, -atlas->origin.y * texture.height);
                v2 size = V2(spriteRect.width, spriteRect.height);

                model = SetupModelMatrix(origin, size);
                SetupMVPUniforms(coloredProgram, renderState->renderColor);

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
                LoadFont(font->fontID, &currentFont);
                break;
            }
            case RenderType_RenderChar: {
                RenderChar *renderChar = (RenderChar *)renderHeader;

                UseProgram(fontProgram);

                BindTextureID(currentFont.fontTextureID, (f32)currentFont.width, (f32)currentFont.height);
                SetupTextureParameters(GL_TEXTURE_2D);

                model = SetupModelMatrix(renderChar->origin);
                SetupMVPUniforms(texturedProgram, renderState->renderColor);

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

                BindTextureID(currentFont.fontTextureID, (f32)currentFont.width, (f32)currentFont.height);
                SetupTextureParameters(GL_TEXTURE_2D);

                v2 textSize = CalculateTextSize(&currentFont, text->string, text->stringSize);

                model = SetupModelMatrix(text->origin);
                SetupMVPUniforms(texturedProgram, renderState->renderColor);

                f32 posX = 0;
                f32 posY = currentFont.lineHeight;
                stbtt_aligned_quad quad;

                u32 lineCharacterCount = 0;
                u32 utfSize = 1;
                for(u32 i = 0; i < text->stringSize - 1; i += utfSize) {
                    u32 currentChar = GetUTF8Char(text->string + i, &utfSize);

                    CalculateCharacterOffset(&currentFont, (char)currentChar, &posX, &posY, &lineCharacterCount);
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
                RenderStyledText_(styledText);
                break;
            }
            case RenderType_RenderButton: {
                RenderButton *button = (RenderButton *)renderHeader;

                char* filepath = renderState->style.slicedFilepath;
                if(button->state == ButtonState_Hovered) {
                    filepath = renderState->style.slicedHoveredFilepath;
                }
                else if(button->state == ButtonState_Down) {
                    filepath = renderState->style.slicedDownFilepath;
                }

                if(!filepath) {
                    LogError("No style found. Render type %d, ID %d", renderHeader->type, renderHeader->id);
                    break;
                }

                RenderImage9Slice image9Slice = {};
                image9Slice.filepath = filepath;
                image9Slice.origin = button->origin;
                image9Slice.endOrigin = button->endOrigin;
                image9Slice.slice = renderState->style.slice;
                image9Slice.header.id = -1;
                image9Slice.header.renderFlags = 0;
                RenderImage9Slice_(&image9Slice);

                RenderStyledText styledText = {};
                styledText.origin = button->origin;
                styledText.endOrigin = button->endOrigin;
                styledText.string = button->label;
                styledText.stringSize = button->labelSize;
                styledText.header.id = -1;
                styledText.header.renderFlags = TextRenderFlag_Center | TextRenderFlag_WordWrap;
                RenderStyledText_(&styledText);
            
                break;
            }
            case RenderType_RenderInput: {
                RenderInput *input = (RenderInput *)renderHeader;

                v4 savedColor = renderState->renderColor;
                if(input->baseText) {
                    renderState->renderColor = V4(0.75f, 0.75f, 0.75f, 0.75f);
                }

                RenderStyledText styledText = {};
                styledText.origin = input->origin;
                styledText.endOrigin = input->endOrigin;
                styledText.string = input->input;
                styledText.stringSize = input->inputSize;
                styledText.header.id = -1;
                styledText.header.renderFlags = 0;
                RenderStyledText_(&styledText);

                renderState->renderColor = savedColor;

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
 
                renderState->overrideProgram = program->programID;

                UseProgram(program->programID);
                break;
            }
            case RenderType_RenderOverrideVertices: {
                RenderOverrideVertices *vertices = (RenderOverrideVertices *)renderHeader;
 
                renderState->overridingVertices = vertices->vertices != 0;
                if(renderState->overridingVertices) {
                    glBindVertexArray(overrideBuffer.vertexArray);
                    glBindBuffer(GL_ARRAY_BUFFER, overrideBuffer.vertexBuffer);
                    glBufferData(GL_ARRAY_BUFFER, vertices->size, vertices->vertices, GL_STATIC_DRAW);
                }
                break;
            }
            case RenderType_RenderOverrideIndices: {
                RenderOverrideIndices *indices = (RenderOverrideIndices *)renderHeader;

                renderState->overridingIndices = indices->indices != 0;
                if(renderState->overridingIndices) {
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

        AssertMessage(renderHeader->size > 0, "Rendering loop error: header has no size");
        #ifdef GAME_EDITOR
        editorRenderDebugger.drawCount++;
        #endif
        renderHeader = (RenderHeader *)((u8 *)renderHeader + renderHeader->size);
    }
}

#endif