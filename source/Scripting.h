#if !defined(SCRIPTING_H)
#define SCRIPTING_H

#include "Game.h"
#include "ScriptingTypes.h"
#include "ScriptingWrappers.h"

#include <string>

static char* scriptDataPath;

#if GAME_INTERNAL
static char watchList[200];
static std::filesystem::file_time_type watchListTimes[20];
static u32 watchListSize = 0;
static u32 watchFiles = 0;
#endif

static void LoadScriptFile(char* name)
{

    char completePath[100]; // #TODO (Juan): Max file path is 100 chars for now, should be calculated and not have a limit
    strcpy(completePath, scriptDataPath);
    strcat(completePath, name);
    sol::load_result loadResult = lua.load_file(completePath);

    if(loadResult.valid()) {
        sol::protected_function_result result = loadResult();
        if(result.valid()) {
        }
        else {
            sol::error luaError = loadResult;
            std::string errorReport = luaError.what();
        }
    }
    else {
        sol::error luaError = loadResult;
        std::string errorReport = luaError.what();
    }

    #if GAME_INTERNAL
    watchListTimes[watchFiles] = std::filesystem::last_write_time(completePath);
    watchFiles++;

    strcat(watchList, name);
    strcat(watchList, "@");
    watchListSize += (u32)strlen(name) + 1;
    #endif
}

static void ScriptingPanic(sol::optional<std::string> message)
{
	console.AddLogSimple(message.value().c_str());
}

static void LoadLUALibrary(sol::lib library)
{
    lua.open_libraries(library);

    // #NOTE (Juan): Add library extensions
    switch(library) {
        case sol::lib::math: {
            lua["math"]["sign"] = Sign;
            lua["math"]["round"] = RoundToInt;
            lua["math"]["rotLeft"] = RotateLeft;
            lua["math"]["rotRight"] = RotateRight;
            lua["math"]["sqr"] = Square;
            lua["math"]["floorV2"] = sol::resolve<v2(v2)>(Floor);;
            break;
        }
    }
}

static void ScriptingInit(char* dataPath)
{
    scriptDataPath = dataPath;

    lua = sol::state(sol::c_call<decltype(&ScriptingPanic), &ScriptingPanic>);

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
    lua["camera"] = lua.create_table();
    lua["camera"]["size"] = gameState->camera.size;
    lua["camera"]["ratio"] = gameState->camera.ratio;
    lua["camera"]["nearPlane"] = gameState->camera.nearPlane;
    lua["camera"]["farPlane"] = gameState->camera.farPlane;
    lua["ReloadCameraData"] = ReloadCameraData;

    lua["screen"] = lua.create_table();
    lua["screen"]["width"] = gameState->screen.width;
    lua["screen"]["height"] = gameState->screen.height;
    lua["screen"]["bufferWidth"] = gameState->screen.bufferWidth;
    lua["screen"]["bufferHeight"] = gameState->screen.bufferHeight;
    lua["screen"]["refreshRate"] = gameState->screen.refreshRate;

    lua["time"] = lua.create_table();
    lua["time"]["gameTime"] = gameState->time.gameTime;
    lua["time"]["deltaTime"] = gameState->time.deltaTime;
    lua["time"]["lastFrameGameTime"] = gameState->time.lastFrameGameTime;
    
    lua["input"] = lua.create_table();    
    lua["input"]["mousePosition"] = lua.create_table();
    lua["input"]["mousePosition"]["x"] = gameState->input.mousePosition.x;
    lua["input"]["mousePosition"]["y"] = gameState->input.mousePosition.y;
    lua["input"]["mouseScreenPosition"] = lua.create_table();
    lua["input"]["mouseScreenPosition"]["x"] = gameState->input.mouseScreenPosition.x;
    lua["input"]["mouseScreenPosition"]["y"] = gameState->input.mouseScreenPosition.y;
    lua["input"]["keyState"] = lua.create_table();
    lua["input"]["mouseState"] = lua.create_table();
    lua["KEY_UP"] = KEY_UP;
    lua["KEY_RELEASED"] = KEY_RELEASED;
    lua["KEY_PRESSED"] = KEY_PRESSED;
    lua["KEY_DOWN"] = KEY_DOWN;

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

    // #NOTE (Juan): Input
    lua["MouseOverRectangle"] = MouseOverRectangle;
    lua["ClickOnRectangle"] = ClickOnRectangle;

    // #NOTE (Juan): Render
    lua["PushRenderClear"] = PushRenderClear;
    lua["PushRenderColor"] = PushRenderColor;
    lua["PushRenderTriangle"] = PushRenderTriangle;
    lua["PushRenderRectangle"] = PushRenderRectangle;
    lua["PushRenderCircle"] = PushRenderCircle;
    lua["PushRenderImage"] = PushRenderImage;
    lua["PushRenderImageUV"] = PushRenderImageUV;
    lua["PushRenderAtlasSprite"] = PushRenderAtlasSprite;
    lua["PushRenderTransparent"] = PushRenderTransparent;
    lua["PushRenderTransparentDisable"] = PushRenderTransparentDisable;
    lua["PushRenderFont"] = PushRenderFont;
    lua["PushRenderChar"] = PushRenderChar;
    lua["PushRenderText"] = PushRenderText;

    lua["ScreenToViewport"] = ScreenToViewport;
    // lua["ViewportToScreen"] = ViewportToScreen;
    
    lua["IMAGE_ADAPTATIVE_FIT"] = IMAGE_ADAPTATIVE_FIT;
    lua["IMAGE_KEEP_RATIO_X"] = IMAGE_KEEP_RATIO_X;
    lua["IMAGE_KEEP_RATIO_Y"] = IMAGE_KEEP_RATIO_Y;

    lua["PerspectiveProjection"] = PerspectiveProjection;
    lua["OrtographicProjection"] = sol::resolve<m44(f32, f32, f32, f32)>(OrtographicProjection);

    // #NOTE (Juan): GLRender
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
    
    lua["GL_SRC1_COLOR"] = GL_SRC1_COLOR;
    lua["GL_ONE_MINUS_SRC1_COLOR"] = GL_ONE_MINUS_SRC1_COLOR;
    lua["GL_SRC1_ALPHA"] = GL_SRC1_ALPHA;
    lua["GL_ONE_MINUS_SRC1_ALPHA"] = GL_ONE_MINUS_SRC1_ALPHA;

    // #NOTE (Juan): IMGUI
    lua["ImguiBegin"] = ImguiBegin;
    lua["ImguiLabelText"] = ImGui::LabelText;
    lua["ImguiCheckbox"] = ImguiCheckbox;
    lua["ImguiButton"] = ImguiButton;
    lua["ImguiSliderFloat"] = ImGui::SliderFloat;
    lua["ImguiColorPicker4"] = ImGui::ColorPicker4;
    lua["ImguiEnd"] = ImGui::End;

    // #NOTE (Juan): Console
    lua["ConsoleAddLog"] = ConsoleAddLog;
}

static void ScriptingUpdate()
{
    lua["time"]["gameTime"] = gameState->time.gameTime;
    lua["time"]["deltaTime"] = gameState->time.deltaTime;
    lua["time"]["lastFrameGameTime"] = gameState->time.lastFrameGameTime;

    lua["input"]["mousePosition"]["x"] = gameState->input.mousePosition.x;
    lua["input"]["mousePosition"]["y"] = gameState->input.mousePosition.y;

    lua["input"]["mouseScreenPosition"]["x"] = gameState->input.mouseScreenPosition.x;
    lua["input"]["mouseScreenPosition"]["y"] = gameState->input.mouseScreenPosition.y;

    for(i32 key = 0; key < KEY_COUNT; ++key) {
        lua["input"]["keyState"][key + 1] = gameState->input.keyState[key];
    }

    for(i32 key = 0; key < MOUSE_COUNT; ++key) {
        lua["input"]["mouseState"][key + 1] = gameState->input.mouseState[key];
    }
}

#if GAME_INTERNAL
static void ReloadScriptIfChanged(char *name, i32 fileIndex) {
    char completePath[100]; // #TODO (Juan): Max file path is 100 chars for now, should be calculated and not have a limit
    strcpy(completePath, scriptDataPath);
    strcat(completePath, name);
    auto fileTime = std::filesystem::last_write_time(completePath);
    if(fileTime != watchListTimes[fileIndex]) {
        console.AddLog("Started to reload script %s", name);

        sol::load_result loadResult = lua.load_file(completePath);

        if(loadResult.valid()) {
            sol::protected_function_result result = loadResult();
            if(result.valid()) {

            }
            else {
                sol::error luaError = loadResult;
                std::string errorReport = luaError.what();
                console.AddLog("Scripting reload run error");
                console.AddLog(errorReport.c_str());
            }
        }
        else {
            sol::error luaError = loadResult;
            std::string errorReport = luaError.what();
            console.AddLog("Scripting reload file error");
            console.AddLog(errorReport.c_str());
        }

        watchListTimes[fileIndex] = fileTime;
    }
}
#endif

static void ScriptingWatchChanges()
{
    #if GAME_INTERNAL
    int nameIndex = 0;
    int fileIndex = 0;
    int watchIndex = 0;
    char name[100];
    while(watchIndex < watchListSize) {
        if(watchList[watchIndex] == '@') {
            name[nameIndex] = 0;
            ReloadScriptIfChanged(name, fileIndex);
            fileIndex++;
            nameIndex = -1;
        }
        else {
            name[nameIndex] = watchList[watchIndex];
        }
        nameIndex++;
        watchIndex++;
    }
    #endif
}

#endif