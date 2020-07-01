#if !defined(SCRIPTING_H)
#define SCRIPTING_H

#include "Render.h"
#include "Game.h"
#include "LUA/sol.hpp"
#include "ScriptingWrappers.h"

#include <string>

static char* scriptDataPath;

#if GAME_INTERNAL
#include <filesystem>

static char watchList[200];
static std::filesystem::file_time_type watchListTimes[20];
static i32 watchListSize = 0;
static i32 watchFiles = 0;
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
    watchListSize += strlen(name + 1);
    #endif
}

static void ScriptingPanic(sol::optional<std::string> message)
{
    
}

static void ScriptingInit(char* dataPath)
{

    scriptDataPath = dataPath;

    lua = sol::state(sol::c_call<decltype(&ScriptingPanic), &ScriptingPanic>);

    lua.open_libraries(sol::lib::base, sol::lib::package);

    // #NOTE (Juan): Data
    lua["screen"] = lua.create_table();
    lua["screen"]["width"] = (gameState->screen).width;
    lua["screen"]["height"] = (gameState->screen).height;
    lua["screen"]["refreshRate"] = (gameState->screen).refreshRate;

    // #NOTE (Juan): Render
    lua["V2"] = V2;
    lua["V3"] = V3;
    lua["V4"] = V4;

    lua["M22"] = M22;
    lua["IdM22"] = IdM22;
    lua["M33"] = M33;
    lua["IdM33"] = IdM33;
    lua["M44"] = M44;
    lua["IdM44"] = IdM44;
    lua["PushClear"] = PushClear;
    lua["PushColor"] = PushColor;
    lua["PushTriangle"] = PushTriangle;
    lua["PushRectangle"] = PushRectangle;
    lua["PushCircle"] = PushCircle;
    
    lua["PerspectiveProyection"] = PerspectiveProyection;
    lua["OrtographicProyection"] = sol::resolve<m44(f32, f32, f32, f32)>(OrtographicProyection);	

    // #NOTE (Juan): IMGUI
    lua["ImguiBegin"] = ImguiBegin;
    lua["ImguiLabelText"] = ImGui::LabelText;
    lua["ImguiCheckbox"] = ImguiCheckbox;
    lua["ImguiButton"] = ImguiButton;
    lua["ImguiSliderFloat"] = ImGui::SliderFloat;
    lua["ImguiColorPicker4"] = ImGui::ColorPicker4;
    lua["ImguiEnd"] = ImGui::End;

    LoadScriptFile("test.lua");
    LoadScriptFile("test2.lua");

}

static void ReloadScriptIfChanged(char *name, i32 fileIndex) {
    char completePath[100]; // #TODO (Juan): Max file path is 100 chars for now, should be calculated and not have a limit
    strcpy(completePath, scriptDataPath);
    strcat(completePath, name);
    auto fileTime = std::filesystem::last_write_time(completePath);
    if(fileTime != watchListTimes[fileIndex]) {
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

        watchListTimes[fileIndex] = fileTime;
    }
}

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