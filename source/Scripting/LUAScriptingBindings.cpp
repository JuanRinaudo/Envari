#ifndef LUA_SCRIPTING_BINDINGS_CPP
#define LUA_SCRIPTING_BINDINGS_CPP

#if PLATFORM_WASM
    #include <GLES3/gl3.h>
    #define GL_PROFILE_GLES3
#else
    #include <gl3w.h>
#endif

#include <stb_truetype.h>

#include <Defines.h>
#include <Templates.h>
#include <sol.hpp>
#include <miniaudio.h>

#include <SDL.h>

#include <MemoryStructs.h>
#include <MathStructs.h>
#include <GameStructs.h>

#ifdef PLATFORM_EDITOR
#ifdef PLATFORM_WINDOWS
    #include <psapi.h>
#endif
#include <imgui.h>
#include <EditorStructs.h>
extern ConsoleWindow editorConsole;
#endif

extern sol::state lua;
 
#ifdef PLATFORM_EDITOR
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

#include "Externs.h"

#define LUAEditorSaveGetSet(POSTFIX, valueType) static valueType EditorSaveGet ## POSTFIX (const char* key, valueType defaultValue) \
{ \
    return TableGet ## POSTFIX (&editorSave, key, defaultValue); \
} \
static void EditorSaveSet ## POSTFIX (const char* key, valueType value) \
{ \
    TableSet ## POSTFIX ## _(&permanentState->arena, &editorSave, key, value); \
}

#define LUASaveGetSet(POSTFIX, valueType) static valueType SaveGet ## POSTFIX (const char* key, valueType defaultValue) \
{ \
    return TableGet ## POSTFIX (&saveData, key, defaultValue); \
} \
static void SaveSet ## POSTFIX (const char* key, valueType value) \
{ \
    TableSet ## POSTFIX ## _(&permanentState->arena, &saveData, key, value); \
}

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

static void DrawDefaultTransform()
{
    DrawSetTransform(0, 0, 1, 1, 0);
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

static void SetUniform1F(u32 programID, u32 locationID, f32 v0)
{
#if PLATFORM_WASM
    glUseProgram(programID);
    glUniform1f(locationID, v0);
#else
    glProgramUniform1f(programID, locationID, v0);
#endif
}

static void SetUniform2F(u32 programID, u32 locationID, f32 v0, f32 v1)
{
#if PLATFORM_WASM
    glUseProgram(programID);
    glUniform2f(locationID, v0, v1);
#else
    glProgramUniform2f(programID, locationID, v0, v1);
#endif
}

static void SetUniform3F(u32 programID, u32 locationID, f32 v0, f32 v1, f32 v2)
{
#if PLATFORM_WASM
    glUseProgram(programID);
    glUniform3f(locationID, v0, v1, v2);
#else
    glProgramUniform3f(programID, locationID, v0, v1, v2);
#endif
}

static void SetUniform4F(u32 programID, u32 locationID, f32 v0, f32 v1, f32 v2, f32 v3)
{
#if PLATFORM_WASM
    glUseProgram(programID);
    glUniform4f(locationID, v0, v1, v2, v3);
#else
    glProgramUniform4f(programID, locationID, v0, v1, v2, v3);
#endif
}

// #NOTE(Juan): GLRender
static std::tuple<f32, f32> TextureSizeBinding(const char* texturePath)
{
    v2 size = TextureSize(texturePath);
    return std::tuple<f32, f32>(size.x, size.y);
}

// static std::tuple<f32, f32> TextureSizeBinding(u32 textureID)
// {
//     v2 size = TextureSize(textureID);
//     return std::tuple<f32, f32>(size.x, size.y);
// }

static TextureAsset LoadSceneTextureFile(const char *texturePath)
{
    return LoadTextureFile(texturePath, false);
}

static TextureAsset LoadPermanentTexture(const char *texturePath)
{
    return LoadTextureFile(texturePath, true);
}

// #NOTE(Juan): Render
static i32 DrawMultibuttonLUA(f32 posX, f32 posY, f32 endX, f32 height, f32 yPadding, const char* options)
{
    return DrawMultibutton(posX, posY, endX, height, yPadding, options) + 1;
}

#define GenerateRenderTemporaryPush(PREFIX, type) static type* RenderTemporaryPush ## PREFIX (type value) \
{ \
    u32 size = sizeof(type); \
    if(renderTemporaryMemory.arena->used + size < renderTemporaryMemory.arena->size) { \
        type *valuePointer = (type*)PushSize(&renderTemporaryMemory, size); \
        *valuePointer = value; \
        return valuePointer; \
    } \
    else { \
        InvalidCodePath; \
        return 0; \
    } \
}

GenerateRenderTemporaryPush(Float, f32);
GenerateRenderTemporaryPush(Vector2, v2);

// #NOTE(Juan): Save
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

static char* EditorSaveGetString(const char* key, const char* defaultValue)
{
    char* temporalString = PushString(&temporalState->arena, defaultValue);
    return TableGetString(&editorSave, key, temporalString);
}

static void EditorSaveSetString(const char* key, const char* value)
{
    char* permanentString = PushString(&permanentState->arena, value);
    TableSetString_(&permanentState->arena, &editorSave, key, permanentString);
}

LUASaveGetSet(Bool, bool)
LUASaveGetSet(I32, i32)
LUASaveGetSet(F32, f32)
LUASaveGetSet(V2, v2)

LUAEditorSaveGetSet(Bool, bool)
LUAEditorSaveGetSet(I32, i32)
LUAEditorSaveGetSet(F32, f32)
LUAEditorSaveGetSet(V2, v2)

// #NOTE(Juan): Sound
static SoundInstance* SoundPlaySimple(const char* filepath, f32 volume, bool unique) {
    return PlaySound(filepath, volume, false, unique);
}

#ifdef PLATFORM_EDITOR
// #NOTE(Juan): Editor
static std::tuple<bool, bool> ImGuiBegin(const char* name, bool open, u32 flags) {
    bool shouldDraw = ImGui::Begin(name, &open, flags);
    return std::tuple<bool, bool>(open, shouldDraw);
}

static void ImGuiSetNextWindowSize(f32 width, f32 height, u32 cond) {
    ImGui::SetNextWindowSize(ImVec2(width, height), cond);
}

static void ImGuiSetNextWindowSizeConstraints(f32 minX, f32 minY, f32 maxX, f32 maxY) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(minX, minY), ImVec2(maxX, maxY));
}

static void ImGuiDummy(f32 width, f32 height) {
    ImGui::Dummy(ImVec2(width, height));
}

static bool ImGuiButton(const char* label, f32 width, f32 height) {
    return ImGui::Button(label, ImVec2(width, height));
}

static std::tuple<bool, i32> ImGuiInputInt(const char* label, i32 value, i32 step, i32 fastStep, u32 flags) {
    bool changed = ImGui::InputInt(label, &value, step, fastStep, flags);
    return std::tuple<bool, i32>(changed, value);
}

static std::tuple<bool, f32> ImGuiInputFloat(const char* label, f32 value, f32 step, f32 fastStep, u32 flags) {
    bool changed = ImGui::InputFloat(label, &value, step, fastStep, "%.3f", flags);
    return std::tuple<bool, f32>(changed, value);
}

static void ImGuiImage(i32 id, f32 width, f32 height) {
    return ImGui::Image((ImTextureID)id, ImVec2(width, height));
}

static bool ImGuiImageButton(i32 id, f32 width, f32 height) {
    return ImGui::ImageButton((ImTextureID)id, ImVec2(width, height));
}

static void ImGuiPushStyleColor(i32 styleColor, f32 r, f32 g, f32 b, f32 a) {
    ImGui::PushStyleColor(styleColor, ImVec4(r, g, b, a));
}

static std::tuple<bool, bool> ImGuiCheckbox(const char* label, bool value) {
    bool pressed = ImGui::Checkbox(label, &value);
    return std::tuple<bool, bool>(pressed, value);
}

char inputBuffer[CONSOLE_INPUT_BUFFER_COUNT];
static std::tuple<bool, char*> ImGuiInputText(const char* label) {
    bool pressed = ImGui::InputText(label, inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue, 0, 0);
    return std::tuple<bool, char*>(pressed, inputBuffer);
}

static std::tuple<f32, f32> ImGuiGetCursorScreenPos() {
    ImVec2 position = ImGui::GetCursorScreenPos();
    return std::tuple<f32, f32>(position.x, position.y);
}

static void ImGuiAddImage(ImDrawList* drawList, i32 id, f32 x, f32 y, f32 width, f32 height) {
    drawList->AddImage((ImTextureID)id, ImVec2(x, y), ImVec2(x + width, y + height));
}
#endif

void LUAScriptingBindings()
{
    // #NOTE (Juan): Platform
    lua["PLATFORM_EDITOR"] = MACRO_DEFINED(PLATFORM_EDITOR);
    lua["PLATFORM_LINUX"] = MACRO_DEFINED(PLATFORM_LINUX);
    lua["PLATFORM_ANDROID"] = MACRO_DEFINED(PLATFORM_ANDROID);
    lua["PLATFORM_WINDOWS"] = MACRO_DEFINED(PLATFORM_WINDOWS);
    lua["PLATFORM_WASM"] = MACRO_DEFINED(PLATFORM_WASM);

    // #NOTE (Juan): Lua
    lua["LoadLUAScriptFile"] = sol::resolve<void(const char*)>(LoadLUAScriptFile);
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

    // #NOTE (Juan): Memory
    sol::usertype<DynamicString> dynamicString_usertype = lua.new_usertype<DynamicString>("dynamicstring");
    dynamicString_usertype["value"] = sol::property([](DynamicString &string) { return string.value; }, [](DynamicString &string, char* value) { string = value; });;
    
    // #NOTE (Juan): C/C++
    lua["CharToInt"] = CharToInt;
    lua["IntToChar"] = IntToChar;

    // #NOTE (Juan): Data
    sol::usertype<Game> game_usertype = lua.new_usertype<Game>("game");
    game_usertype["running"] = &Game::running;
    game_usertype["updateRunning"] = &Game::updateRunning;
    game_usertype["hasFocus"] = &Game::hasFocus;
    game_usertype["version"] = &Game::version;
    game_usertype["saveSlotID"] = &Game::saveSlotID;
    lua["game"] = &gameState->game;

    sol::usertype<Camera> camera_usertype = lua.new_usertype<Camera>("camera");
    camera_usertype["size"] = &Camera::size;
    camera_usertype["ratio"] = &Camera::ratio;
    camera_usertype["nearPlane"] = &Camera::nearPlane;
    camera_usertype["farPlane"] = &Camera::farPlane;
    camera_usertype["view"] = &Camera::view;
    camera_usertype["projection"] = &Camera::projection;
    lua["camera"] = &gameState->camera;

    sol::usertype<Render> render_usertype = lua.new_usertype<Render>("render");
    render_usertype["vsync"] = &Render::vsync;
    render_usertype["framebufferEnabled"] = &Render::framebufferEnabled;
    render_usertype["frameBuffer"] = &Render::frameBuffer;
    render_usertype["renderBuffer"] = &Render::renderBuffer;
    render_usertype["depthrenderbuffer"] = &Render::depthrenderbuffer;
    render_usertype["refreshRate"] = &Render::refreshRate;
    render_usertype["size"] = &Render::size;
    render_usertype["bufferSize"] = &Render::bufferSize;
    render_usertype["renderScale"] = &Render::renderScale;
    render_usertype["scaledBufferSize"] = &Render::scaledBufferSize;
    render_usertype["windowPosition"] = &Render::windowPosition;
    render_usertype["windowSize"] = &Render::windowSize;
    render_usertype["defaultFontID"] = &Render::defaultFontID;
    render_usertype["fitStyle"] = &Render::framebufferAdjustStyle;
    lua["render"] = &gameState->render;

    sol::usertype<Time> time_usertype = lua.new_usertype<Time>("time");
    time_usertype["realTime"] = &Time::realTime;
    time_usertype["gameTime"] = &Time::gameTime;
    time_usertype["deltaTime"] = &Time::deltaTime;
    time_usertype["timeScale"] = &Time::timeScale;
    time_usertype["gameFrames"] = &Time::gameFrames;
    time_usertype["frames"] = &Time::frames;
    time_usertype["fpsLimit"] = &Time::fpsLimit;
    time_usertype["fpsFixed"] = &Time::fpsFixed;
    time_usertype["fpsDelta"] = &Time::fpsDelta;
    lua["time"] = &gameState->time;

    sol::usertype<Sound> sound_usertype = lua.new_usertype<Sound>("sound");
    sound_usertype["bindingsEnabled"] = &Sound::bindingsEnabled;
    lua["sound"] = &gameState->sound;

    sol::usertype<TouchPoint> touchpoint_usertype = lua.new_usertype<TouchPoint>("touchpoint");
    touchpoint_usertype["state"] = &TouchPoint::state;
    touchpoint_usertype["x"] = &TouchPoint::x;
    touchpoint_usertype["y"] = &TouchPoint::y;
    touchpoint_usertype["pressure"] = &TouchPoint::pressure;

    sol::usertype<Controller> controller_usertype = lua.new_usertype<Controller>("controller");
    controller_usertype["axisValue"] = &Controller::axisValue;
    controller_usertype["buttonState"] = &Controller::buttonState;
    controller_usertype["touchPoints"] = &Controller::touchPoints;
        
    sol::usertype<Input> input_usertype = lua.new_usertype<Input>("input");
    input_usertype["mouseTextureID"] = &Input::mouseTextureID;
    input_usertype["mousePosition"] = &Input::mousePosition;
    input_usertype["mouseDeltaPosition"] = &Input::mouseDeltaPosition;
    input_usertype["mouseScreenPosition"] = &Input::mouseScreenPosition;
    input_usertype["mouseScreenDeltaPosition"] = &Input::mouseScreenDeltaPosition;
    input_usertype["mouseWheel"] = &Input::mouseWheel;
    input_usertype["textInputBuffer"] = sol::property([](Input &input) { return input.textInputBuffer->value; }, [](Input &input, char* value) { return *input.textInputBuffer = value; });
    input_usertype["keyState"] = sol::property([](Input &input) { return &input.keyState; });
    input_usertype["anyReasonableKeyState"] = &Input::anyReasonableKeyState;
    input_usertype["anyKeyState"] = &Input::anyKeyState;
    input_usertype["controllerState"] = &Input::controllerState;
    input_usertype["mouseState"] = sol::property([](Input &input) { return &input.mouseState; });
    input_usertype["anyMouseState"] = &Input::anyMouseState;
    lua["input"] = &gameState->input;

    lua["SetCustomCursor"] = SetCustomCursor;
    lua["DisableCustomCursor"] = DisableCustomCursor;

    lua["KEY_COUNT"] = KEY_COUNT;
    lua["MOUSE_COUNT"] = MOUSE_COUNT;
    lua["CONTROLLER_COUNT"] = CONTROLLER_COUNT;
    lua["CONTROLLER_AXIS_COUNT"] = CONTROLLER_AXIS_COUNT;
    lua["CONTROLLER_BUTTON_COUNT"] = CONTROLLER_BUTTON_COUNT;
    lua["CONTROLLER_TOUCHPAD_POINTS"] = CONTROLLER_TOUCHPAD_POINTS;
    lua["CONTROLLER_DEADZONE_THRESHOLD"] = CONTROLLER_DEADZONE_THRESHOLD;
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
    lua["GetClipboardText"] = SDL_GetClipboardText;
    lua["SetClipboardText"] = SDL_SetClipboardText;

    // #NOTE (Juan): Render
    lua["TextureAdjustStyle_Stretch"] = TextureAdjustStyle_Stretch;
    lua["TextureAdjustStyle_FitRatio"] = TextureAdjustStyle_FitRatio;
    lua["TextureAdjustStyle_KeepRatioX"] = TextureAdjustStyle_KeepRatioX;
    lua["TextureAdjustStyle_KeepRatioY"] = TextureAdjustStyle_KeepRatioY;
    
    lua["colorLocation"] = &colorLocation;
    lua["mvpLocation"] = &mvpLocation;
    
    lua["bufferSizeLocation"] = &bufferSizeLocation;
    lua["scaledBufferSizeLocation"] = &scaledBufferSizeLocation;
    lua["textureSizeLocation"] = &textureSizeLocation;
    lua["dimensionsLocation"] = &dimensionsLocation;
    lua["borderLocation"] = &borderLocation;
    
    lua["timeLocation"] = &timeLocation;

    lua["DrawClear"] = DrawClear;
    lua["DrawSetStyle"] = DrawSetStyle;
    lua["DrawColor"] = DrawColor;
    lua["DrawDefaultTransparent"] = sol::resolve<void()>(DrawTransparent);
    lua["DrawTransparent"] = sol::resolve<void(u32, u32, u32, u32, u32, u32)>(DrawTransparent);
    lua["DrawTransparentDisable"] = DrawTransparentDisable;
    lua["DrawSetLayer"] = DrawSetLayer;
    lua["DrawSetTransform"] = DrawSetTransform;
    lua["DrawPushTransform"] = DrawPushTransform;
    lua["DrawPopTransform"] = DrawPopTransform;
    lua["DrawDefaultTransform"] = DrawDefaultTransform;
    lua["DrawLineWidth"] = DrawLineWidth;
    lua["DrawLine"] = DrawLine;
    lua["DrawTriangle"] = DrawTriangle;
    lua["DrawRectangle"] = DrawRectangle;
    lua["DrawCircle"] = DrawCircle;
    lua["DrawInstancedCircles"] = DrawInstancedCircles;
    lua["DrawTextureParameters"] = DrawTextureParameters;
    lua["DrawTexture"] = DrawTexture;
    lua["DrawImage"] = DrawImage;
    lua["DrawImageUV"] = DrawImageUV;
    lua["DrawImage9Slice"] = DrawImage9Slice;
    lua["DrawAtlasSprite"] = DrawAtlasSprite;
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
    lua["ColorHexRGBA"] = ColorHexRGBA;
    lua["ColorHexRGB"] = ColorHexRGB;
    
    lua["GetSceneFilepath"] = GetSceneFilepath;
    lua["LoadLUAScene"] = LoadLUAScene;
    
    lua["ImageRenderFlag_Fit"] = ImageRenderFlag_Fit;
    lua["ImageRenderFlag_KeepRatioX"] = ImageRenderFlag_KeepRatioX;
    lua["ImageRenderFlag_KeepRatioY"] = ImageRenderFlag_KeepRatioY;
    lua["ImageRenderFlag_NoMipMaps"] = ImageRenderFlag_NoMipMaps;

    lua["TextRenderFlag_Left"] = TextRenderFlag_Left;
    lua["TextRenderFlag_Center"] = TextRenderFlag_Center;
    lua["TextRenderFlag_Right"] = TextRenderFlag_Right;
    lua["TextRenderFlag_LetterWrap"] = TextRenderFlag_LetterWrap;
    lua["TextRenderFlag_WordWrap"] = TextRenderFlag_WordWrap;

    lua["PerspectiveProjection"] = PerspectiveProjection;
    lua["OrtographicProjection"] = sol::resolve<m44(f32, f32, f32, f32)>(OrtographicProjection);
    
    lua["Perlin2D"] = sol::resolve<f32(f32, f32)>(Perlin2D);
    lua["Perlin2DInt"] = sol::resolve<f32(i32, i32)>(Perlin2D);
    lua["Perlin2DOctaves"] = Perlin2DOctaves;

    // #NOTE (Juan): GLRender
    sol::usertype<TextureAsset> gltexture_usertype = lua.new_usertype<TextureAsset>("textureasset");
    gltexture_usertype["textureID"] = &TextureAsset::textureID;
    gltexture_usertype["width"] = &TextureAsset::width;
    gltexture_usertype["height"] = &TextureAsset::height;
    gltexture_usertype["channels"] = &TextureAsset::channels;

    lua["LoadTextureID"] = BindTextureID;
    lua["LoadSceneTexture"] = LoadSceneTextureFile;
    lua["LoadPermanentTexture"] = LoadPermanentTexture;
    lua["TextureSize"] = sol::resolve<std::tuple<f32, f32>(const char*)>(TextureSizeBinding);
    // lua["TextureSizeID"] = sol::resolve<std::tuple<f32, f32>(u32)>(TextureSizeBinding);
    lua["GenerateFont"] = sol::resolve<u32(const char*, f32, u32, u32)>(GenerateFont);
    lua["DEFAULT_FONT_ATLAS_WIDTH"] = DEFAULT_FONT_ATLAS_WIDTH;
    lua["DEFAULT_FONT_ATLAS_HEIGHT"] = DEFAULT_FONT_ATLAS_HEIGHT;
    lua["GenerateBitmapFontStrip"] = GenerateBitmapFontStrip;
    lua["CompileProgram"] = CompileProgram;
    lua["CompileProgramPlatform"] = CompileProgramPlatform;
    lua["GetUniformLocation"] = glGetUniformLocation;
    lua["SetUniform1F"] = SetUniform1F;
    lua["SetUniform2F"] = SetUniform2F;
    lua["SetUniform3F"] = SetUniform3F;
    lua["SetUniform4F"] = SetUniform4F;
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
    lua["PlaySound"] = SoundPlaySimple;
    lua["PlaySoundLoop"] = PlaySound;
    lua["StopSound"] = StopSound;
    lua["SetMasterVolume"] = SetMasterVolume;
    lua["dbToVolume"] = dbToVolume;
    lua["volumeToDB"] = volumeToDB;

    // #NOTE (Juan): Console
    lua["LogConsole"] = LogConsole;
    lua["LogConsoleError"] = LogConsoleError;
    lua["LogConsoleCommand"] = LogConsoleCommand;

    // #NOTE (Juan): Serialization
    lua["SaveData"] = SaveData;
    
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

    lua["EditorSaveGetString"] = EditorSaveGetString;
    lua["EditorSaveSetString"] = EditorSaveSetString;
    lua["EditorSaveGetBool"] = EditorSaveGetBool;
    lua["EditorSaveSetBool"] = EditorSaveSetBool;
    lua["EditorSaveGetI32"] = EditorSaveGetI32;
    lua["EditorSaveSetI32"] = EditorSaveSetI32;
    lua["EditorSaveGetF32"] = EditorSaveGetF32;
    lua["EditorSaveSetF32"] = EditorSaveSetF32;
    lua["EditorSaveGetV2"] = EditorSaveGetV2;
    lua["EditorSaveSetV2"] = EditorSaveSetV2;
    
    // #NOTE (Juan): Runtime
    lua["RuntimeQuit"] = RuntimeQuit;

    // #NOTE (Juan): Editor
#ifdef PLATFORM_EDITOR
    lua["ChangeLogFlag"] = ChangeLogFlag_;

    sol::usertype<ShaderDebuggerWindow> editorShaderDebugger_usertype = lua.new_usertype<ShaderDebuggerWindow>("editorShaderDebugger");
    editorShaderDebugger_usertype["programIndexChanged"] = &ShaderDebuggerWindow::programIndexChanged;
    editorShaderDebugger_usertype["programIndex"] = &ShaderDebuggerWindow::programIndex;
    lua["editorShaderDebugger"] = &editorShaderDebugger;

    lua["LogFlag_PERFORMANCE"] = LogFlag_PERFORMANCE;
    lua["LogFlag_RENDER"] = LogFlag_RENDER;
    lua["LogFlag_MEMORY"] = LogFlag_MEMORY;
    lua["LogFlag_TEXTURE"] = LogFlag_TEXTURE;
    lua["LogFlag_SOUND"] = LogFlag_SOUND;
    lua["LogFlag_INPUT"] = LogFlag_INPUT;
    lua["LogFlag_TIME"] = LogFlag_TIME;
    lua["LogFlag_LUA"] = LogFlag_LUA;

    lua["LogFlag_GAME"] = LogFlag_GAME;
    lua["LogFlag_C_SCRIPTING"] = LogFlag_C_SCRIPTING;
    lua["LogFlag_LUA_SCRIPTING"] = LogFlag_LUA_SCRIPTING;
    lua["LogFlag_LUA_SCRIPTING_FUNCTION"] = LogFlag_LUA_SCRIPTING_FUNCTIONS;

    lua["ImGuiCond_FirstUseEver"] = ImGuiCond_FirstUseEver;

    lua["ImGuiSetNextWindowSize"] = ImGuiSetNextWindowSize;
    lua["ImGuiSetNextWindowSizeConstraints"] = ImGuiSetNextWindowSizeConstraints;
    lua["ImGuiBegin"] = ImGuiBegin;
    lua["ImGuiEnd"] = ImGui::End;
    lua["ImGuiSameLine"] = ImGui::SameLine;
    lua["ImGuiSpacing"] = ImGui::Spacing;
    lua["ImGuiSeparator"] = ImGui::Separator;
    lua["ImGuiDummy"] = ImGuiDummy;
    lua["ImGuiInputInt"] = ImGuiInputInt;
    lua["ImGuiInputFloat"] = ImGuiInputFloat;
    lua["ImGuiSmallButton"] = ImGui::SmallButton;
    lua["ImGuiButton"] = ImGuiButton;
    lua["ImGuiImage"] = ImGuiImage;
    lua["ImGuiImageButton"] = ImGuiImageButton;
    lua["ImGuiPushStyleColor"] = ImGuiPushStyleColor;
    lua["ImGuiCol_Button"] = ImGuiCol_Button;
    lua["ImGuiPopStyleColor"] = ImGui::PopStyleColor;
    lua["ImGuiPushItemWidth"] = ImGui::PushItemWidth;
    lua["ImGuiPopItemWidth"] = ImGui::PopItemWidth;
    lua["ImGuiPushID"] = sol::resolve<void(const char*)>(ImGui::PushID);
    lua["ImGuiPopID"] = ImGui::PopID;
    lua["ImGuiBeginMenu"] = ImGui::BeginMenu;
    lua["ImGuiEndMenu"] = ImGui::EndMenu;
    lua["ImGuiCheckbox"] = ImGuiCheckbox;
    lua["ImGuiTextUnformatted"] = ImGui::TextUnformatted;
    lua["ImGuiInputText"] = ImGuiInputText;
    lua["ImGuiGetWindowDrawList"] = ImGui::GetWindowDrawList;
    lua["ImGuiGetCursorScreenPos"] = ImGuiGetCursorScreenPos;
    lua["ImGuiAddImage"] = ImGuiAddImage;
#endif
}

void LUAScriptingMathBindings()
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
    v3_usertype["r"] = sol::property([](v3 &v) { return v.x; }, [](v3 &v, f32 f) { v.x = f; });
    v3_usertype["g"] = sol::property([](v3 &v) { return v.y; }, [](v3 &v, f32 f) { v.y = f; });
    v3_usertype["b"] = sol::property([](v3 &v) { return v.z; }, [](v3 &v, f32 f) { v.z = f; });
    v3_usertype["e"] = sol::property([](v3 &v) { return &v.e; });

    sol::usertype<v4> v4_usertype = lua.new_usertype<v4>("v4");
    v4_usertype["x"] = sol::property([](v4 &v) { return v.x; }, [](v4 &v, f32 f) { v.x = f; });
    v4_usertype["y"] = sol::property([](v4 &v) { return v.y; }, [](v4 &v, f32 f) { v.y = f; });
    v4_usertype["z"] = sol::property([](v4 &v) { return v.z; }, [](v4 &v, f32 f) { v.z = f; });
    v4_usertype["w"] = sol::property([](v4 &v) { return v.w; }, [](v4 &v, f32 f) { v.w = f; });
    v4_usertype["r"] = sol::property([](v4 &v) { return v.x; }, [](v4 &v, f32 f) { v.x = f; });
    v4_usertype["g"] = sol::property([](v4 &v) { return v.y; }, [](v4 &v, f32 f) { v.y = f; });
    v4_usertype["b"] = sol::property([](v4 &v) { return v.z; }, [](v4 &v, f32 f) { v.z = f; });
    v4_usertype["a"] = sol::property([](v4 &v) { return v.w; }, [](v4 &v, f32 f) { v.w = f; });
    v4_usertype["e"] = sol::property([](v4 &v) { return &v.e; });

    sol::usertype<m33> m33_usertype = lua.new_usertype<m33>("m33");
    m33_usertype["_00"] = sol::property([](m33 &m) { return m._00; }, [](m33 &m, f32 f) { m._00 = f; });
    m33_usertype["_10"] = sol::property([](m33 &m) { return m._10; }, [](m33 &m, f32 f) { m._10 = f; });
    m33_usertype["_20"] = sol::property([](m33 &m) { return m._20; }, [](m33 &m, f32 f) { m._20 = f; });
    m33_usertype["_01"] = sol::property([](m33 &m) { return m._01; }, [](m33 &m, f32 f) { m._01 = f; });
    m33_usertype["_11"] = sol::property([](m33 &m) { return m._11; }, [](m33 &m, f32 f) { m._11 = f; });
    m33_usertype["_21"] = sol::property([](m33 &m) { return m._21; }, [](m33 &m, f32 f) { m._21 = f; });
    m33_usertype["_02"] = sol::property([](m33 &m) { return m._02; }, [](m33 &m, f32 f) { m._02 = f; });
    m33_usertype["_12"] = sol::property([](m33 &m) { return m._12; }, [](m33 &m, f32 f) { m._12 = f; });
    m33_usertype["_22"] = sol::property([](m33 &m) { return m._22; }, [](m33 &m, f32 f) { m._22 = f; });
    m33_usertype["e"] = sol::property([](m33 &m) { return &m.e; });

    sol::usertype<m44> m44_usertype = lua.new_usertype<m44>("m44");
    m44_usertype["_00"] = sol::property([](m44 &m) { return m._00; }, [](m44 &m, f32 f) { m._00 = f; });
    m44_usertype["_10"] = sol::property([](m44 &m) { return m._10; }, [](m44 &m, f32 f) { m._10 = f; });
    m44_usertype["_20"] = sol::property([](m44 &m) { return m._20; }, [](m44 &m, f32 f) { m._20 = f; });
    m44_usertype["_30"] = sol::property([](m44 &m) { return m._30; }, [](m44 &m, f32 f) { m._30 = f; });
    m44_usertype["_01"] = sol::property([](m44 &m) { return m._01; }, [](m44 &m, f32 f) { m._01 = f; });
    m44_usertype["_11"] = sol::property([](m44 &m) { return m._11; }, [](m44 &m, f32 f) { m._11 = f; });
    m44_usertype["_21"] = sol::property([](m44 &m) { return m._21; }, [](m44 &m, f32 f) { m._21 = f; });
    m44_usertype["_31"] = sol::property([](m44 &m) { return m._31; }, [](m44 &m, f32 f) { m._31 = f; });
    m44_usertype["_02"] = sol::property([](m44 &m) { return m._02; }, [](m44 &m, f32 f) { m._02 = f; });
    m44_usertype["_12"] = sol::property([](m44 &m) { return m._12; }, [](m44 &m, f32 f) { m._12 = f; });
    m44_usertype["_22"] = sol::property([](m44 &m) { return m._22; }, [](m44 &m, f32 f) { m._22 = f; });
    m44_usertype["_32"] = sol::property([](m44 &m) { return m._32; }, [](m44 &m, f32 f) { m._32 = f; });
    m44_usertype["_03"] = sol::property([](m44 &m) { return m._03; }, [](m44 &m, f32 f) { m._03 = f; });
    m44_usertype["_13"] = sol::property([](m44 &m) { return m._13; }, [](m44 &m, f32 f) { m._13 = f; });
    m44_usertype["_23"] = sol::property([](m44 &m) { return m._23; }, [](m44 &m, f32 f) { m._23 = f; });
    m44_usertype["_33"] = sol::property([](m44 &m) { return m._33; }, [](m44 &m, f32 f) { m._33 = f; });
    m44_usertype["e"] = sol::property([](m44 &m) { return &m.e; });

    sol::usertype<rectangle2> rectangle2_usertype = lua.new_usertype<rectangle2>("rectangle2");
    rectangle2_usertype["x"] = sol::property([](rectangle2 &r) { return r.x; }, [](rectangle2 &r, f32 f) { r.x = f; });
    rectangle2_usertype["y"] = sol::property([](rectangle2 &r) { return r.y; }, [](rectangle2 &r, f32 f) { r.y = f; });
    rectangle2_usertype["width"] = sol::property([](rectangle2 &r) { return r.width; }, [](rectangle2 &r, f32 f) { r.width = f; });
    rectangle2_usertype["height"] = sol::property([](rectangle2 &r) { return r.height; }, [](rectangle2 &r, f32 f) { r.height = f; });

    sol::usertype<transform2D> transform2D_usertype = lua.new_usertype<transform2D>("transform2D");
    transform2D_usertype["position"] = &transform2D::position;
    transform2D_usertype["scale"] = &transform2D::scale;
    transform2D_usertype["angle"] = &transform2D::angle;

    // #NOTE (Juan): GameMath
    lua["V2"] = V2;
    lua["V3"] = V3;
    lua["V4"] = V4;

    lua["Rectangle2"] = Rectangle2;
    
    lua["M22"] = M22;
    lua["IdM22"] = IdM22;
    lua["M33"] = M33;
    lua["IdM33"] = IdM33;
    lua["M44fromM33"] = sol::resolve<m44(m33)>(M44);
    lua["M44"] = sol::resolve<m44(f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32)>(M44);
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
    lua["math"]["lerp"] = Lerp;
}

#endif