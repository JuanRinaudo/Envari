-- #ifndef LUA_SCRIPTING_BINDINGS_CPP
-- #define LUA_SCRIPTING_BINDINGS_CPP

-- #if PLATFORM_WASM
--     #include <GLES3/gl3.h>
--     #define GL_PROFILE_GLES3
-- #else
--     #include "GL3W/gl3w.h"
-- #endif

-- #include "STB/stb_truetype.h" 

-- #include "Defines.h"
-- #include "Templates.h"
-- #include "LUA/sol.hpp"
-- #include "Miniaudio/miniaudio.h"

-- #include "MemoryStructs.h"
-- #include "MathStructs.h"
-- #include "GameStructs.h"

-- #include <SDL.h>

-- #ifdef PLATFORM_EDITOR
-- #ifdef PLATFORM_WINDOWS
--     #include <psapi.h>
-- #endif
-- #include "IMGUI/imgui.h"
-- #include "EditorStructs.h"
-- extern ConsoleWindow editorConsole;
-- #endif

-- extern sol::state lua;

-- #ifdef PLATFORM_EDITOR
-- extern void Log_(ConsoleWindow* console, ConsoleLogType type, const char* file, u32 line, const char* fmt, ...);
-- #define Log(fmt, ...) Log_(&editorConsole, ConsoleLogType_NORMAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
-- #define LogError(fmt, ...) Log_(&editorConsole, ConsoleLogType_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
-- #define LogCommand(fmt, ...) Log_(&editorConsole, ConsoleLogType_COMMAND, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
-- #else
-- extern void Log_(ConsoleLogType type, const char* fmt, ...);
-- #define Log(fmt, ...) Log_(ConsoleLogType_NORMAL, fmt, ##__VA_ARGS__)
-- #define LogError(fmt, ...) Log_(ConsoleLogType_ERROR, fmt, ##__VA_ARGS__)
-- #define LogCommand(fmt, ...) Log_(ConsoleLogType_COMMAND, fmt, ##__VA_ARGS__)
-- #endif

-- #define PushStruct(arena, type) (type *)PushSize_(arena, sizeof(type))
-- #define PushArray(arena, count, type) (type *)PushSize_(arena, ((count)*sizeof(type)))
-- #define PushSize(arena, size) PushSize_(arena, size)
-- extern void *PushSize_(MemoryArena *arena, size_t size);
-- extern void *PushSize_(TemporaryMemory *memory, size_t size);
-- extern char *PushString(MemoryArena *arena, const char *string);

-- // #NOTE(Juan): Engine
-- extern Data *gameState;
-- extern PermanentData *permanentState;
-- extern SceneData *sceneState;
-- extern TemporalData *temporalState;
-- extern TemporaryMemory renderTemporaryMemory;

-- extern SerializableTable* configSave;
-- extern SerializableTable* saveData;
-- extern SerializableTable* editorSave;

-- // #NOTE (Juan): Bindings
-- extern void SaveData();

-- extern void LoadScriptFile(const char* filePath);
-- extern void LoadLUALibrary(sol::lib library);

-- extern void SetCustomCursor(TextureAsset texture);
-- extern void DisableCustomCursor();

-- extern v2 V2(f32 x, f32 y);

-- extern bool MouseOverRectangle(rectangle2 rectangle);
-- extern bool ClickOverRectangle(rectangle2 rectangle, i32 button);
-- extern bool ClickedOverRectangle(rectangle2 rectangle, i32 button);

-- extern m44 PerspectiveProjection(f32 fovY, f32 aspect, f32 nearPlane, f32 farPlane);
-- extern m44 OrtographicProjection(f32 left, f32 right, f32 top, f32 bottom, f32 nearPlane, f32 farPlane);
-- extern m44 OrtographicProjection(f32 size, f32 aspect, f32 nearPlane, f32 farPlane);

-- extern f32 Perlin2D(f32 x, f32 y);
-- extern f32 Perlin2D(i32 x, i32 y);
-- extern f32 Perlin2DOctaves(f32 x, f32 y, u32 octaves, f32 frecuency);

-- extern u32 colorLocation;
-- extern u32 mvpLocation;

-- extern u32 bufferSizeLocation;
-- extern u32 scaledBufferSizeLocation;
-- extern u32 textureSizeLocation;
-- extern u32 dimensionsLocation;
-- extern u32 borderLocation;

-- extern u32 timeLocation;

-- extern void DrawClear(f32 red, f32 green, f32 blue, f32 alpha);
-- extern void DrawSetStyle(const char* filepath, const char* filepathHovered, const char* filepathDown, f32 slice);
-- extern void DrawColor(f32 red, f32 green, f32 blue, f32 alpha);
-- extern void DrawTransparent();
-- extern void DrawTransparent(u32 modeRGB, u32 modeAlpha, u32 srcRGB, u32 dstRGB, u32 srcAlpha, u32 dstAlpha);
-- extern void DrawTransparentDisable();
-- extern void DrawSetLayer(u32 targetLayer, bool transparent);
-- extern void DrawSetTransform(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle);
-- extern void DrawPushTransform(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle);
-- extern void DrawPopTransform();
-- extern void DrawLineWidth(f32 width);
-- extern void DrawLine(f32 startX, f32 startY, f32 endX, f32 endY);
-- extern void DrawTriangle(f32 p1X, f32 p1Y, f32 p2X, f32 p2Y, f32 p3X, f32 p3Y);
-- extern void DrawRectangle(f32 posX, f32 posY, f32 sizeX, f32 sizeY);
-- extern void DrawCircle(f32 posX, f32 posY, f32 radius, u32 segments);
-- extern void DrawInstancedCircles(u32 instanceCount, std::vector<f32> positions, f32 radius, u32 segments);
-- extern void DrawTextureParameters(u32 wrapS, u32 wrapT, u32 minFilter, u32 magFilter);
-- extern void DrawTexture(f32 posX, f32 posY, f32 sizeX, f32 sizeY, u32 textureID);
-- extern void DrawImage(f32 posX, f32 posY, const char* filepath, u32 renderFlags);
-- extern void DrawImageUV(f32 posX, f32 posY, f32 uvX, f32 uvY, f32 uvEndX, f32 uvEndY, const char* filepath);
-- extern void DrawImage9Slice(f32 posX, f32 posY, f32 endX, f32 endY, f32 slice, const char* filepath);
-- extern void DrawAtlasSprite(f32 posX, f32 posY, const char* filepath, const char* atlasName, const char* key);
-- extern void DrawSetFont(i32 fontID);
-- extern void DrawChar(f32 posX, f32 posY, const char singleChar);
-- extern void DrawString(f32 posX, f32 posY, const char* string, u32 renderFlags);
-- extern void DrawStyledString(f32 posX, f32 posY, f32 endX, f32 endY, const char* string, u32 renderFlags);
-- extern void ClearInputBuffer();
-- extern bool DrawStringInput(f32 posX, f32 posY, f32 endX, f32 endY, const char* baseText, u32 maxSize);
-- extern bool DrawButton(f32 posX, f32 posY, f32 endX, f32 endY, const char* label);
-- extern i32 DrawMultibutton(f32 posX, f32 posY, f32 endX, f32 height, f32 yPadding, const char* options);
-- extern void DrawSetUniform(u32 locationID, UniformType type);
-- extern void DrawOverrideProgram(u32 programID);
-- extern void DrawOverrideVertices(f32* vertices, u32 count);
-- extern void DrawOverrideIndices(u32* indices, u32 count);
-- extern v4 ColorHexRGBA(u32 hex);
-- extern v4 ColorHexRGB(u32 hex);

-- extern const char* GetSceneFilepath();
-- extern void LoadLUAScene(const char* luaFilepath);

-- extern u32 defaultFontID;

-- extern void BindTextureID(u32 textureID, f32 width, f32 height);
-- extern TextureAsset LoadTextureFile(const char *texturePath, bool permanentAsset);
-- extern v2 TextureSize(const char* texturePath);
-- extern v2 TextureSize(u32 textureID);
-- extern u32 GenerateFont(const char *filepath, f32 fontSize, u32 width, u32 height);
-- extern u32 GenerateBitmapFontStrip(const char *filepath, const char* glyphs, u32 glyphWidth, u32 glyphHeight);
-- extern u32 CompileProgram(const char *vertexShaderSource, const char *fragmentShaderSource);
-- extern u32 CompileProgramPlatform(const char *vertexShaderPlatform, const char *fragmentShaderPlatform);

-- extern SoundInstance* PlaySound(const char* filepath, f32 volume, bool loop, bool unique);
-- extern void SoundStop(SoundInstance* instance);
-- extern void SetMasterVolume(float value);
-- extern float dbToVolume(float db);
-- extern float volumeToDB(float volume);

-- extern v2 V2(f32 x, f32 y);
-- extern v3 V3(f32 x, f32 y, f32 z);
-- extern v4 V4(f32 x, f32 y, f32 z, f32 w);

-- extern rectangle2 Rectangle2(f32 x, f32 y, f32 width, f32 height);

-- extern m22 M22(
--     f32 _00, f32 _01,
--     f32 _10, f32 _11);
-- extern m22 IdM22();

-- extern m33 M33(
--     f32 _00, f32 _01, f32 _02,
--     f32 _10, f32 _11, f32 _12,
--     f32 _20, f32 _21, f32 _22);
-- extern m33 IdM33();

-- extern m44 M44(
--     f32 _00, f32 _01, f32 _02, f32 _03,
--     f32 _10, f32 _11, f32 _12, f32 _13,
--     f32 _20, f32 _21, f32 _22, f32 _23,
--     f32 _30, f32 _31, f32 _32, f32 _33);
-- extern m44 IdM44();

-- extern f32 Lerp(f32 a, f32 b, f32 t);

-- extern transform2D Transform2D(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle);

-- extern f32 Length(v2 a);

-- extern void RuntimeQuit();

-- extern void ChangeLogFlag_(u32 newFlag);

-- extern void SerializeTable(SerializableTable** table, const char* filepath);

-- #ifdef PLATFORM_EDITOR
-- extern ShaderDebuggerWindow editorShaderDebugger;
-- #endif

-- GenerateTableGetExtern(String, char*)
-- GenerateTableSetExtern(String, const char*, SerializableType_STRING)
-- GenerateTableGetExtern(Bool, bool)
-- GenerateTableSetExtern(Bool, bool, SerializableType_BOOL)
-- GenerateTableGetExtern(I32, i32)
-- GenerateTableSetExtern(I32, i32, SerializableType_I32)
-- GenerateTableGetExtern(F32, f32)
-- GenerateTableSetExtern(F32, f32, SerializableType_F32)
-- GenerateTableGetExtern(V2, v2)
-- GenerateTableSetExtern(V2, v2, SerializableType_V2)

-- #define LUAEditorSaveGetSet(POSTFIX, valueType) static valueType EditorSaveGet ## POSTFIX (const char* key, valueType defaultValue) \
-- { \
--     return TableGet ## POSTFIX (&editorSave, key, defaultValue); \
-- } \
-- static void EditorSaveSet ## POSTFIX (const char* key, valueType value) \
-- { \
--     TableSet ## POSTFIX ## _(&permanentState->arena, &editorSave, key, value); \
-- }

-- #define LUASaveGetSet(POSTFIX, valueType) static valueType SaveGet ## POSTFIX (const char* key, valueType defaultValue) \
-- { \
--     return TableGet ## POSTFIX (&saveData, key, defaultValue); \
-- } \
-- static void SaveSet ## POSTFIX (const char* key, valueType value) \
-- { \
--     TableSet ## POSTFIX ## _(&permanentState->arena, &saveData, key, value); \
-- }

-- // #NOTE(Juan): Console
-- static void LogConsole(const char* log)
-- {
-- 	Log(log);
-- }

-- static void LogConsoleError(const char* log)
-- {
-- 	LogError(log);
-- }

-- static void LogConsoleCommand(const char* log)
-- {
-- 	LogCommand(log);
-- }

-- // #NOTE(Juan): Cast
-- static i32 CharToInt(const char* singleChar)
-- {
--     return (i32)singleChar[0];
-- }

-- char castChar[1];
-- static char* IntToChar(i32 value)
-- {
--     castChar[0] = (char)value;
--     return castChar;
-- }

-- static void DrawDefaultTransform()
-- {
--     DrawSetTransform(0, 0, 1, 1, 0);
-- }

-- static void DrawDisableOverrideVertices()
-- {
--     DrawOverrideVertices(0, 0);
-- }

-- static void DrawDisableOverrideIndices()
-- {    
--     DrawOverrideIndices(0, 0);
-- }

-- static void DrawDisableOverrideProgram()
-- {    
--     DrawOverrideProgram(0);
-- }

-- static void SetUniform1F(u32 programID, u32 locationID, f32 v0)
-- {
-- #if PLATFORM_WASM
--     glUseProgram(programID);
--     glUniform1f(locationID, v0);
-- #else
--     glProgramUniform1f(programID, locationID, v0);
-- #endif
-- }

-- static void SetUniform2F(u32 programID, u32 locationID, f32 v0, f32 v1)
-- {
-- #if PLATFORM_WASM
--     glUseProgram(programID);
--     glUniform2f(locationID, v0, v1);
-- #else
--     glProgramUniform2f(programID, locationID, v0, v1);
-- #endif
-- }

-- static void SetUniform3F(u32 programID, u32 locationID, f32 v0, f32 v1, f32 v2)
-- {
-- #if PLATFORM_WASM
--     glUseProgram(programID);
--     glUniform3f(locationID, v0, v1, v2);
-- #else
--     glProgramUniform3f(programID, locationID, v0, v1, v2);
-- #endif
-- }

-- static void SetUniform4F(u32 programID, u32 locationID, f32 v0, f32 v1, f32 v2, f32 v3)
-- {
-- #if PLATFORM_WASM
--     glUseProgram(programID);
--     glUniform4f(locationID, v0, v1, v2, v3);
-- #else
--     glProgramUniform4f(programID, locationID, v0, v1, v2, v3);
-- #endif
-- }

-- // #NOTE(Juan): GLRender
-- static std::tuple<f32, f32> TextureSizeBinding(const char* texturePath)
-- {
--     v2 size = TextureSize(texturePath);
--     return std::tuple<f32, f32>(size.x, size.y);
-- }

-- // static std::tuple<f32, f32> TextureSizeBinding(u32 textureID)
-- // {
-- //     v2 size = TextureSize(textureID);
-- //     return std::tuple<f32, f32>(size.x, size.y);
-- // }

-- static i32 GetTextureID(TextureAsset texture)
-- {
--     return texture.textureID;
-- }

-- static TextureAsset LoadSceneTextureFile(const char *texturePath)
-- {
--     return LoadTextureFile(texturePath, false);
-- }

-- static TextureAsset LoadPermanentTexture(const char *texturePath)
-- {
--     return LoadTextureFile(texturePath, true);
-- }

-- // #NOTE(Juan): Render
-- static i32 DrawMultibuttonLUA(f32 posX, f32 posY, f32 endX, f32 height, f32 yPadding, const char* options)
-- {
--     return DrawMultibutton(posX, posY, endX, height, yPadding, options) + 1;
-- }

-- #define GenerateRenderTemporaryPush(PREFIX, type) static type* RenderTemporaryPush ## PREFIX (type value) \
-- { \
--     u32 size = sizeof(type); \
--     if(renderTemporaryMemory.arena->used + size < renderTemporaryMemory.arena->size) { \
--         type *valuePointer = (type*)PushSize(&renderTemporaryMemory, size); \
--         *valuePointer = value; \
--         return valuePointer; \
--     } \
--     else { \
--         InvalidCodePath; \
--         return 0; \
--     } \
-- }

-- GenerateRenderTemporaryPush(Float, f32);
-- GenerateRenderTemporaryPush(Vector2, v2);

-- // #NOTE(Juan): Save
-- static char* SaveGetString(const char* key, const char* defaultValue)
-- {
--     char* temporalString = PushString(&temporalState->arena, defaultValue);
--     return TableGetString(&saveData, key, temporalString);
-- }

-- static void SaveSetString(const char* key, const char* value)
-- {
--     char* permanentString = PushString(&permanentState->arena, value);
--     TableSetString_(&permanentState->arena, &saveData, key, permanentString);
-- }

-- static char* EditorSaveGetString(const char* key, const char* defaultValue)
-- {
--     char* temporalString = PushString(&temporalState->arena, defaultValue);
--     return TableGetString(&editorSave, key, temporalString);
-- }

-- static void EditorSaveSetString(const char* key, const char* value)
-- {
--     char* permanentString = PushString(&permanentState->arena, value);
--     TableSetString_(&permanentState->arena, &editorSave, key, permanentString);
-- }

-- LUASaveGetSet(Bool, bool)
-- LUASaveGetSet(I32, i32)
-- LUASaveGetSet(F32, f32)
-- LUASaveGetSet(V2, v2)

-- LUAEditorSaveGetSet(Bool, bool)
-- LUAEditorSaveGetSet(I32, i32)
-- LUAEditorSaveGetSet(F32, f32)
-- LUAEditorSaveGetSet(V2, v2)

-- // #NOTE(Juan): Sound
-- static SoundInstance* SoundPlaySimple(const char* filepath, f32 volume, bool unique) {
--     return PlaySound(filepath, volume, false, unique);
-- }

-- #ifdef PLATFORM_EDITOR
-- // #NOTE(Juan): Editor
-- static std::tuple<bool, bool> ImGuiBegin(const char* name, bool open, u32 flags) {
--     bool shouldDraw = ImGui::Begin(name, &open, flags);
--     return std::tuple<bool, bool>(open, shouldDraw);
-- }

-- static void ImGuiSetNextWindowSize(f32 width, f32 height, u32 cond) {
--     ImGui::SetNextWindowSize(ImVec2(width, height), cond);
-- }

-- static void ImGuiSetNextWindowSizeConstraints(f32 minX, f32 minY, f32 maxX, f32 maxY) {
--     ImGui::SetNextWindowSizeConstraints(ImVec2(minX, minY), ImVec2(maxX, maxY));
-- }

-- static void ImGuiDummy(f32 width, f32 height) {
--     ImGui::Dummy(ImVec2(width, height));
-- }

-- static bool ImGuiButton(const char* label, f32 width, f32 height) {
--     return ImGui::Button(label, ImVec2(width, height));
-- }

-- static std::tuple<bool, i32> ImGuiInputInt(const char* label, i32 value, i32 step, i32 fastStep, u32 flags) {
--     bool changed = ImGui::InputInt(label, &value, step, fastStep, flags);
--     return std::tuple<bool, i32>(changed, value);
-- }

-- static std::tuple<bool, f32> ImGuiInputFloat(const char* label, f32 value, f32 step, f32 fastStep, u32 flags) {
--     bool changed = ImGui::InputFloat(label, &value, step, fastStep, "%.3f", flags);
--     return std::tuple<bool, f32>(changed, value);
-- }

-- static void ImGuiImage(i32 id, f32 width, f32 height) {
--     return ImGui::Image((ImTextureID)id, ImVec2(width, height));
-- }

-- static bool ImGuiImageButton(i32 id, f32 width, f32 height) {
--     return ImGui::ImageButton((ImTextureID)id, ImVec2(width, height));
-- }

-- static void ImGuiPushStyleColor(i32 styleColor, f32 r, f32 g, f32 b, f32 a) {
--     ImGui::PushStyleColor(styleColor, ImVec4(r, g, b, a));
-- }

-- static std::tuple<bool, bool> ImGuiCheckbox(const char* label, bool value) {
--     bool pressed = ImGui::Checkbox(label, &value);
--     return std::tuple<bool, bool>(pressed, value);
-- }

-- char inputBuffer[CONSOLE_INPUT_BUFFER_COUNT];
-- static std::tuple<bool, char*> ImGuiInputText(const char* label) {
--     bool pressed = ImGui::InputText(label, inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue, 0, 0);
--     return std::tuple<bool, char*>(pressed, inputBuffer);
-- }

-- static std::tuple<f32, f32> ImGuiGetCursorScreenPos() {
--     ImVec2 position = ImGui::GetCursorScreenPos();
--     return std::tuple<f32, f32>(position.x, position.y);
-- }

-- static void ImGuiAddImage(ImDrawList* drawList, i32 id, f32 x, f32 y, f32 width, f32 height) {
--     drawList->AddImage((ImTextureID)id, ImVec2(x, y), ImVec2(x + width, y + height));
-- }
-- #endif

-- void ScriptingBindings()
-- {
--     // #NOTE (Juan): Platform
--     lua["PLATFORM_EDITOR"] = MACRO_DEFINED(PLATFORM_EDITOR);
--     lua["PLATFORM_WINDOWS"] = MACRO_DEFINED(PLATFORM_WINDOWS);
--     lua["PLATFORM_WASM"] = MACRO_DEFINED(PLATFORM_WASM);

--     // #NOTE (Juan): Lua
--     lua["LoadScriptFile"] = sol::resolve<void(const char*)>(LoadScriptFile);
--     lua["LoadLibrary"] = LoadLUALibrary;
--     lua["SOL_LIBRARY_BASE"] = sol::lib::base;
--     lua["SOL_LIBRARY_PACKAGE"] = sol::lib::package;
--     lua["SOL_LIBRARY_COROUTINE"] = sol::lib::coroutine;
--     lua["SOL_LIBRARY_STRING"] = sol::lib::string;
--     lua["SOL_LIBRARY_OS"] = sol::lib::os;
--     lua["SOL_LIBRARY_MATH"] = sol::lib::math;
--     lua["SOL_LIBRARY_TABLE"] = sol::lib::table;
--     lua["SOL_LIBRARY_DEBUG"] = sol::lib::debug;
--     lua["SOL_LIBRARY_BIT32"] = sol::lib::bit32;
--     lua["SOL_LIBRARY_IO"] = sol::lib::io;
--     lua["SOL_LIBRARY_BIT32"] = sol::lib::bit32;
--     lua["SOL_LIBRARY_FFI"] = sol::lib::ffi;
--     lua["SOL_LIBRARY_JIT"] = sol::lib::jit;
--     lua["SOL_LIBRARY_UTF8"] = sol::lib::utf8;

--     // #NOTE (Juan): Memory
--     sol::usertype<DynamicString> dynamicString_usertype = lua.new_usertype<DynamicString>("dynamicstring");
--     dynamicString_usertype["value"] = sol::property([](DynamicString &string) { return string.value; }, [](DynamicString &string, char* value) { string = value; });;
    
--     // #NOTE (Juan): C/C++
--     lua["CharToInt"] = CharToInt;
--     lua["IntToChar"] = IntToChar;

---@class game
game = {
    running = false,
    updateRunning = false,
    hasFocus = false,
    version = 0,
    saveSlotID = 0,
}

---@class render
render = {
    vsync = false,
    framebufferEnabled = false,
    frameBuffer = 0,
    renderBuffer = 0,
    depthrenderbuffer = 0,
    refreshRate = 0,
    ---@type v2
    size = nil,
    ---@type v2
    bufferSize = nil,
    renderScale = 0.0,
    ---@type v2
    scaledBufferSize = nil,
    ---@type v2
    windowPosition = nil,
    ---@type v2
    windowSize = nil,
    defaultFontID = 0,

    ---@type TextureAdjustStyle
    fitStyle = 0,
}

---@class camera
camera = {
    size = 0.0,
    ratio = 0.0,
    nearPlane = 0.0,
    farPlane = 0.0,
    ---@type m44
    view = nil,
    ---@type m44
    projection = nil,
}

---@class time
time = {
    realLastFrameGameTime = 0.0,
    lastFrameGameTime = 0.0,
    gameTime = 0.0,
    deltaTime = 0.0,
    gameFrames = 0,
    frames = 0,

    fpsLimit = 0,
    fpsFixed = 0,
    fpsDelta = 0.0,
}

---@class sound
sound = {
    bindingsEnabled = false,
}

---@class input
input = {
    mouseTextureID = 0,
    ---@type v2
    mousePosition = nil,
    ---@type v2
    mouseDeltaPosition = nil,
    ---@type v2
    mouseScreenPosition = nil,
    ---@type v2
    mouseScreenDeltaPosition = nil,
    mouseWheel = 0,
    ---@type string
    textInputBuffer = nil,
    ---@type KeyState[]
    keyState = nil,
    ---@type KeyState
    anyReasonableKeyState = nil,
    ---@type KeyState
    anyKeyState = nil,
    ---@type KeyState[]
    mouseState = nil,
    ---@type KeyState
    anyMouseState = nil,
}

--     m33 transformStack[TRANSFORM_STACK_SIZE];
--     u32 transformIndex;
--     lua["render"] = &gameState->render;

--     sol::usertype<Time> time_usertype = lua.new_usertype<Time>("time");
--     time_usertype["gameTime"] = &Time::gameTime;
--     time_usertype["deltaTime"] = &Time::deltaTime;
--     time_usertype["lastFrameGameTime"] = &Time::lastFrameGameTime;
--     time_usertype["gameFrames"] = &Time::gameFrames;
--     time_usertype["frames"] = &Time::frames;
--     lua["time"] = &gameState->time;

--     sol::usertype<Sound> sound_usertype = lua.new_usertype<Sound>("sound");
--     sound_usertype["bindingsEnabled"] = &Sound::bindingsEnabled;
--     lua["sound"] = &gameState->sound;
        
--     sol::usertype<Input> input_usertype = lua.new_usertype<Input>("input");
--     input_usertype["mouseTextureID"] = &Input::mouseTextureID;
--     input_usertype["mousePosition"] = &Input::mousePosition;
--     input_usertype["mouseDeltaPosition"] = &Input::mouseDeltaPosition;
--     input_usertype["mouseScreenPosition"] = &Input::mouseScreenPosition;
--     input_usertype["mouseScreenDeltaPosition"] = &Input::mouseScreenDeltaPosition;
--     input_usertype["mouseWheel"] = &Input::mouseWheel;
--     input_usertype["textInputBuffer"] = sol::property([](Input &input) { return input.textInputBuffer->value; }, [](Input &input, char* value) { return *input.textInputBuffer = value; });
--     input_usertype["keyState"] = sol::property([](Input &input) { return &input.keyState; });
--     input_usertype["anyReasonableKeyState"] = &Input::anyReasonableKeyState;
--     input_usertype["anyKeyState"] = &Input::anyKeyState;
--     input_usertype["mouseState"] = sol::property([](Input &input) { return &input.mouseState; });
--     input_usertype["anyMouseState"] = &Input::anyMouseState;
--     lua["input"] = &gameState->input;

--     lua["SetCustomCursor"] = SetCustomCursor;
--     lua["DisableCustomCursor"] = DisableCustomCursor;

--     lua["KEY_COUNT"] = KEY_COUNT;
--     lua["MOUSE_COUNT"] = MOUSE_COUNT;
--     lua["TEXT_INPUT_BUFFER_COUNT"] = TEXT_INPUT_BUFFER_COUNT;

---@class KeyState

---@type KeyState
KEY_UP = 0
---@type KeyState
KEY_RELEASED = 0
---@type KeyState
KEY_PRESSED = 0
---@type KeyState
KEY_DOWN = 0

--     // #NOTE (Juan): Temporal memory
--     lua["RenderTemporaryPushFloat"] = RenderTemporaryPushFloat;
--     lua["RenderTemporaryPushVector2"] = RenderTemporaryPushVector2;

--     // #NOTE (Juan): Input
--     lua["MouseOverRectangle"] = MouseOverRectangle;
--     lua["ClickOverRectangle"] = ClickOverRectangle;
--     lua["ClickedOverRectangle"] = ClickedOverRectangle;
--     lua["GetClipboardText"] = SDL_GetClipboardText;
--     lua["SetClipboardText"] = SDL_SetClipboardText;

---@class TextureAdjustStyle

---@type TextureAdjustStyle
TextureAdjustStyle_Stretch = 0
---@type TextureAdjustStyle
TextureAdjustStyle_FitRatio = 0
---@type TextureAdjustStyle
TextureAdjustStyle_KeepRatioX = 0
---@type TextureAdjustStyle
TextureAdjustStyle_KeepRatioY = 0
    
--     lua["colorLocation"] = &colorLocation;
--     lua["mvpLocation"] = &mvpLocation;
    
--     lua["bufferSizeLocation"] = &bufferSizeLocation;
--     lua["scaledBufferSizeLocation"] = &scaledBufferSizeLocation;
--     lua["textureSizeLocation"] = &textureSizeLocation;
--     lua["dimensionsLocation"] = &dimensionsLocation;
--     lua["borderLocation"] = &borderLocation;
    
--     lua["timeLocation"] = &timeLocation;

---@param red number
---@param green number
---@param blue number
---@param alpha number
function DrawClear(red, green, blue, alpha) end

---@param filepath string
---@param filepathHovered string
---@param filepathDown string
---@param slice number
function DrawSetStyle(filepath, filepathHovered, filepathDown, slice) end

---@param red number
---@param green number
---@param blue number
---@param alpha number
function DrawColor(red, green, blue, alpha) end

function DrawDefaultTransparent() end

---@param modeRGB integer
---@param modeAlpha integer
---@param srcRGB integer
---@param dstRGB integer
---@param srcAlpha integer
---@param dstAlpha integer
function DrawTransparent(modeRGB, modeAlpha, srcRGB, dstRGB, srcAlpha, dstAlpha) end

function DrawTransparentDisable() end

---@param targetLayer integer
---@param transparent boolean
function DrawSetLayer(targetLayer, transparent) end

---@param posX number
---@param posY number
---@param scaleX number
---@param scaleY number
---@param angle number
function DrawSetTransform(posX, posY, scaleX, scaleY, angle) end

---@param posX number
---@param posY number
---@param scaleX number
---@param scaleY number
---@param angle number
function DrawPushTransform(posX, posY, scaleX, scaleY, angle) end

function DrawPopTransform() end

function DrawDefaultTransform() end

---@param width number
function DrawLineWidth(width) end

---@param startX number
---@param startY number
---@param endX number
---@param endY number
function DrawLine(startX, startY, endX, endY) end

---@param p1X number
---@param p1Y number
---@param p2X number
---@param p2Y number
---@param p3X number
---@param p3Y number
function DrawTriangle(p1X, p1Y, p2X, p2Y, p3X, p3Y) end

---@param posX number
---@param posY number
---@param sizeX number
---@param sizeY number
function DrawRectangle(posX, posY, sizeX, sizeY) end

---@param posX number
---@param posY number
---@param radius number
---@param segments integer
function DrawCircle(posX, posY, radius, segments) end

---@param instanceCount integer
---@param positions number[]
---@param radius number
---@param segments integer
function DrawInstancedCircles(instanceCount, positions, radius, segments) end

---@param wrapS integer
---@param wrapT integer
---@param minFilter integer
---@param magFilter integer
function DrawTextureParameters(wrapS, wrapT, minFilter, magFilter) end

---@param posX number
---@param posY number
---@param sizeX number
---@param sizeY number
---@param textureID integer
function DrawTexture(posX, posY, sizeX, sizeY, textureID) end

---@param posX number
---@param posY number
---@param filepath string
---@param renderFlags integer
function DrawImage(posX, posY, filepath, renderFlags) end

---@param posX number
---@param posY number
---@param uvX number
---@param uvY number
---@param uvEndX number
---@param uvEndY number
---@param filepath string
function DrawImageUV(posX, posY, uvX, uvY, uvEndX, uvEndY, filepath) end

---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param slice number
---@param filepath string
function DrawImage9Slice(posX, posY, endX, endY, slice, filepath) end

---@param posX number
---@param posY number
---@param filepath string
---@param atlasName string
---@param key string
function DrawAtlasSprite(posX, posY, filepath, atlasName, key) end

---@param fontID integer
function DrawSetFont(fontID) end

---@param posX number
---@param posY number
---@param singleChar string
function DrawChar(posX, posY, singleChar) end

---@param posX number
---@param posY number
---@param string string
---@param renderFlags integer
function DrawString(posX, posY, string, renderFlags) end

---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param string string
---@param renderFlags integer
function DrawStyledString(posX, posY, endX, endY, string, renderFlags) end

function ClearInputBuffer() end

---@return boolean
---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param baseText string
---@param maxSize integer
function DrawStringInput(posX, posY, endX, endY, baseText, maxSize) end

---@return boolean
---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param label string
function DrawButton(posX, posY, endX, endY, label) end

---@return integer
---@param posX number
---@param posY number
---@param endX number
---@param height number
---@param yPadding number
---@param options string
function DrawMultibutton(posX, posY, endX, height, yPadding, options) end

---@param locationID integer
---@param type integer
function DrawSetUniform(locationID, type) end

---@param programID integer
function DrawOverrideProgram(programID) end

function DrawDisableOverrideProgram() end

---@param vertices number[]
---@param count integer
function DrawOverrideVertices(vertices, count) end

function DrawDisableOverrideVertices() end

---@param indices integer[]
---@param count integer
function DrawOverrideIndices(indices, count) end

function DrawDisableOverrideIndices() end

---@return v4
---@param hex integer
function ColorHexRGBA(hex) end

---@return v4
---@param hex integer
function ColorHexRGB(hex) end
    
function GetSceneFilepath() end

---@param luaFilepath string
function LoadLUAScene(luaFilepath) end

ImageRenderFlag_Fit = 0;
ImageRenderFlag_KeepRatioX = 0;
ImageRenderFlag_KeepRatioY = 0;
ImageRenderFlag_NoMipMaps = 0;

TextRenderFlag_Left = 0;
TextRenderFlag_Center = 0;
TextRenderFlag_Right = 0;
TextRenderFlag_LetterWrap = 0;
TextRenderFlag_WordWrap = 0;

--     lua["PerspectiveProjection"] = PerspectiveProjection;
--     lua["OrtographicProjection"] = sol::resolve<m44(f32, f32, f32, f32)>(OrtographicProjection);
--     lua["Perlin2D"] = sol::resolve<f32(f32, f32)>(Perlin2D);
--     lua["Perlin2DInt"] = sol::resolve<f32(i32, i32)>(Perlin2D);
--     lua["Perlin2DOctaves"] = Perlin2DOctaves;

--     // #NOTE (Juan): GLRender
--     sol::usertype<TextureAsset> gltexture_usertype = lua.new_usertype<TextureAsset>("gltexture");
--     gltexture_usertype["textureID"] = &TextureAsset::textureID;
--     gltexture_usertype["width"] = &TextureAsset::width;
--     gltexture_usertype["height"] = &TextureAsset::height;
--     gltexture_usertype["channels"] = &TextureAsset::channels;

--     lua["LoadTextureID"] = BindTextureID;
--     lua["LoadSceneTexture"] = LoadSceneTextureFile;
--     lua["LoadPermanentTexture"] = LoadPermanentTexture;
--     lua["TextureSize"] = sol::resolve<std::tuple<f32, f32>(const char*)>(TextureSizeBinding);
--     // lua["TextureSizeID"] = sol::resolve<std::tuple<f32, f32>(u32)>(TextureSizeBinding);
--     lua["GetTextureID"] = GetTextureID;
--     lua["GenerateFont"] = sol::resolve<u32(const char*, f32, u32, u32)>(GenerateFont);
--     lua["DEFAULT_FONT_ATLAS_WIDTH"] = DEFAULT_FONT_ATLAS_WIDTH;
--     lua["DEFAULT_FONT_ATLAS_HEIGHT"] = DEFAULT_FONT_ATLAS_HEIGHT;
--     lua["GenerateBitmapFontStrip"] = GenerateBitmapFontStrip;
--     lua["CompileProgram"] = CompileProgram;
--     lua["CompileProgramPlatform"] = CompileProgramPlatform;
--     lua["GetUniformLocation"] = glGetUniformLocation;
--     lua["SetUniform1F"] = SetUniform1F;
--     lua["SetUniform2F"] = SetUniform2F;
--     lua["SetUniform3F"] = SetUniform3F;
--     lua["SetUniform4F"] = SetUniform4F;
--     lua["UniformType_Float"] = UniformType_Float;
--     lua["UniformType_Vector2"] = UniformType_Vector2;

--     // #NOTE (Juan): OpenGL
--     lua["GL_ZERO"] = GL_ZERO;
--     lua["GL_ONE"] = GL_ONE;

--     lua["GL_FUNC_ADD"] = GL_FUNC_ADD;
--     lua["GL_FUNC_SUBTRACT"] = GL_FUNC_SUBTRACT;
--     lua["GL_FUNC_REVERSE_SUBTRACT"] = GL_FUNC_REVERSE_SUBTRACT;
--     lua["GL_MIN"] = GL_MIN;
--     lua["GL_MAX"] = GL_MAX;

--     lua["GL_SRC_COLOR"] = GL_SRC_COLOR;
--     lua["GL_ONE_MINUS_SRC_COLOR"] = GL_ONE_MINUS_SRC_COLOR;
--     lua["GL_SRC_ALPHA"] = GL_SRC_ALPHA;
--     lua["GL_ONE_MINUS_SRC_ALPHA"] = GL_ONE_MINUS_SRC_ALPHA;
--     lua["GL_DST_ALPHA"] = GL_DST_ALPHA;
--     lua["GL_ONE_MINUS_DST_ALPHA"] = GL_ONE_MINUS_DST_ALPHA;
--     lua["GL_DST_COLOR"] = GL_DST_COLOR;
--     lua["GL_ONE_MINUS_DST_COLOR"] = GL_ONE_MINUS_DST_COLOR;

--     lua["GL_CONSTANT_COLOR"] = GL_CONSTANT_COLOR;
--     lua["GL_ONE_MINUS_CONSTANT_COLOR"] = GL_ONE_MINUS_CONSTANT_COLOR;
--     lua["GL_CONSTANT_ALPHA"] = GL_CONSTANT_ALPHA;
--     lua["GL_ONE_MINUS_CONSTANT_ALPHA"] = GL_ONE_MINUS_CONSTANT_ALPHA;
--     lua["GL_SRC_ALPHA_SATURATE"] = GL_SRC_ALPHA_SATURATE;
    
--     lua["GL_TEXTURE_2D"] = GL_TEXTURE_2D;
--     lua["GL_TEXTURE_2D_ARRAY"] = GL_TEXTURE_2D_ARRAY;
--     lua["GL_TEXTURE_3D"] = GL_TEXTURE_3D;

--     lua["GL_TEXTURE_WRAP_S"] = GL_TEXTURE_WRAP_S;
--     lua["GL_TEXTURE_WRAP_T"] = GL_TEXTURE_WRAP_T;
--     lua["GL_TEXTURE_WRAP_R"] = GL_TEXTURE_WRAP_R;
--     lua["GL_CLAMP_TO_EDGE"] = GL_CLAMP_TO_EDGE;;
--     lua["GL_MIRRORED_REPEAT"] = GL_MIRRORED_REPEAT;
--     lua["GL_REPEAT"] = GL_REPEAT;
    
--     lua["GL_TEXTURE_MIN_FILTER"] = GL_TEXTURE_MIN_FILTER;
--     lua["GL_TEXTURE_MAG_FILTER"] = GL_TEXTURE_MAG_FILTER;
--     lua["GL_NEAREST"] = GL_NEAREST;
--     lua["GL_LINEAR"] = GL_LINEAR;
--     lua["GL_NEAREST_MIPMAP_NEAREST"] = GL_NEAREST_MIPMAP_NEAREST;
--     lua["GL_LINEAR_MIPMAP_NEAREST"] = GL_LINEAR_MIPMAP_NEAREST;
--     lua["GL_NEAREST_MIPMAP_LINEAR"] = GL_NEAREST_MIPMAP_LINEAR;
--     lua["GL_LINEAR_MIPMAP_LINEAR"] = GL_LINEAR_MIPMAP_LINEAR;

--     lua["GL_TEXTURE_BASE_LEVEL"] = GL_TEXTURE_BASE_LEVEL;
--     lua["GL_TEXTURE_COMPARE_FUNC"] = GL_TEXTURE_COMPARE_FUNC;
--     lua["GL_TEXTURE_COMPARE_MODE"] = GL_TEXTURE_COMPARE_MODE;
--     lua["GL_TEXTURE_MIN_LOD"] = GL_TEXTURE_MIN_LOD;
--     lua["GL_TEXTURE_MAX_LOD"] = GL_TEXTURE_MAX_LOD;
--     lua["GL_TEXTURE_MAX_LEVEL"] = GL_TEXTURE_MAX_LEVEL;
--     lua["GL_TEXTURE_SWIZZLE_R"] = GL_TEXTURE_SWIZZLE_R;
--     lua["GL_TEXTURE_SWIZZLE_G"] = GL_TEXTURE_SWIZZLE_G;
--     lua["GL_TEXTURE_SWIZZLE_B"] = GL_TEXTURE_SWIZZLE_B;
--     lua["GL_TEXTURE_SWIZZLE_A"] = GL_TEXTURE_SWIZZLE_A;

-- #ifndef GL_PROFILE_GLES3    
--     lua["GL_SRC1_COLOR"] = GL_SRC1_COLOR;
--     lua["GL_ONE_MINUS_SRC1_COLOR"] = GL_ONE_MINUS_SRC1_COLOR;
--     lua["GL_SRC1_ALPHA"] = GL_SRC1_ALPHA;
--     lua["GL_ONE_MINUS_SRC1_ALPHA"] = GL_ONE_MINUS_SRC1_ALPHA;

--     lua["GL_TEXTURE_1D"] = GL_TEXTURE_1D;
--     lua["GL_TEXTURE_1D_ARRAY"] = GL_TEXTURE_1D_ARRAY;

--     lua["GL_TEXTURE_2D_MULTISAMPLE"] = GL_TEXTURE_2D_MULTISAMPLE;
--     lua["GL_TEXTURE_2D_MULTISAMPLE_ARRAY"] = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
--     lua["GL_TEXTURE_CUBE_MAP"] = GL_TEXTURE_CUBE_MAP;
--     lua["GL_TEXTURE_CUBE_MAP_ARRAY"] = GL_TEXTURE_CUBE_MAP_ARRAY;
--     lua["GL_TEXTURE_RECTANGLE"] = GL_TEXTURE_RECTANGLE;

--     lua["GL_CLAMP_TO_BORDER"] = GL_CLAMP_TO_BORDER;
--     lua["GL_MIRROR_CLAMP_TO_EDGE"] = GL_MIRROR_CLAMP_TO_EDGE;
    
--     lua["GL_DEPTH_STENCIL_TEXTURE_MODE"] = GL_DEPTH_STENCIL_TEXTURE_MODE;
--     lua["GL_TEXTURE_LOD_BIAS"] = GL_TEXTURE_LOD_BIAS;
-- #endif

--     // #NOTE (Juan): Sound
--     lua["PlaySound"] = SoundPlaySimple;
--     lua["SoundPlayLoop"] = PlaySound;
--     lua["SoundStop"] = SoundStop;
--     lua["SetMasterVolume"] = SetMasterVolume;
--     lua["dbToVolume"] = dbToVolume;
--     lua["volumeToDB"] = volumeToDB;

--     // #NOTE (Juan): Console
--     lua["LogConsole"] = LogConsole;
--     lua["LogConsoleError"] = LogConsoleError;
--     lua["LogConsoleCommand"] = LogConsoleCommand;

--     // #NOTE (Juan): Serialization
--     lua["SaveData"] = SaveData;
    
--     lua["SaveGetString"] = SaveGetString;
--     lua["SaveSetString"] = SaveSetString;
--     lua["SaveGetBool"] = SaveGetBool;
--     lua["SaveSetBool"] = SaveSetBool;
--     lua["SaveGetI32"] = SaveGetI32;
--     lua["SaveSetI32"] = SaveSetI32;
--     lua["SaveGetF32"] = SaveGetF32;
--     lua["SaveSetF32"] = SaveSetF32;
--     lua["SaveGetV2"] = SaveGetV2;
--     lua["SaveSetV2"] = SaveSetV2;

--     lua["EditorSaveGetString"] = EditorSaveGetString;
--     lua["EditorSaveSetString"] = EditorSaveSetString;
--     lua["EditorSaveGetBool"] = EditorSaveGetBool;
--     lua["EditorSaveSetBool"] = EditorSaveSetBool;
--     lua["EditorSaveGetI32"] = EditorSaveGetI32;
--     lua["EditorSaveSetI32"] = EditorSaveSetI32;
--     lua["EditorSaveGetF32"] = EditorSaveGetF32;
--     lua["EditorSaveSetF32"] = EditorSaveSetF32;
--     lua["EditorSaveGetV2"] = EditorSaveGetV2;
--     lua["EditorSaveSetV2"] = EditorSaveSetV2;
    
--     // #NOTE (Juan): Runtime
--     lua["RuntimeQuit"] = RuntimeQuit;

--     // #NOTE (Juan): Editor
-- #ifdef PLATFORM_EDITOR
--     lua["ChangeLogFlag"] = ChangeLogFlag_;

--     sol::usertype<ShaderDebuggerWindow> editorShaderDebugger_usertype = lua.new_usertype<ShaderDebuggerWindow>("editorShaderDebugger");
--     editorShaderDebugger_usertype["programIDChanged"] = &ShaderDebuggerWindow::programIDChanged;
--     editorShaderDebugger_usertype["programIndex"] = &ShaderDebuggerWindow::programIndex;
--     lua["editorShaderDebugger"] = &editorShaderDebugger;

--     lua["LogFlag_PERFORMANCE"] = LogFlag_PERFORMANCE;
--     lua["LogFlag_RENDER"] = LogFlag_RENDER;
--     lua["LogFlag_MEMORY"] = LogFlag_MEMORY;
--     lua["LogFlag_TEXTURE"] = LogFlag_TEXTURE;
--     lua["LogFlag_SOUND"] = LogFlag_SOUND;
--     lua["LogFlag_INPUT"] = LogFlag_INPUT;
--     lua["LogFlag_TIME"] = LogFlag_TIME;
--     lua["LogFlag_LUA"] = LogFlag_LUA;

--     lua["LogFlag_GAME"] = LogFlag_GAME;
--     lua["LogFlag_SCRIPTING"] = LogFlag_SCRIPTING;

--     lua["ImGuiCond_FirstUseEver"] = ImGuiCond_FirstUseEver;

--     lua["ImGuiSetNextWindowSize"] = ImGuiSetNextWindowSize;
--     lua["ImGuiSetNextWindowSizeConstraints"] = ImGuiSetNextWindowSizeConstraints;
--     lua["ImGuiBegin"] = ImGuiBegin;
--     lua["ImGuiEnd"] = ImGui::End;
--     lua["ImGuiSameLine"] = ImGui::SameLine;
--     lua["ImGuiSpacing"] = ImGui::Spacing;
--     lua["ImGuiSeparator"] = ImGui::Separator;
--     lua["ImGuiDummy"] = ImGuiDummy;
--     lua["ImGuiInputInt"] = ImGuiInputInt;
--     lua["ImGuiInputFloat"] = ImGuiInputFloat;
--     lua["ImGuiSmallButton"] = ImGui::SmallButton;
--     lua["ImGuiButton"] = ImGuiButton;
--     lua["ImGuiImage"] = ImGuiImage;
--     lua["ImGuiImageButton"] = ImGuiImageButton;
--     lua["ImGuiPushStyleColor"] = ImGuiPushStyleColor;
--     lua["ImGuiCol_Button"] = ImGuiCol_Button;
--     lua["ImGuiPopStyleColor"] = ImGui::PopStyleColor;
--     lua["ImGuiPushItemWidth"] = ImGui::PushItemWidth;
--     lua["ImGuiPopItemWidth"] = ImGui::PopItemWidth;
--     lua["ImGuiPushID"] = sol::resolve<void(const char*)>(ImGui::PushID);
--     lua["ImGuiPopID"] = ImGui::PopID;
--     lua["ImGuiBeginMenu"] = ImGui::BeginMenu;
--     lua["ImGuiEndMenu"] = ImGui::EndMenu;
--     lua["ImGuiCheckbox"] = ImGuiCheckbox;
--     lua["ImGuiTextUnformatted"] = ImGui::TextUnformatted;
--     lua["ImGuiInputText"] = ImGuiInputText;
--     lua["ImGuiGetWindowDrawList"] = ImGui::GetWindowDrawList;
--     lua["ImGuiGetCursorScreenPos"] = ImGuiGetCursorScreenPos;
--     lua["ImGuiAddImage"] = ImGuiAddImage;
-- #endif
-- }

---@class v2
v2 = {
    x = 0.0,
    y = 0.0,
    ---@type number[]
    e = nil,
}

---@return v2
---@param x number
---@param y number
function V2(x, y) end

---@class v3
v3 = {
    x = 0.0,
    y = 0.0,
    ---@type number[]
    e = nil,
}

---@return v3
---@param x number
---@param y number
---@param z number
function V3(x, y, z) end

---@class v4
v4 = {
    x = 0.0,
    y = 0.0,
    ---@type number[]
    e = nil,
}

---@return v4
---@param x number
---@param y number
---@param z number
---@param w number
function V4(x, y, z, w) end

---@param _00 number
---@param _01 number
---@param _10 number
---@param _11 number
function M22(_00, _01, _10, _11) end
function IdM22() end

---@class m33
m33 = {
    _00 = 0.0,
    _10 = 0.0,
    _20 = 0.0,
    _01 = 0.0,
    _11 = 0.0,
    _21 = 0.0,
    _02 = 0.0,
    _12 = 0.0,
    _22 = 0.0,
    ---@type number[]
    e = nil,
}

---@param _00 number
---@param _01 number
---@param _02 number
---@param _10 number
---@param _11 number
---@param _12 number
---@param _20 number
---@param _21 number
---@param _22 number
function M33(_00, _01, _02, _10, _11, _12, _20, _21, _22) end
function IdM33() end

---@class m44
m44 = {
    _00 = 0.0,
    _10 = 0.0,
    _20 = 0.0,
    _30 = 0.0,
    _01 = 0.0,
    _11 = 0.0,
    _21 = 0.0,
    _31 = 0.0,
    _02 = 0.0,
    _12 = 0.0,
    _22 = 0.0,
    _32 = 0.0,
    _03 = 0.0,
    _13 = 0.0,
    _23 = 0.0,
    _33 = 0.0,
    ---@type number[]
    e = nil,
}

---@param _00 number
---@param _01 number
---@param _02 number
---@param _03 number
---@param _10 number
---@param _11 number
---@param _12 number
---@param _13 number
---@param _20 number
---@param _21 number
---@param _22 number
---@param _23 number
---@param _30 number
---@param _31 number
---@param _32 number
---@param _33 number
function M44(_00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33) end
function IdM44() end

--     sol::usertype<rectangle2> rectangle2_usertype = lua.new_usertype<rectangle2>("rectangle2");
--     rectangle2_usertype["x"] = sol::property([](rectangle2 &r) { return r.x; }, [](rectangle2 &r, f32 f) { r.x = f; });
--     rectangle2_usertype["y"] = sol::property([](rectangle2 &r) { return r.y; }, [](rectangle2 &r, f32 f) { r.y = f; });
--     rectangle2_usertype["width"] = sol::property([](rectangle2 &r) { return r.width; }, [](rectangle2 &r, f32 f) { r.width = f; });
--     rectangle2_usertype["height"] = sol::property([](rectangle2 &r) { return r.height; }, [](rectangle2 &r, f32 f) { r.height = f; });

--     sol::usertype<transform2D> transform2D_usertype = lua.new_usertype<transform2D>("transform2D");
--     transform2D_usertype["position"] = &transform2D::position;
--     transform2D_usertype["scale"] = &transform2D::scale;
--     transform2D_usertype["angle"] = &transform2D::angle;

--     // #NOTE (Juan): GameMath

--     lua["Rectangle2"] = Rectangle2;
    
--     lua["M22"] = M22;
--     lua["IdM22"] = IdM22;
--     lua["M33"] = M33;
--     lua["IdM33"] = IdM33;
--     lua["M44"] = sol::resolve<m44(f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32, f32)>(M44);
--     lua["IdM44"] = IdM44;

--     lua["Transform2D"] = Transform2D;

--     // lua["LengthV2"] = sol::resolve<f32(v2)>(Length);
--     // lua["NormalizeV2"] = sol::resolve<v2(v2)>(Normalize);
--     // lua["LengthSqV2"] = sol::resolve<f32(v2)>(LengthSq);

--     // #NOTE (Juan): Math custom functionality
--     // lua["math"]["sign"] = Sign;
--     // lua["math"]["round"] = RoundToInt;
--     // lua["math"]["rotLeft"] = RotateLeft;
--     // lua["math"]["rotRight"] = RotateRight;
--     // lua["math"]["sqr"] = Square;
--     // lua["math"]["floorV2"] = sol::resolve<v2(v2)>(Floor);
--     lua["math"]["lerp"] = Lerp;
-- }