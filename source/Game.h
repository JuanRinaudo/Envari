#ifndef GAME_H
#define GAME_H

#include "Miniaudio/miniaudio.h"
#include "ZSTD/zstddeclib.c"
#include "Defines.h"
#include "MemoryStructs.h"
#include "Memory.h"

#include "IMGUI/imgui.h"

#include <cstdlib>
#include <filesystem>

#include "MathStructs.h"
#include "GameStructs.h"

#define SOL_ALL_SAFETIES_ON 1

DataTable* initialConfig = NULL;

RenderState renderState;

Data *gameState;
PermanentData *permanentState;
SceneData *sceneState;
TemporalData *temporalState;
TemporaryMemory renderTemporaryMemory;
TemporaryMemory renderStepTemporaryMemory;
#ifdef GAME_EDITOR
EditorData *editorState;
#endif

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
#include "File.h"
#ifdef GAME_EDITOR
#include "EditorStructs.h"
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
#include "Scene.h"

#ifdef GAME_EDITOR
#include "Editor.cpp"
#endif

static u32 GameInit()
{
    gameState->game.version = 1;
    gameState->game.updateRunning = true;

    if(gameState->render.bufferSize.x > 0) {
        gameState->camera.size = gameState->render.bufferSize.y;
        gameState->camera.ratio = gameState->render.bufferSize.x / gameState->render.bufferSize.y;
    }
    else {
        gameState->camera.size = gameState->render.size.y;
        gameState->camera.ratio = gameState->render.size.x / gameState->render.size.y;
    }
    gameState->camera.nearPlane = 0.01;
    gameState->camera.farPlane = 100.0;
    gameState->camera.view = IdM44();
    gameState->camera.projection = OrtographicProjection(gameState->camera.size, gameState->camera.ratio, gameState->camera.nearPlane, gameState->camera.farPlane);

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
    
    return 1;
}

static u32 ScriptingUpdate()
{
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

    return 1;
}

static u32 GameUpdate()
{
    if(gameState->game.updateRunning) {
        f32 fps = (f32)(1 / gameState->time.deltaTime);
    }

    if(gameState->input.mouseTextureID) {
        DrawColor(1, 1, 1, 1);
        DrawTexture(gameState->input.mousePosition.x, gameState->input.mousePosition.y, gameState->input.mouseTextureSize.x, gameState->input.mouseTextureSize.y, gameState->input.mouseTextureID);
    }

    return 1;
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

    return 1;
}

#endif