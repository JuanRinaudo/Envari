#ifndef SCRIPTINGWRAPPERS_H
#define SCRIPTINGWRAPPERS_H

#ifndef __EMSCRIPTEN__
#include <filesystem>

#include "GL3W/gl3w.h"

#include "STB/stb_truetype.h" 

#include "Defines.h"
#include "LUA/sol.hpp"
#include "Miniaudio/miniaudio.h"

#include "MemoryStructs.h"
#include "MathStructs.h"
#include "GameStructs.h"

#ifdef GAME_EDITOR
#include <psapi.h>
#include "IMGUI/imgui.h"
#include "EditorStructs.h"
extern ConsoleWindow editorConsole;
#endif

extern sol::state lua;

#ifdef GAME_EDITOR
extern void Log_(ConsoleWindow* console, ConsoleLogType type, const char* file, u32 line, const char* fmt, ...);
#define Log(fmt, ...) Log_(&editorConsole, ConsoleLogType_NORMAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LogError(fmt, ...) Log_(&editorConsole, ConsoleLogType_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LogCommand(fmt, ...) Log_(&editorConsole, ConsoleLogType_COMMAND, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
extern void Log_(ConsoleLogType type, const char* fmt, ...);
#define Log(fmt, ...) Log_(ConsoleLogType_NORMAL, fmt, ##__VA_ARGS__)
#define LogError(fmt, ...) Log_(ConsoleLogType_ERROR, fmt, ##__VA_ARGS__)
#define LogCommand(fmt, ...) Log_(ConsoleLogType_COMMAND, fmt, ##__VA_ARGS__)
#endif

#define LUASaveGetSet(POSTFIX, valueType) static valueType SaveGet##POSTFIX##(const char* key, valueType defaultValue) \
{ \
    return TableGet##POSTFIX##(&saveData, key, defaultValue); \
} \
static void SaveSet##POSTFIX##(const char* key, valueType value) \
{ \
    TableSet##POSTFIX##_(&permanentState->arena, &saveData, key, value); \
}

#define PushStruct(arena, type) (type *)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type *)PushSize_(arena, ((count)*sizeof(type)))
#define PushSize(arena, size) PushSize_(arena, size)
extern void *PushSize_(MemoryArena *arena, size_t size);
extern void *PushSize_(TemporaryMemory *memory, size_t size);
extern char *PushString(MemoryArena *arena, const char *string);

// #NOTE(Juan): Engine
extern Data *gameState;
extern PermanentData *permanentState;
extern SceneData *sceneState;
extern TemporalData *temporalState;
extern TemporaryMemory renderTemporaryMemory;

extern SerializableTable* configSave;
extern SerializableTable* saveData;

extern void LoadScriptFile(char* filePath);
extern void LoadLUALibrary(sol::lib library);

// #NOTE (Juan): Input
extern void SetCursorTexture(GLTexture texture); 

// #NOTE (Juan): Bindings
extern v2 V2(f32 x, f32 y);

extern bool MouseOverRectangle(rectangle2 rectangle);
extern bool ClickOverRectangle(rectangle2 rectangle, i32 button);
extern bool ClickedOverRectangle(rectangle2 rectangle, i32 button);

extern m44 PerspectiveProjection(f32 fovY, f32 aspect, f32 nearPlane, f32 farPlane);
extern m44 OrtographicProjection(f32 left, f32 right, f32 top, f32 bottom, f32 nearPlane, f32 farPlane);
extern m44 OrtographicProjection(f32 size, f32 aspect, f32 nearPlane, f32 farPlane);

extern void Begin2D(u32 frameBufferID, u32 width, u32 height);
extern void DrawClear(f32 red, f32 green, f32 blue, f32 alpha);
extern void DrawColor(f32 red, f32 green, f32 blue, f32 alpha);
extern void DrawTransparent();
extern void DrawTransparent(u32 modeRGB, u32 modeAlpha, u32 srcRGB, u32 dstRGB, u32 srcAlpha, u32 dstAlpha);
extern void DrawTransparentDisable();
extern void DrawSetLayer(u32 targetLayer, bool transparent);
extern void DrawLineWidth(f32 width);
extern void DrawLine(f32 startX, f32 startY, f32 endX, f32 endY);
extern void DrawTriangle(f32 p1X, f32 p1Y, f32 p2X, f32 p2Y, f32 p3X, f32 p3Y);
extern void DrawRectangle(f32 posX, f32 posY, f32 scaleX, f32 scaleY);
extern void DrawCircle(f32 posX, f32 posY, f32 radius, i32 segments);
extern void DrawTextureParameters(u32 wrapS, u32 wrapT, u32 minFilter, u32 magFilter);
extern void DrawTexture(f32 posX, f32 posY, f32 scaleX, f32 scaleY, u32 textureID);
extern void DrawImage(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char* filepath, u32 renderFlags);
extern void DrawImageUV(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 uvX, f32 uvY, f32 uvEndX, f32 uvEndY, const char* filepath);
extern void DrawImage9Slice(f32 posX, f32 posY, f32 endX, f32 endY, f32 slice, const char* filepath);
extern void DrawAtlasSprite(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char* filepath, const char* atlasName, const char* key);
extern void DrawSetFont(i32 fontID);
extern void DrawChar(f32 posX, f32 posY, f32 scaleX, f32 scaleY, const char singleChar);
extern void DrawString(f32 posX, f32 posY, const char* string, u32 renderFlags);
extern void DrawStyledString(f32 posX, f32 posY, f32 endX, f32 endY, const char* string, u32 renderFlags);
extern void ClearInputBuffer();
extern bool DrawStringInput(f32 posX, f32 posY, f32 endX, f32 endY, const char* baseText, i32 maxSize);
extern bool DrawButton(f32 posX, f32 posY, f32 endX, f32 endY, f32 slice, const char* string, const char* buttonUp, const char* buttonDown);
extern i32 DrawMultibutton(f32 posX, f32 posY, f32 endX, f32 height, f32 slice, f32 yPadding, const char* options, const char* buttonUp, const char* buttonDown);
extern void DrawSetUniform(u32 locationID, UniformType type);
extern void DrawOverrideProgram(u32 programID);
extern void DrawOverrideVertices(f32* vertices, u32 count);
extern void DrawOverrideIndices(u32* indices, u32 count);
extern void End2D();
extern v2 RenderToViewport(f32 screenX, f32 screenY, f32 size, f32 ratio);

GenerateTableGetExtern(String, char*, "")
GenerateTableSetExtern(String, const char*, SerializableType_STRING)
GenerateTableGetExtern(Bool, bool, false)
GenerateTableSetExtern(Bool, bool, SerializableType_BOOL)
GenerateTableGetExtern(I32, i32, 0)
GenerateTableSetExtern(I32, i32, SerializableType_I32)
GenerateTableGetExtern(F32, f32, 0)
GenerateTableSetExtern(F32, f32, SerializableType_F32)
GenerateTableGetExtern(V2, v2, V2(0, 0))
GenerateTableSetExtern(V2, v2, SerializableType_V2)

extern void LoadLUAScene(const char* luaFilepath);
extern void UnloadLUAScene();
extern void ClearLUAState();

extern u32 defaultFontID;

extern f32* CreateQuadPosUV(f32 posStartX, f32 posStartY, f32 posEndX, f32 posEndY, f32 uvStartX, f32 uvStartY, f32 uvEndX, f32 uvEndY);
extern void GL_LoadTextureID(u32 textureID, f32 width, f32 height);
extern GLTexture GL_LoadTextureFile(const char *texturePath);
extern u32 GL_GenerateFont(const char *filepath, f32 fontSize, u32 width, u32 height);
extern u32 GL_CompileProgram(const char *vertexShaderSource, const char *fragmentShaderSource);
extern u32 GL_CompileProgramPlatform(const char *vertexShaderPlatform, const char *fragmentShaderPlatform);

extern ma_decoder* SoundLoad(const char* soundKey);
extern void SoundPlay(const char* filepath, f32 volume, bool loop);
extern void SoundStop(const char* filepath);
extern void SetMasterVolume(float value);
extern float dbToVolume(float db);
extern float volumeToDB(float volume);

extern v2 V2(f32 x, f32 y);
extern v3 V3(f32 x, f32 y, f32 z);
extern v4 V4(f32 x, f32 y, f32 z, f32 w);

extern rectangle2 Rectangle2(f32 x, f32 y, f32 width, f32 height);

extern m22 M22(
    f32 _00, f32 _01,
    f32 _10, f32 _11);
extern m22 IdM22();

extern m33 M33(
    f32 _00, f32 _01, f32 _02,
    f32 _10, f32 _11, f32 _12,
    f32 _20, f32 _21, f32 _22);
extern m33 IdM33();

extern m44 M44(
    f32 _00, f32 _01, f32 _02, f32 _03,
    f32 _10, f32 _11, f32 _12, f32 _13,
    f32 _20, f32 _21, f32 _22, f32 _23,
    f32 _30, f32 _31, f32 _32, f32 _33);
extern m44 IdM44();

extern transform2D Transform2D(f32 posX, f32 posY, f32 scaleX, f32 scaleY);

extern f32 Length(v2 a);
#endif

// #NOTE(Juan): Console
static void LogConsole(const char* log)
{
	Log(log);
}

static void LogConsoleError(const char* log)
{
	LogError(log);
}

static void LogConsoleCommand(const char* log)
{
	LogCommand(log);
}

// #NOTE(Juan): Cast
static i32 CharToInt(const char* singleChar)
{
    return (i32)singleChar[0];
}

char castChar[1];
static char* IntToChar(i32 value)
{
    castChar[0] = (char)value;
    return castChar;
}

static void DrawDisableOverrideVertices()
{
    DrawOverrideVertices(0, 0);
}

static void DrawDisableOverrideIndices()
{    
    DrawOverrideIndices(0, 0);
}

static void DrawDisableOverrideProgram()
{    
    DrawOverrideProgram(0);
}

// #NOTE(Juan): GLRender
GLTexture LoadTexture(const char* filePath)
{
    GLTexture texture = GL_LoadTextureFile(filePath);
    return texture;
}

// #NOTE(Juan): Render
static i32 DrawMultibuttonLUA(f32 posX, f32 posY, f32 endX, f32 height, f32 slice, f32 yPadding, const char* options, const char* buttonUp, const char* buttonDown)
{
    return DrawMultibutton(posX, posY, endX, height, slice, yPadding, options, buttonUp, buttonDown) + 1;
}

// #NOTE(Juan): Save
// #TODO (Juan): Check strdup and see how to fix this
static char* SaveGetString(const char* key, const char* defaultValue)
{
    char* temporalString = PushString(&temporalState->arena, defaultValue);
    return TableGetString(&saveData, key, temporalString);
}
static void SaveSetString(const char* key, const char* value)
{
    char* permanentString = PushString(&permanentState->arena, value);
    TableSetString_(&permanentState->arena, &saveData, key, permanentString);
}

// #NOTE(Juan): Sound
static void SoundPlaySimple(const char* filepath, f32 volume, bool loop) {
    SoundPlay(filepath, volume, false);
}

LUASaveGetSet(Bool, bool)
LUASaveGetSet(I32, i32)
LUASaveGetSet(F32, f32)
LUASaveGetSet(V2, v2)

GenerateRenderTemporaryPush(Float, f32);
GenerateRenderTemporaryPush(Vector2, v2);

void ScriptingInitBindings()
{
    // #NOTE (Juan): Lua
    lua["LoadScriptFile"] = LoadScriptFile;
    lua["LoadLibrary"] = LoadLUALibrary;
    lua["SOL_LIBRARY_BASE"] = sol::lib::base;
    lua["SOL_LIBRARY_PACKAGE"] = sol::lib::package;
    lua["SOL_LIBRARY_COROUTINE"] = sol::lib::coroutine;
    lua["SOL_LIBRARY_STRING"] = sol::lib::string;
    lua["SOL_LIBRARY_OS"] = sol::lib::os;
    lua["SOL_LIBRARY_MATH"] = sol::lib::math;
    lua["SOL_LIBRARY_TABLE"] = sol::lib::table;
    lua["SOL_LIBRARY_DEBUG"] = sol::lib::debug;
    lua["SOL_LIBRARY_BIT32"] = sol::lib::bit32;
    lua["SOL_LIBRARY_IO"] = sol::lib::io;
    lua["SOL_LIBRARY_BIT32"] = sol::lib::bit32;
    lua["SOL_LIBRARY_FFI"] = sol::lib::ffi;
    lua["SOL_LIBRARY_JIT"] = sol::lib::jit;
    lua["SOL_LIBRARY_UTF8"] = sol::lib::utf8;
    
    // #NOTE (Juan): C/C++
    lua["CharToInt"] = CharToInt;
    lua["IntToChar"] = IntToChar;

    // #NOTE (Juan): Data
    sol::usertype<Game> game_usertype = lua.new_usertype<Game>("game");
    game_usertype["running"] = &Game::running;
    game_usertype["updateRunning"] = &Game::updateRunning;
    game_usertype["version"] = &Game::version;
    lua["game"] = &gameState->game;

    sol::usertype<Camera> camera_usertype = lua.new_usertype<Camera>("camera");
    camera_usertype["size"] = &Camera::size;
    camera_usertype["ratio"] = &Camera::ratio;
    camera_usertype["nearPlane"] = &Camera::nearPlane;
    camera_usertype["farPlane"] = &Camera::farPlane;
    camera_usertype["view"] = &Camera::view;
    camera_usertype["projection"] = &Camera::projection;
    lua["camera"] = &gameState->camera;

    sol::usertype<Render> screen_usertype = lua.new_usertype<Render>("screen");
    screen_usertype["refreshRate"] = &Render::refreshRate;
    screen_usertype["size"] = &Render::size;
    screen_usertype["bufferSize"] = &Render::bufferSize;
    screen_usertype["windowSize"] = &Render::windowSize;
    screen_usertype["defaultFontID"] = &Render::defaultFontID;
    lua["screen"] = &gameState->render;

    sol::usertype<Time> time_usertype = lua.new_usertype<Time>("time");
    time_usertype["startTime"] = &Time::startTime;
    time_usertype["gameTime"] = &Time::gameTime;
    time_usertype["deltaTime"] = &Time::deltaTime;
    time_usertype["lastFrameGameTime"] = &Time::lastFrameGameTime;
    time_usertype["gameFrames"] = &Time::gameFrames;
    time_usertype["frames"] = &Time::frames;
    lua["time"] = &gameState->time;
        
    sol::usertype<Input> input_usertype = lua.new_usertype<Input>("input");
    input_usertype["mouseTextureID"] = &Input::mouseTextureID;
    input_usertype["mousePosition"] = &Input::mousePosition;
    input_usertype["mouseScreenPosition"] = &Input::mouseScreenPosition;
    input_usertype["mouseWheel"] = &Input::mouseWheel;
    input_usertype["textInputBuffer"] = sol::property([](Input &input) { return input.textInputBuffer; });
    input_usertype["keyState"] = sol::property([](Input &input) { return &input.keyState; });
    input_usertype["mouseState"] = sol::property([](Input &input) { return &input.mouseState; });
    lua["input"] = &gameState->input;

    lua["SetCursorTexture"] = SetCursorTexture;

    lua["KEY_COUNT"] = KEY_COUNT;
    lua["MOUSE_COUNT"] = MOUSE_COUNT;
    lua["TEXT_INPUT_BUFFER_COUNT"] = TEXT_INPUT_BUFFER_COUNT;
    
    lua["KEY_UP"] = KEY_UP;
    lua["KEY_RELEASED"] = KEY_RELEASED;
    lua["KEY_PRESSED"] = KEY_PRESSED;
    lua["KEY_DOWN"] = KEY_DOWN;

    // #NOTE (Juan): Temporal memory
    lua["RenderTemporaryPushFloat"] = RenderTemporaryPushFloat;
    lua["RenderTemporaryPushVector2"] = RenderTemporaryPushVector2;

    // #NOTE (Juan): Input
    lua["MouseOverRectangle"] = MouseOverRectangle;
    lua["ClickOverRectangle"] = ClickOverRectangle;
    lua["ClickedOverRectangle"] = ClickedOverRectangle;

    // #NOTE (Juan): Render
    lua["DrawClear"] = DrawClear;
    lua["DrawColor"] = DrawColor;
    lua["DrawSetLayer"] = DrawSetLayer;
    lua["DrawLineWidth"] = DrawLineWidth;
    lua["DrawLine"] = DrawLine;
    lua["DrawTriangle"] = DrawTriangle;
    lua["DrawRectangle"] = DrawRectangle;
    lua["DrawCircle"] = DrawCircle;
    lua["DrawTextureParameters"] = DrawTextureParameters;
    lua["DrawTexture"] = DrawTexture;
    lua["DrawImage"] = DrawImage;
    lua["DrawImageUV"] = DrawImageUV;
    lua["DrawImage9Slice"] = DrawImage9Slice;
    lua["DrawAtlasSprite"] = DrawAtlasSprite;
    lua["DrawDefaultTransparent"] = sol::resolve<void()>(DrawTransparent);
    lua["DrawTransparent"] = sol::resolve<void(u32, u32, u32, u32, u32, u32)>(DrawTransparent);
    lua["DrawTransparentDisable"] = DrawTransparentDisable;
    lua["DrawSetFont"] = DrawSetFont;
    lua["DrawChar"] = DrawChar;
    lua["DrawString"] = DrawString;
    lua["DrawStyledString"] = DrawStyledString;
    lua["ClearInputBuffer"] = ClearInputBuffer;
    lua["DrawStringInput"] = DrawStringInput;
    lua["DrawButton"] = DrawButton;
    lua["DrawMultibutton"] = DrawMultibuttonLUA;
    lua["DrawSetUniform"] = DrawSetUniform;
    lua["DrawOverrideProgram"] = DrawOverrideProgram;
    lua["DrawDisableOverrideProgram"] = DrawDisableOverrideProgram;
    lua["DrawOverrideVertices"] = DrawOverrideVertices;
    lua["DrawDisableOverrideVertices"] = DrawDisableOverrideVertices;
    lua["DrawOverrideIndices"] = DrawOverrideIndices;
    lua["DrawDisableOverrideIndices"] = DrawDisableOverrideIndices;
    
    lua["LoadLUAScene"] = LoadLUAScene;
    lua["UnloadLUAScene"] = UnloadLUAScene;
    lua["ClearLUAState"] = ClearLUAState;

    lua["RenderToViewport"] = RenderToViewport;
    
    lua["ImageRenderFlag_Fit"] = ImageRenderFlag_Fit;
    lua["ImageRenderFlag_KeepRatioX"] = ImageRenderFlag_KeepRatioX;
    lua["ImageRenderFlag_KeepRatioY"] = ImageRenderFlag_KeepRatioY;

    lua["TextRenderFlag_Center"] = TextRenderFlag_Center;
    lua["TextRenderFlag_LetterWrap"] = TextRenderFlag_LetterWrap;
    lua["TextRenderFlag_WordWrap"] = TextRenderFlag_WordWrap;

    lua["PerspectiveProjection"] = PerspectiveProjection;
    lua["OrtographicProjection"] = sol::resolve<m44(f32, f32, f32, f32)>(OrtographicProjection);

    // #NOTE (Juan): GLRender    
    sol::usertype<GLTexture> gltexture_usertype = lua.new_usertype<GLTexture>("gltexture");
    gltexture_usertype["textureID"] = &GLTexture::textureID;
    gltexture_usertype["width"] = &GLTexture::width;
    gltexture_usertype["height"] = &GLTexture::height;
    gltexture_usertype["channels"] = &GLTexture::channels;

    lua["CreateQuadPosUV"] = CreateQuadPosUV;
    lua["LoadTextureID"] = GL_LoadTextureID;
    lua["LoadTexture"] = LoadTexture;
    lua["GenerateFont"] = GL_GenerateFont;
    lua["CompileProgram"] = GL_CompileProgram;
    lua["CompileProgramPlatform"] = GL_CompileProgramPlatform;
    lua["GetUniformLocation"] = glGetUniformLocation;
    lua["UniformType_Float"] = UniformType_Float;
    lua["UniformType_Vector2"] = UniformType_Vector2;

    // #NOTE (Juan): OpenGL
    lua["GL_ZERO"] = GL_ZERO;
    lua["GL_ONE"] = GL_ONE;

    lua["GL_FUNC_ADD"] = GL_FUNC_ADD;
    lua["GL_FUNC_SUBTRACT"] = GL_FUNC_SUBTRACT;
    lua["GL_FUNC_REVERSE_SUBTRACT"] = GL_FUNC_REVERSE_SUBTRACT;
    lua["GL_MIN"] = GL_MIN;
    lua["GL_MAX"] = GL_MAX;

    lua["GL_SRC_COLOR"] = GL_SRC_COLOR;
    lua["GL_ONE_MINUS_SRC_COLOR"] = GL_ONE_MINUS_SRC_COLOR;
    lua["GL_SRC_ALPHA"] = GL_SRC_ALPHA;
    lua["GL_ONE_MINUS_SRC_ALPHA"] = GL_ONE_MINUS_SRC_ALPHA;
    lua["GL_DST_ALPHA"] = GL_DST_ALPHA;
    lua["GL_ONE_MINUS_DST_ALPHA"] = GL_ONE_MINUS_DST_ALPHA;
    lua["GL_DST_COLOR"] = GL_DST_COLOR;
    lua["GL_ONE_MINUS_DST_COLOR"] = GL_ONE_MINUS_DST_COLOR;

    lua["GL_CONSTANT_COLOR"] = GL_CONSTANT_COLOR;
    lua["GL_ONE_MINUS_CONSTANT_COLOR"] = GL_ONE_MINUS_CONSTANT_COLOR;
    lua["GL_CONSTANT_ALPHA"] = GL_CONSTANT_ALPHA;
    lua["GL_ONE_MINUS_CONSTANT_ALPHA"] = GL_ONE_MINUS_CONSTANT_ALPHA;
    lua["GL_SRC_ALPHA_SATURATE"] = GL_SRC_ALPHA_SATURATE;
    
    lua["GL_TEXTURE_2D"] = GL_TEXTURE_2D;
    lua["GL_TEXTURE_2D_ARRAY"] = GL_TEXTURE_2D_ARRAY;
    lua["GL_TEXTURE_3D"] = GL_TEXTURE_3D;

    lua["GL_TEXTURE_WRAP_S"] = GL_TEXTURE_WRAP_S;
    lua["GL_TEXTURE_WRAP_T"] = GL_TEXTURE_WRAP_T;
    lua["GL_TEXTURE_WRAP_R"] = GL_TEXTURE_WRAP_R;
    lua["GL_CLAMP_TO_EDGE"] = GL_CLAMP_TO_EDGE;;
    lua["GL_MIRRORED_REPEAT"] = GL_MIRRORED_REPEAT;
    lua["GL_REPEAT"] = GL_REPEAT;
    
    lua["GL_TEXTURE_MIN_FILTER"] = GL_TEXTURE_MIN_FILTER;
    lua["GL_TEXTURE_MAG_FILTER"] = GL_TEXTURE_MAG_FILTER;
    lua["GL_NEAREST"] = GL_NEAREST;
    lua["GL_LINEAR"] = GL_LINEAR;
    lua["GL_NEAREST_MIPMAP_NEAREST"] = GL_NEAREST_MIPMAP_NEAREST;
    lua["GL_LINEAR_MIPMAP_NEAREST"] = GL_LINEAR_MIPMAP_NEAREST;
    lua["GL_NEAREST_MIPMAP_LINEAR"] = GL_NEAREST_MIPMAP_LINEAR;
    lua["GL_LINEAR_MIPMAP_LINEAR"] = GL_LINEAR_MIPMAP_LINEAR;

    lua["GL_TEXTURE_BASE_LEVEL"] = GL_TEXTURE_BASE_LEVEL;
    lua["GL_TEXTURE_COMPARE_FUNC"] = GL_TEXTURE_COMPARE_FUNC;
    lua["GL_TEXTURE_COMPARE_MODE"] = GL_TEXTURE_COMPARE_MODE;
    lua["GL_TEXTURE_MIN_LOD"] = GL_TEXTURE_MIN_LOD;
    lua["GL_TEXTURE_MAX_LOD"] = GL_TEXTURE_MAX_LOD;
    lua["GL_TEXTURE_MAX_LEVEL"] = GL_TEXTURE_MAX_LEVEL;
    lua["GL_TEXTURE_SWIZZLE_R"] = GL_TEXTURE_SWIZZLE_R;
    lua["GL_TEXTURE_SWIZZLE_G"] = GL_TEXTURE_SWIZZLE_G;
    lua["GL_TEXTURE_SWIZZLE_B"] = GL_TEXTURE_SWIZZLE_B;
    lua["GL_TEXTURE_SWIZZLE_A"] = GL_TEXTURE_SWIZZLE_A;

#ifndef GL_PROFILE_GLES3    
    lua["GL_SRC1_COLOR"] = GL_SRC1_COLOR;
    lua["GL_ONE_MINUS_SRC1_COLOR"] = GL_ONE_MINUS_SRC1_COLOR;
    lua["GL_SRC1_ALPHA"] = GL_SRC1_ALPHA;
    lua["GL_ONE_MINUS_SRC1_ALPHA"] = GL_ONE_MINUS_SRC1_ALPHA;

    lua["GL_TEXTURE_1D"] = GL_TEXTURE_1D;
    lua["GL_TEXTURE_1D_ARRAY"] = GL_TEXTURE_1D_ARRAY;

    lua["GL_TEXTURE_2D_MULTISAMPLE"] = GL_TEXTURE_2D_MULTISAMPLE;
    lua["GL_TEXTURE_2D_MULTISAMPLE_ARRAY"] = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    lua["GL_TEXTURE_CUBE_MAP"] = GL_TEXTURE_CUBE_MAP;
    lua["GL_TEXTURE_CUBE_MAP_ARRAY"] = GL_TEXTURE_CUBE_MAP_ARRAY;
    lua["GL_TEXTURE_RECTANGLE"] = GL_TEXTURE_RECTANGLE;

    lua["GL_CLAMP_TO_BORDER"] = GL_CLAMP_TO_BORDER;
    lua["GL_MIRROR_CLAMP_TO_EDGE"] = GL_MIRROR_CLAMP_TO_EDGE;
    
    lua["GL_DEPTH_STENCIL_TEXTURE_MODE"] = GL_DEPTH_STENCIL_TEXTURE_MODE;
    lua["GL_TEXTURE_LOD_BIAS"] = GL_TEXTURE_LOD_BIAS;
#endif

    // #NOTE (Juan): Sound
    lua["SoundLoad"] = SoundLoad;
    lua["SoundPlay"] = SoundPlaySimple;
    lua["SoundPlayLoop"] = SoundPlay;
    lua["SoundStop"] = SoundStop;
    lua["SetMasterVolume"] = SetMasterVolume;
    lua["dbToVolume"] = dbToVolume;
    lua["volumeToDB"] = volumeToDB;

    // #NOTE (Juan): Console
    lua["LogConsole"] = LogConsole;
    lua["LogConsoleError"] = LogConsoleError;
    lua["LogConsoleCommand"] = LogConsoleCommand;

    // #NOTE (Juan): Serialization
    lua["SaveGetString"] = SaveGetString;
    lua["SaveSetString"] = SaveSetString;
    lua["SaveGetBool"] = SaveGetBool;
    lua["SaveSetBool"] = SaveSetBool;
    lua["SaveGetI32"] = SaveGetI32;
    lua["SaveSetI32"] = SaveSetI32;
    lua["SaveGetF32"] = SaveGetF32;
    lua["SaveSetF32"] = SaveSetF32;
    lua["SaveGetV2"] = SaveGetV2;
    lua["SaveSetV2"] = SaveSetV2;
}

void ScriptingMathBindings()
{
    // #NOTE (Juan): Math usertypes
    sol::usertype<v2> v2_usertype = lua.new_usertype<v2>("v2");
    v2_usertype["x"] = sol::property([](v2 &v) { return v.x; }, [](v2 &v, f32 f) { v.x = f; });
    v2_usertype["y"] = sol::property([](v2 &v) { return v.y; }, [](v2 &v, f32 f) { v.y = f; });
    v2_usertype["e"] = sol::property([](v2 &v) { return &v.e; });

    // #TODO (Juan): Research if there is a better way to do usertype alias
    sol::usertype<v3> v3_usertype = lua.new_usertype<v3>("v3");
    v3_usertype["x"] = sol::property([](v3 &v) { return v.x; }, [](v3 &v, f32 f) { v.x = f; });
    v3_usertype["y"] = sol::property([](v3 &v) { return v.y; }, [](v3 &v, f32 f) { v.y = f; });
    v3_usertype["z"] = sol::property([](v3 &v) { return v.z; }, [](v3 &v, f32 f) { v.z = f; });
    v3_usertype["e"] = sol::property([](v3 &v) { return &v.e; });

    sol::usertype<v4> v4_usertype = lua.new_usertype<v4>("v4");
    v4_usertype["x"] = sol::property([](v4 &v) { return v.x; }, [](v4 &v, f32 f) { v.x = f; });
    v4_usertype["y"] = sol::property([](v4 &v) { return v.y; }, [](v4 &v, f32 f) { v.y = f; });
    v4_usertype["z"] = sol::property([](v4 &v) { return v.z; }, [](v4 &v, f32 f) { v.z = f; });
    v4_usertype["w"] = sol::property([](v4 &v) { return v.w; }, [](v4 &v, f32 f) { v.w = f; });
    v4_usertype["e"] = sol::property([](v4 &v) { return &v.e; });

    sol::usertype<m44> m44_usertype = lua.new_usertype<m44>("m44");
    m44_usertype["_00"] = sol::property([](m44 &v) { return v._00; }, [](m44 &v, f32 f) { v._00 = f; });
    m44_usertype["_10"] = sol::property([](m44 &v) { return v._10; }, [](m44 &v, f32 f) { v._10 = f; });
    m44_usertype["_20"] = sol::property([](m44 &v) { return v._20; }, [](m44 &v, f32 f) { v._20 = f; });
    m44_usertype["_30"] = sol::property([](m44 &v) { return v._30; }, [](m44 &v, f32 f) { v._30 = f; });
    m44_usertype["_01"] = sol::property([](m44 &v) { return v._01; }, [](m44 &v, f32 f) { v._01 = f; });
    m44_usertype["_11"] = sol::property([](m44 &v) { return v._11; }, [](m44 &v, f32 f) { v._11 = f; });
    m44_usertype["_21"] = sol::property([](m44 &v) { return v._21; }, [](m44 &v, f32 f) { v._21 = f; });
    m44_usertype["_31"] = sol::property([](m44 &v) { return v._31; }, [](m44 &v, f32 f) { v._31 = f; });
    m44_usertype["_02"] = sol::property([](m44 &v) { return v._02; }, [](m44 &v, f32 f) { v._02 = f; });
    m44_usertype["_12"] = sol::property([](m44 &v) { return v._12; }, [](m44 &v, f32 f) { v._12 = f; });
    m44_usertype["_22"] = sol::property([](m44 &v) { return v._22; }, [](m44 &v, f32 f) { v._22 = f; });
    m44_usertype["_32"] = sol::property([](m44 &v) { return v._32; }, [](m44 &v, f32 f) { v._32 = f; });
    m44_usertype["_03"] = sol::property([](m44 &v) { return v._03; }, [](m44 &v, f32 f) { v._03 = f; });
    m44_usertype["_13"] = sol::property([](m44 &v) { return v._13; }, [](m44 &v, f32 f) { v._13 = f; });
    m44_usertype["_23"] = sol::property([](m44 &v) { return v._23; }, [](m44 &v, f32 f) { v._23 = f; });
    m44_usertype["_33"] = sol::property([](m44 &v) { return v._33; }, [](m44 &v, f32 f) { v._33 = f; });
    m44_usertype["e"] = sol::property([](m44 &m) { return &m.e; });

    sol::usertype<transform2D> transform2D_usertype = lua.new_usertype<transform2D>("transform2D");
    transform2D_usertype["position"] = &transform2D::position;
    transform2D_usertype["scale"] = &transform2D::scale;

    // #NOTE (Juan): GameMath
    lua["V2"] = V2;
    lua["V3"] = V3;
    lua["V4"] = V4;

    lua["Rectangle2"] = Rectangle2;
    
    lua["M22"] = M22;
    lua["IdM22"] = IdM22;
    lua["M33"] = M33;
    lua["IdM33"] = IdM33;
    lua["M44"] = M44;
    lua["IdM44"] = IdM44;

    lua["Transform2D"] = Transform2D;

    // lua["LengthV2"] = sol::resolve<f32(v2)>(Length);
    // lua["NormalizeV2"] = sol::resolve<v2(v2)>(Normalize);
    // lua["LengthSqV2"] = sol::resolve<f32(v2)>(LengthSq);

    // #NOTE (Juan): Math custom functionality
    // lua["math"]["sign"] = Sign;
    // lua["math"]["round"] = RoundToInt;
    // lua["math"]["rotLeft"] = RotateLeft;
    // lua["math"]["rotRight"] = RotateRight;
    // lua["math"]["sqr"] = Square;
    // lua["math"]["floorV2"] = sol::resolve<v2(v2)>(Floor);
}

#endif