#ifndef GAME_H
#define GAME_H

#include "Defines.h"

#include "IMGUI/imgui.h"

#include <cstdlib>
#include <filesystem>
#include "STB/stb_truetype.h"

#include "EditorStructs.h"
#include "GameStructs.h"

#define SOL_ALL_SAFETIES_ON 1

DataTable* initialConfig = NULL;

u32 frameBuffer;
u32 renderBuffer;
u32 depthrenderbuffer;
RenderState renderState;

void *gameMemory;
Data *gameState;
PermanentData *permanentState;
TemporalData *temporalState;
TemporaryMemory renderTemporaryMemory;

// NOTE(Juan): Temp test data, should be deleatedG
b32 Running = false;
b32 FullScreen = false;

#ifdef LUA_SCRIPTING_ENABLED
#include "LUA/sol.hpp"
sol::state lua;
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "STB/stb_ds.h"

#include "IMGUI/imgui.h"

#include "GameMath.h"

#include "Memory.h"
#include "File.h"
#include "Editor.h"
#include "Data.h"
#include "Data.cpp"
#include "Sound.h"
#include "Render.h"
#include "GLRender.h"
#include "Input.h"
#ifdef LUA_SCRIPTING_ENABLED
#include "Scripting.h"
#endif
#include "Editor.cpp"

static u32 GameInit()
{
    EditorInit(&editorConsole);

#ifdef LUA_SCRIPTING_ENABLED
    LoadScriptFile(TableGetString(&initialConfig, WINDOWSCONFIG_INITLUASCRIPT));

    sol::protected_function Init(lua["Init"]);
    if(Init.valid()) {
        sol::protected_function_result result = Init();
        if (!result.valid()) {
            sol::error error = result;
		    std::string what = error.what();
            LogError(&editorConsole, "%s", what.c_str());
        }
    }
    else {
        LogError(&editorConsole, "Error on script 'Init', not valid");
    }
#endif
    
    return 0;
}

static u32 GameLoop()
{
    f32 fps = (f32)(1 / gameState->time.deltaTime);

    EditorDrawAllOpen();
    
#ifdef LUA_SCRIPTING_ENABLED
    sol::protected_function Update(lua["Update"]);
    if(Update.valid()) {
        sol::protected_function_result result = Update();
        if (!result.valid()) {
            sol::error error = result;
		    std::string what = error.what();
            LogError(&editorConsole, "%s", what.c_str());
        }
    }
    else {
        LogError(&editorConsole, "Error on script 'Update', not valid");
    }
#endif

    // ImGui::ShowDemoWindow();

    return 0;
}

static u32 GameEnd()
{
    return 0;
}

#endif