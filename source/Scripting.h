#if !defined(SCRIPTING_H)
#define SCRIPTING_H

#include "Game.h"
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
	AddLogSimple(&editorConsole, message.value().c_str());
}

static void LoadLUALibrary(sol::lib library)
{
    lua.open_libraries(library);

    // #NOTE (Juan): Add library extensions
    switch(library) {
        case sol::lib::math: {
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

            sol::usertype<transform2D> transform2D_usertype = lua.new_usertype<transform2D>("transform2D");
            transform2D_usertype["position"] = &transform2D::position;
            transform2D_usertype["scale"] = &transform2D::scale;
    
            // #NOTE (Juan): Math custom functionality
            lua["math"]["sign"] = Sign;
            lua["math"]["round"] = RoundToInt;
            lua["math"]["rotLeft"] = RotateLeft;
            lua["math"]["rotRight"] = RotateRight;
            lua["math"]["sqr"] = Square;
            lua["math"]["floorV2"] = sol::resolve<v2(v2)>(Floor);
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
    lua["input"]["mousePosition"] = gameState->input.mousePosition;
    lua["input"]["mouseScreenPosition"] = gameState->input.mouseScreenPosition;
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

    lua["Transform2D"] = Transform2D;

    // #NOTE (Juan): Input
    lua["MouseOverRectangle"] = MouseOverRectangle;
    lua["ClickOnRectangle"] = ClickOnRectangle;

    // #NOTE (Juan): Render
    lua["PushRenderClear"] = PushRenderClear;
    lua["PushRenderColor"] = PushRenderColor;
    lua["PushRenderTriangle"] = PushRenderTriangle;
    lua["PushRenderRectangle"] = PushRenderRectangle;
    lua["PushRenderCircle"] = PushRenderCircle;
    lua["PushRenderTextureParameters"] = PushRenderTextureParameters;
    lua["PushRenderImage"] = PushRenderImage;
    lua["PushRenderImageUV"] = PushRenderImageUV;
    lua["PushRenderAtlasSprite"] = PushRenderAtlasSprite;
    lua["PushRenderTransparent"] = PushRenderTransparent;
    lua["PushRenderTransparentDisable"] = PushRenderTransparentDisable;
    lua["PushRenderFont"] = PushRenderFont;
    lua["PushRenderChar"] = PushRenderChar;
    lua["PushRenderText"] = PushRenderText;
    lua["PushRenderOverrideVertices"] = PushRenderOverrideVertices;
    lua["PushRenderDisableOverrideVertices"] = PushRenderDisableOverrideVertices;
    lua["PushRenderOverrideIndices"] = PushRenderOverrideIndices;
    lua["PushRenderDisableOverrideIndices"] = PushRenderDisableOverrideIndices;
 

    lua["ScreenToViewport"] = ScreenToViewport;
    // lua["ViewportToScreen"] = ViewportToScreen;
    
    lua["IMAGE_ADAPTATIVE_FIT"] = IMAGE_ADAPTATIVE_FIT;
    lua["IMAGE_KEEP_RATIO_X"] = IMAGE_KEEP_RATIO_X;
    lua["IMAGE_KEEP_RATIO_Y"] = IMAGE_KEEP_RATIO_Y;

    lua["PerspectiveProjection"] = PerspectiveProjection;
    lua["OrtographicProjection"] = sol::resolve<m44(f32, f32, f32, f32)>(OrtographicProjection);

    // #NOTE (Juan): GLRender
    lua["CreateQuadPosUV"] = CreateQuadPosUV;

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
    
    lua["GL_SRC1_COLOR"] = GL_SRC1_COLOR;
    lua["GL_ONE_MINUS_SRC1_COLOR"] = GL_ONE_MINUS_SRC1_COLOR;
    lua["GL_SRC1_ALPHA"] = GL_SRC1_ALPHA;
    lua["GL_ONE_MINUS_SRC1_ALPHA"] = GL_ONE_MINUS_SRC1_ALPHA;
    
    lua["GL_TEXTURE_1D"] = GL_TEXTURE_1D;
    lua["GL_TEXTURE_1D_ARRAY"] = GL_TEXTURE_1D_ARRAY;
    lua["GL_TEXTURE_2D"] = GL_TEXTURE_2D;
    lua["GL_TEXTURE_2D_ARRAY"] = GL_TEXTURE_2D_ARRAY;
    lua["GL_TEXTURE_2D_MULTISAMPLE"] = GL_TEXTURE_2D_MULTISAMPLE;
    lua["GL_TEXTURE_2D_MULTISAMPLE_ARRAY"] = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    lua["GL_TEXTURE_3D"] = GL_TEXTURE_3D;
    lua["GL_TEXTURE_CUBE_MAP"] = GL_TEXTURE_CUBE_MAP;
    lua["GL_TEXTURE_CUBE_MAP_ARRAY"] = GL_TEXTURE_CUBE_MAP_ARRAY;
    lua["GL_TEXTURE_RECTANGLE"] = GL_TEXTURE_RECTANGLE;

    lua["GL_TEXTURE_WRAP_S"] = GL_TEXTURE_WRAP_S;
    lua["GL_TEXTURE_WRAP_T"] = GL_TEXTURE_WRAP_T;
    lua["GL_TEXTURE_WRAP_R"] = GL_TEXTURE_WRAP_R;
    lua["GL_CLAMP_TO_EDGE"] = GL_CLAMP_TO_EDGE;
    lua["GL_CLAMP_TO_BORDER"] = GL_CLAMP_TO_BORDER;
    lua["GL_MIRRORED_REPEAT"] = GL_MIRRORED_REPEAT;
    lua["GL_REPEAT"] = GL_REPEAT;
    lua["GL_MIRROR_CLAMP_TO_EDGE"] = GL_MIRROR_CLAMP_TO_EDGE;
    
    lua["GL_TEXTURE_MIN_FILTER"] = GL_TEXTURE_MIN_FILTER;
    lua["GL_TEXTURE_MAG_FILTER"] = GL_TEXTURE_MAG_FILTER;
    lua["GL_NEAREST"] = GL_NEAREST;
    lua["GL_LINEAR"] = GL_LINEAR;
    lua["GL_NEAREST_MIPMAP_NEAREST"] = GL_NEAREST_MIPMAP_NEAREST;
    lua["GL_LINEAR_MIPMAP_NEAREST"] = GL_LINEAR_MIPMAP_NEAREST;
    lua["GL_NEAREST_MIPMAP_LINEAR"] = GL_NEAREST_MIPMAP_LINEAR;
    lua["GL_LINEAR_MIPMAP_LINEAR"] = GL_LINEAR_MIPMAP_LINEAR;

    lua["GL_DEPTH_STENCIL_TEXTURE_MODE"] = GL_DEPTH_STENCIL_TEXTURE_MODE;
    lua["GL_TEXTURE_BASE_LEVEL"] = GL_TEXTURE_BASE_LEVEL;
    lua["GL_TEXTURE_COMPARE_FUNC"] = GL_TEXTURE_COMPARE_FUNC;
    lua["GL_TEXTURE_COMPARE_MODE"] = GL_TEXTURE_COMPARE_MODE;
    lua["GL_TEXTURE_LOD_BIAS"] = GL_TEXTURE_LOD_BIAS;
    lua["GL_TEXTURE_MIN_LOD"] = GL_TEXTURE_MIN_LOD;
    lua["GL_TEXTURE_MAX_LOD"] = GL_TEXTURE_MAX_LOD;
    lua["GL_TEXTURE_MAX_LEVEL"] = GL_TEXTURE_MAX_LEVEL;
    lua["GL_TEXTURE_SWIZZLE_R"] = GL_TEXTURE_SWIZZLE_R;
    lua["GL_TEXTURE_SWIZZLE_G"] = GL_TEXTURE_SWIZZLE_G;
    lua["GL_TEXTURE_SWIZZLE_B"] = GL_TEXTURE_SWIZZLE_B;
    lua["GL_TEXTURE_SWIZZLE_A"] = GL_TEXTURE_SWIZZLE_A;

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
        AddLog(&editorConsole, "Started to reload script %s", name);

        sol::load_result loadResult = lua.load_file(completePath);

        if(loadResult.valid()) {
            sol::protected_function_result result = loadResult();
            if(result.valid()) {

            }
            else {
                sol::error luaError = loadResult;
                std::string errorReport = luaError.what();
                AddLog(&editorConsole, "Scripting reload run error");
                AddLog(&editorConsole, errorReport.c_str());
            }
        }
        else {
            sol::error luaError = loadResult;
            std::string errorReport = luaError.what();
            AddLog(&editorConsole, "Scripting reload file error");
            AddLog(&editorConsole, errorReport.c_str());
        }

        watchListTimes[fileIndex] = fileTime;
    }
}
#endif

static void ScriptingWatchChanges()
{
    #if GAME_INTERNAL
    i32 nameIndex = 0;
    i32 fileIndex = 0;
    i32 watchIndex = 0;
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