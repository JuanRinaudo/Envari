#if !defined(GLRENDER_H)
#define GLRENDER_H

#include "Defines.h"

#ifdef GL_PROFILE_GLES3
#include <GLES3/gl3.h>

const char* shaderPath = "shaders/gles";

#else
#include <gl/gl.h>

const char* shaderPath = "shaders/glcore";

#endif

u32 glVertexBuffer;
u32 glIndexBuffer;
u32 glVertexArray;

u32 coloredProgram;
u32 texturedProgram;

struct GLTexture {
    u32 textureID;
    u32 width;
    u32 height;
    u32 channels;
};
struct GLTextureCache {
    char* key;
    GLTexture value;
};
GLTextureCache* textureCache = NULL;

#define SPECIAL_ASCII_CHAR_OFFSET 32
#define FONT_CHAR_SIZE 96
struct FontAtlas {
    char* fontFilename;
    u32 fontFilenameSize;
    f32 fontSize;
    u32 width;
    u32 height;
    stbtt_bakedchar charData[FONT_CHAR_SIZE];
};
FontAtlas currentFont;

struct GLFontReference {
    char* key;
    FontAtlas value;
};
GLFontReference* fontCache = NULL;

enum TextStyles {
    style_Normal = 0,
    style_SineX = 1,
    style_SineY = 2,
    style_FadeIn = 3,
    style_FadeOut = 4,
    style_Typewriter = 5
};

m44 model;
m44 view;
m44 projection;

enum GLVendor {
    GL_VENDOR_UNKOWN,
    GL_VENDOR_NVIDIA,
    GL_VENDOR_ATI
};
GLVendor currentVendor = GL_VENDOR_UNKOWN;

#define GL_STRING_VENDOR_ATI "ATI Technologies Inc."
#define GL_STRING_VENDOR_NVIDIA "NVIDIA Corporation"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

// GLuint uNoOfGPUs = wglGetGPUIDsAMD( 0, 0 );
// GLuint* uGPUIDs = new GLuint[uNoOfGPUs];
// wglGetGPUIDsAMD( uNoOfGPUs, uGPUIDs );

#define FILE_BUFFER_SIZE 1<<20
char fileBuffer[FILE_BUFFER_SIZE];

#if GAME_INTERNAL
#include <filesystem>

struct WatchedProgram {
    u32 vertexShader;
    u32 fragmentShader;
    u32 shaderProgram;
    char vertexFilename[100];
    char fragmentFilename[100];
    std::filesystem::file_time_type vertexTime;
    std::filesystem::file_time_type fragmentTime;
};

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
    i32 index = shgeti(textureCache, textureKey);
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

static FontAtlas GL_LoadFont(const char *filename, f32 fontSize, u32 width, u32 height)
{
    i32 index = shgeti(fontCache, filename);
    if(index > -1) {
        return shget(fontCache, filename); 
    } else {
        FontAtlas result;
        result.fontFilename = PushString(&permanentState->arena, filename, &result.fontFilenameSize);
        result.fontSize = fontSize;
        result.width = width;
        result.height = height;

        u32 data_size = 0;
        void* data = ImFileLoadToMemory(filename, "rb", &data_size, 0);

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
        shput(textureCache, filename, texture);
        shput(fontCache, filename, result);

        return result;
    }
}

static void GL_Init()
{
    glGenBuffers(1, &glVertexBuffer);
    glGenBuffers(1, &glIndexBuffer);
    
    glGenVertexArrays(1, &glVertexArray);

    // #NOTE(Juan): Check GPU vendor
    if(GL_CheckVendor(GL_STRING_VENDOR_NVIDIA)) {
        currentVendor = GL_VENDOR_NVIDIA;
    }
    else if(GL_CheckVendor(GL_STRING_VENDOR_ATI)) {
        currentVendor = GL_VENDOR_ATI;
    }
}

static i32 GL_CompileProgram(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    // NOTE(Juan): Shaders
    u32 vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    u32 data_size = 0;
    void* data = ImFileLoadToMemory(vertexShaderSource, "rb", &data_size, 0);
    const char* const vertexSource = static_cast<const char* const>(data);
    
    glShaderSource(vertexShader, 1, &vertexSource, &((i32)data_size));
    glCompileShader(vertexShader);

    i32 success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        console.AddLog("[error] ERROR::VERTEX::COMPILATION_FAILED %s\n", vertexShaderSource);
        console.AddLog(infoLog);
    }

    u32 fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    data = ImFileLoadToMemory(fragmentShaderSource, "rb", &data_size, 0);
    const char* const fragmentSource = static_cast<const char* const>(data);

    glShaderSource(fragmentShader, 1, &fragmentSource, &((i32)data_size));
    glCompileShader(fragmentShader);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        console.AddLog("[error] ERROR::FRAGMENT::COMPILATION_FAILED %s\n", fragmentShaderSource);
        console.AddLog(infoLog);
    }

    i32 shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        console.AddLog("[error] ERROR::PROGRAM::COMPILATION_FAILED\n");
        console.AddLog(infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    #if GAME_INTERNAL
    WatchedProgram watched;
    watched.vertexShader = vertexShader;
    watched.fragmentShader = fragmentShader;
    watched.shaderProgram = shaderProgram;
    strcpy(watched.vertexFilename, vertexShaderSource);
    strcpy(watched.fragmentFilename, fragmentShaderSource);
    watched.vertexTime = std::filesystem::last_write_time(vertexShaderSource);
    watched.fragmentTime = std::filesystem::last_write_time(fragmentShaderSource);

    watchedPrograms[watchedProgramsCount] = watched;
    watchedProgramsCount++;
    #endif

    return shaderProgram;
}

static void GL_WatchChanges()
{
    #if GAME_INTERNAL
    for(i32 i = 0; i < watchedProgramsCount; ++i) {
        WatchedProgram watched = watchedPrograms[i];

        std::filesystem::file_time_type vertexTime = std::filesystem::last_write_time(watched.vertexFilename);
        std::filesystem::file_time_type fragmentTime = std::filesystem::last_write_time(watched.fragmentFilename);

        if(vertexTime != watched.vertexTime || fragmentTime != watched.fragmentTime) {
            u32 vertexSouceSize = 0;
            void* data = ImFileLoadToMemory(watched.vertexFilename, "rb", &vertexSouceSize, 0);
            const char* const vertexSource = static_cast<const char* const>(data);

            u32 fragmentSouceSize = 0;
            data = ImFileLoadToMemory(watched.fragmentFilename, "rb", &fragmentSouceSize, 0);
            const char* const fragmentSource = static_cast<const char* const>(data);

            if(vertexSource[0] != '\0' && fragmentSource[0] != '\0') {
                console.AddLog("Started to reload program %d, vertex %s, fragment %s", watched.shaderProgram, watched.vertexFilename, watched.fragmentFilename);
                glDetachShader(watched.shaderProgram, watched.vertexShader);
                glDetachShader(watched.shaderProgram, watched.fragmentShader);
                
                watched.vertexShader = glCreateShader(GL_VERTEX_SHADER);
                
                glShaderSource(watched.vertexShader, 1, &vertexSource, &((i32)vertexSouceSize));
                glCompileShader(watched.vertexShader);

                i32 success;
                char infoLog[512];
                glGetShaderiv(watched.vertexShader, GL_COMPILE_STATUS, &success);

                if (!success)
                {
                    glGetShaderInfoLog(watched.vertexShader, 512, NULL, infoLog);
                    console.AddLog("[error] ERROR::VERTEX::COMPILATION_FAILED %s\n", watched.vertexFilename);
                    console.AddLog(infoLog);
                }

                watched.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

                glShaderSource(watched.fragmentShader, 1, &fragmentSource, &((i32)fragmentSouceSize));
                glCompileShader(watched.fragmentShader);

                if (!success)
                {
                    glGetShaderInfoLog(watched.vertexShader, 512, NULL, infoLog);
                    console.AddLog("[error] ERROR::FRAGMENT::COMPILATION_FAILED %s\n", watched.fragmentFilename);
                    console.AddLog(infoLog);
                }

                glAttachShader(watched.shaderProgram, watched.vertexShader);
                glAttachShader(watched.shaderProgram, watched.fragmentShader);
                glLinkProgram(watched.shaderProgram);

                glGetProgramiv(watched.shaderProgram, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(watched.shaderProgram, 512, NULL, infoLog);
                    console.AddLog("[error] ERROR::PROGRAM::COMPILATION_FAILED\n");
                    console.AddLog(infoLog);
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

static stbtt_bakedchar *CalculateCharacterOffset(FontAtlas *Font, char Char, v2 *Offset, float LineHeight)
{
    stbtt_bakedchar *bakedChar;
    if(Char < SPECIAL_ASCII_CHAR_OFFSET) {
        if(Char == 10) {
            Offset->x = 0;
            Offset->y += LineHeight;
        }
        bakedChar = 0;
    }
    else {
        bakedChar = Font->charData + Char - SPECIAL_ASCII_CHAR_OFFSET;
    }

    return bakedChar;
}

// static void DrawString(FontAtlas *font, const char* string, f32 x, f32 y, v4 color)
// {
//     v2 offset = V2(0, 0);
//     for(i32 index = 0; string[index] != 0; ++index) {
//         char singleChar = string[index];
//         stbtt_bakedchar *charData = CalculateCharacterOffset(font, singleChar, &offset, font->fontSize);

//         if(charData != 0) {
//             DrawChar(font, charData, x + offset.x, y + offset.y, color);
//             offset.x += charData->xadvance;
//         }
//     }
// }

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

static void GL_Render()
{
    glViewport(0, 0, gameState->screen.width, gameState->screen.height);

    RenderHeader *renderHeader = (RenderHeader *)renderTemporaryMemory.arena->base;

    model = lua["model"];
    view = lua["view"];
    view._23 = 1.0f;
    projection = lua["projection"];

    while(renderHeader->id > 0 && (void*)renderHeader < (void*)(renderTemporaryMemory.arena->base + renderTemporaryMemory.used)) {
        i32 size = 0;
        // #TODO #PERFORMANCE (Juan): This could be improved by generating some geometries on init time (rectangles and circles?)
        // #TODO (Juan): More render types can be added line, spline, etc
        switch(renderHeader->type) {
            case type_RenderClear: {
                RenderClear *clear = (RenderClear *)renderHeader;
                glViewport(0, 0, gameState->screen.width, gameState->screen.height);
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
            case type_RenderTriangle: {
                RenderTriangle *triangle = (RenderTriangle *)renderHeader;
                
                glUseProgram(coloredProgram);

                i32 colorLocation = glGetUniformLocation(coloredProgram, "color");
                i32 modelLocation = glGetUniformLocation(coloredProgram, "model");
                i32 viewLocation = glGetUniformLocation(coloredProgram, "view");
                i32 projectionLocation = glGetUniformLocation(coloredProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.e);
                glUniformMatrix4fv(viewLocation, 1, false, view.e);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.e);

                f32 vertices[] = {
                    triangle->position.x + triangle->point1.x, triangle->position.y + triangle->point1.y, 0.0f,
                    triangle->position.x + triangle->point2.x, triangle->position.y + triangle->point2.y, 0.0f,
                    triangle->position.x + triangle->point3.x, triangle->position.y + triangle->point3.y, 0.0f
                };

                u32 indices[] = {
                    0, 1, 2
                };

                glBindVertexArray(glVertexArray);
                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glDrawArrays(GL_TRIANGLES, 0, 3);

                size = sizeof(RenderTriangle);
                break;
            }
            case type_RenderRectangle: {
                RenderRectangle *rectangle = (RenderRectangle *)renderHeader;

                glUseProgram(coloredProgram);

                i32 colorLocation = glGetUniformLocation(coloredProgram, "color");
                i32 modelLocation = glGetUniformLocation(coloredProgram, "model");
                i32 viewLocation = glGetUniformLocation(coloredProgram, "view");
                i32 projectionLocation = glGetUniformLocation(coloredProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.e);
                glUniformMatrix4fv(viewLocation, 1, false, view.e);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.e);

                f32 vertices[] = {
                    rectangle->position.x, rectangle->position.y, 0.0f,
                    rectangle->position.x + rectangle->size.x, rectangle->position.y, 0.0f,
                    rectangle->position.x, rectangle->position.y + rectangle->size.y, 0.0f,
                    rectangle->position.x + rectangle->size.x, rectangle->position.y + rectangle->size.y, 0.0f
                };

                u32 indices[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                size = sizeof(RenderRectangle);
                break;
            }
            case type_RenderCircle: {
                RenderCircle *circle = (RenderCircle *)renderHeader;

                glUseProgram(coloredProgram);

                i32 colorLocation = glGetUniformLocation(coloredProgram, "color");
                i32 modelLocation = glGetUniformLocation(coloredProgram, "model");
                i32 viewLocation = glGetUniformLocation(coloredProgram, "view");
                i32 projectionLocation = glGetUniformLocation(coloredProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.e);
                glUniformMatrix4fv(viewLocation, 1, false, view.e);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.e);

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

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glDrawElements(GL_TRIANGLES, (circle->segments + 1) * 3, GL_UNSIGNED_INT, 0);

                size = sizeof(RenderRectangle);
                break;
            }
            case type_RenderImage: {
                RenderImage *image = (RenderImage *)renderHeader;

                glUseProgram(texturedProgram);

                i32 colorLocation = glGetUniformLocation(texturedProgram, "color");
                i32 modelLocation = glGetUniformLocation(texturedProgram, "model");
                i32 viewLocation = glGetUniformLocation(texturedProgram, "view");
                i32 projectionLocation = glGetUniformLocation(texturedProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.e);
                glUniformMatrix4fv(viewLocation, 1, false, view.e);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.e);

                GLTexture texture = GL_LoadTexture(image->filename);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                if((image->header.renderFlags & IMAGE_ADAPTATIVE_FIT) > 0) {
                    // #TODO(Juan): Check this and fix errors
                    if(texture.height > texture.width) {
                        f32 oldScaleX = image->size.x;
                        image->size.x *= (f32)texture.width / (f32)texture.height;
                        image->position.x += (oldScaleX - image->size.x) * 0.5f;
                    } else {
                        f32 oldScaleY = image->size.y;
                        image->size.y *= (f32)texture.width / (f32)texture.height;
                        image->position.y += (oldScaleY - image->size.y) * 0.5f;
                    }
                } else if((image->header.renderFlags & IMAGE_KEEP_RATIO_X) > 0) {
                    f32 quadRatio = (f32)image->size.y / (f32)image->size.x;
                    f32 textureRatio = (f32)texture.height / (f32)texture.width;
                    f32 oldScaleY = image->size.y;
                    image->size.y *= textureRatio / quadRatio;
                    image->position.y += (oldScaleY - image->size.y) * 0.5f;
                } else if((image->header.renderFlags & IMAGE_KEEP_RATIO_Y) > 0) {
                    f32 quadRatio = (f32)image->size.x / (f32)image->size.y;
                    f32 textureRatio = (f32)texture.width / (f32)texture.height;
                    f32 oldScaleX = image->size.x;
                    image->size.x *= textureRatio / quadRatio;
                    image->position.x += (oldScaleX - image->size.x) * 0.5f;
                }

                f32 vertices[] = {
                    image->position.x, image->position.y, 0.0f, 0.0f, 0.0f,
                    image->position.x + image->size.x, image->position.y, 0.0f, 1.0f, 0.0f,
                    image->position.x, image->position.y + image->size.y, 0.0f, 0.0f, 1.0f,
                    image->position.x + image->size.x, image->position.y + image->size.y, 0.0f, 1.0f, 1.0f
                };

                u32 indices[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderImage) + image->filenameSize;
                break;
            }
            case type_RenderImageUV: {
                RenderImageUV *imageUV = (RenderImageUV *)renderHeader;

                glUseProgram(texturedProgram);

                i32 colorLocation = glGetUniformLocation(texturedProgram, "color");
                i32 modelLocation = glGetUniformLocation(texturedProgram, "model");
                i32 viewLocation = glGetUniformLocation(texturedProgram, "view");
                i32 projectionLocation = glGetUniformLocation(texturedProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.e);
                glUniformMatrix4fv(viewLocation, 1, false, view.e);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.e);

                GLTexture texture = GL_LoadTexture(imageUV->filename);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                f32 vertices[] = {
                    imageUV->position.x, imageUV->position.y, 0.0f, imageUV->uv.min.x, imageUV->uv.min.y,
                    imageUV->position.x + imageUV->size.x, imageUV->position.y, 0.0f, imageUV->uv.max.x, imageUV->uv.min.y,
                    imageUV->position.x, imageUV->position.y + imageUV->size.y, 0.0f, imageUV->uv.min.x, imageUV->uv.max.y,
                    imageUV->position.x + imageUV->size.x, imageUV->position.y + imageUV->size.y, 0.0f, imageUV->uv.max.x, imageUV->uv.max.y
                };

                u32 indices[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderImageUV) + imageUV->filenameSize;
                break;
            }
            case type_RenderFont: {
                RenderFont *font = (RenderFont *)renderHeader;
                currentFont = GL_LoadFont(font->filename, font->fontSize, font->width, font->height);

                size = sizeof(RenderFont) + font->filenameSize;
                break;
            }
            case type_RenderChar: {                
                RenderChar *renderChar = (RenderChar *)renderHeader;

                v2 offset = V2(0, 0);
                stbtt_bakedchar *charData = CalculateCharacterOffset(&currentFont, renderChar->singleChar, &offset, currentFont.fontSize);
                
                rectangle2 charRect = RectMinMax(V2(charData->x0, charData->y0), V2(charData->x1, charData->y1));
                charRect.min.x /= currentFont.width;
                charRect.min.y /= currentFont.height;
                charRect.max.x /= currentFont.width;
                charRect.max.y /= currentFont.height;

                glUseProgram(texturedProgram);

                i32 colorLocation = glGetUniformLocation(texturedProgram, "color");
                i32 modelLocation = glGetUniformLocation(texturedProgram, "model");
                i32 viewLocation = glGetUniformLocation(texturedProgram, "view");
                i32 projectionLocation = glGetUniformLocation(texturedProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.e);
                glUniformMatrix4fv(viewLocation, 1, false, view.e);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.e);

                GLTexture texture = GL_LoadTexture(currentFont.fontFilename);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                f32 charWidth = (charRect.max.x - charRect.min.x);
                f32 charHeight = (charRect.max.y - charRect.min.y);
                
                f32 quadRatio = (f32)renderChar->size.x / (f32)renderChar->size.y;
                f32 textureRatio = (f32)charWidth / (f32)charHeight;

                f32 oldScaleX = renderChar->size.x;
                renderChar->size.x *= textureRatio / quadRatio;
                renderChar->position.x += (oldScaleX - renderChar->size.x) * 0.5f;

                renderChar->position.x += (f32)charData->xoff / (f32)currentFont.width;
                // renderChar->position.y += (f32)charData->yoff / (f32)currentFont.height;

                f32 vertices[] = {
                    renderChar->position.x, renderChar->position.y, 0.0f, charRect.min.x, charRect.min.y,
                    renderChar->position.x + renderChar->size.x, renderChar->position.y, 0.0f, charRect.max.x, charRect.min.y,
                    renderChar->position.x, renderChar->position.y + renderChar->size.y, 0.0f, charRect.min.x, charRect.max.y,
                    renderChar->position.x + renderChar->size.x, renderChar->position.y + renderChar->size.y, 0.0f, charRect.max.x, charRect.max.y
                };

                u32 indices[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                glEnableVertexAttribArray(1);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                size = sizeof(RenderChar);
                break;
            }
            case type_RenderText: {
                RenderText *text = (RenderText *)renderHeader;

                glUseProgram(texturedProgram);

                i32 colorLocation = glGetUniformLocation(texturedProgram, "color");
                i32 modelLocation = glGetUniformLocation(texturedProgram, "model");
                i32 viewLocation = glGetUniformLocation(texturedProgram, "view");
                i32 projectionLocation = glGetUniformLocation(texturedProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.e);
                glUniformMatrix4fv(viewLocation, 1, false, view.e);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.e);

                GLTexture texture = GL_LoadTexture(currentFont.fontFilename);
                glBindTexture(GL_TEXTURE_2D, texture.textureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                v2 offset = V2(0, 0);

                v2 textSize = text->size;
                v2 textPosition = text->position;
                v2 fontDividers = V2(currentFont.fontSize / currentFont.width, currentFont.fontSize / currentFont.height);

                for(int i = 0; i < text->stringSize - 1; ++i) {
                    char currentChar = text->string[i];

                    stbtt_bakedchar *charData = CalculateCharacterOffset(&currentFont, currentChar, &offset, text->size.y * 2.0f);
                    
                    if(charData != 0) {
                        rectangle2 charRect = RectMinMax(V2(charData->x0, charData->y0), V2(charData->x1, charData->y1));
                        charRect.min.x /= currentFont.width;
                        charRect.min.y /= currentFont.height;
                        charRect.max.x /= currentFont.width;
                        charRect.max.y /= currentFont.height;

                        f32 charWidth = (charRect.max.x - charRect.min.x);
                        f32 charHeight = (charRect.max.y - charRect.min.y);

                        text->size.x = textSize.x * (charWidth / fontDividers.x);
                        text->size.y = textSize.y * (charHeight / fontDividers.y);

                        text->position.x = textPosition.x + (f32)charData->xoff / currentFont.width + offset.x;
                        text->position.y = textPosition.y + (f32)charData->yoff / currentFont.height + offset.y;

                        f32 vertices[] = {
                            text->position.x, text->position.y, 0.0f, charRect.min.x, charRect.min.y,
                            text->position.x + text->size.x, text->position.y, 0.0f, charRect.max.x, charRect.min.y,
                            text->position.x, text->position.y + text->size.y, 0.0f, charRect.min.x, charRect.max.y,
                            text->position.x + text->size.x, text->position.y + text->size.y, 0.0f, charRect.max.x, charRect.max.y
                        };

                        offset.x += charData->xadvance / currentFont.width;

                        u32 indices[] = {
                            0, 1, 2,
                            1, 2, 3
                        };

                        glBindVertexArray(glVertexArray);

                        glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
                        glEnableVertexAttribArray(1);

                        glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    }
                }
                
                size = sizeof(RenderText) + text->stringSize;
                break;
            }
            default: {
                InvalidCodePath;
                break;
            }
        }

        Assert(size > 0)
        renderHeader = (RenderHeader *)((u8 *)renderHeader + size);
    }
}

static void GLEnd()
{
    glDeleteBuffers(1, &glVertexBuffer);
    glDeleteBuffers(1, &glIndexBuffer);
}

#endif