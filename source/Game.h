#ifndef GAME_H
#define GAME_H

#include "Miniaudio/miniaudio.h"
#include "ZSTD/zstddeclib.c"
#include "Defines.h"
#include "UTF8.h"
#include "MemoryStructs.h"
#include "Memory.h"

#include "IMGUI/imgui.h"

#include <cstdlib>
#include <filesystem>

#include "MathStructs.h"
#include "GameStructs.h"

DataTable* initialConfig = NULL;

RenderState *renderState;

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
SerializableTable* editorSave = 0;

#ifdef LUA_ENABLED
#define SOL_ALL_SAFETIES_ON 1

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
#ifdef LUA_ENABLED
#include "LUAScripting.h"
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

    renderState = PushStruct(&permanentState->arena, RenderState);
    ResetRenderState();

#ifdef LUA_ENABLED
    ChangeLogFlag(LogFlag_SCRIPTING);

    LoadLUAScene(TableGetString(&initialConfig, INITLUASCRIPT));
    RunLUAProtectedFunction(EditorInit)
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
    #ifdef LUA_ENABLED
    ChangeLogFlag(LogFlag_SCRIPTING);

    RunLUAProtectedFunction(End)
    #endif

    return 1;
}

#endif