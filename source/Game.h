#ifndef GAME_H
#define GAME_H

#include "Defines.h"

#include "IMGUI/imgui.h"

#include <cstdlib>
#include <filesystem>

#include "MathStructs.h"
#include "GameStructs.h"
#include "EditorStructs.h"

#define SOL_ALL_SAFETIES_ON 1

DataTable* initialConfig = NULL;

RenderState renderState;

Data *gameState;
PermanentData *permanentState;
SceneData *sceneState;
TemporalData *temporalState;
TemporaryMemory renderTemporaryMemory;

SerializableTable* configSave = 0;
SerializableTable* saveData = 0;

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
#ifdef GAME_EDITOR
#include "Editor.h"
#else
#include "Runtime.h"
#endif
#include "Data.h"
#include "Serialization.h"
#include "Data.cpp"
#include "Sound.h"
#include "Input.h"
#include "Render.h"
#include "GLRender.h"
#ifdef LUA_SCRIPTING_ENABLED
#include "Scripting.h"
#endif

#ifdef GAME_EDITOR
#include "Editor.cpp"
#endif

static u32 GameInit()
{
    gameState->game.updateRunning = true;

#ifdef LUA_SCRIPTING_ENABLED
    LoadScriptFile(TableGetString(&initialConfig, INITLUASCRIPT));

    sol::protected_function Init(lua["Init"]);
    if(Init.valid()) {
        sol::protected_function_result result = Init();
        if (!result.valid()) {
            sol::error error = result;
		    std::string what = error.what();
            LogError("%s", what.c_str());
        }
    }
    else {
        LogError("Error on script 'Init', not valid");
    }
#endif
    
    return 0;
}

static u32 GameLoop()
{
    if(gameState->game.updateRunning) {
        f32 fps = (f32)(1 / gameState->time.deltaTime);
    
    #ifdef LUA_SCRIPTING_ENABLED
        sol::protected_function Update(lua["Update"]);
        if(Update.valid()) {
            sol::protected_function_result result = Update();
            if (!result.valid()) {
                sol::error error = result;
                std::string what = error.what();
                LogError("%s", what.c_str());
            }
        }
        else {
            LogError("Error on script 'Update', not valid");
        }
    #endif
    }

    return 0;
}

static u32 GameEnd()
{
    #ifdef LUA_SCRIPTING_ENABLED
        sol::protected_function End(lua["End"]);
        if(End.valid()) {
            sol::protected_function_result result = End();
            if (!result.valid()) {
                sol::error error = result;
                std::string what = error.what();
                LogError("%s", what.c_str());
            }
        }
        else {
            LogError("Error on script 'End', not valid");
        }
    #endif

    return 0;
}

#endif