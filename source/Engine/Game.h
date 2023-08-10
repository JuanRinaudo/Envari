#ifndef GAME_H
#define GAME_H

#include <vector>
#include <cstdlib>

#include <stb_truetype.h>
#include "Defines.h"
#include "Templates.h"
#include "MemoryStructs.h"
#include "Memory.h"

#include <miniaudio.h>
#include <zstddeclib.c>
#include <Defines.h>
#include <EditorConstants.h>
#include <Templates.h>
#include <UTF8.h>
#include <MemoryStructs.h>
#include <Memory.h>

#ifdef PLATFORM_EDITOR
#include <imgui.h>
#include <imgui_customs.h>
#endif

#include <MathStructs.h>
#include <GameStructs.h>

DataTable* initialConfig = NULL;

RenderState *renderState;

Data *gameState;
StringAllocator *stringAllocator;
PermanentData *permanentState;
SceneData *sceneState;
TemporalData *temporalState;
TemporaryMemory renderTemporaryMemory;
TemporaryMemory renderStepTemporaryMemory;

SerializableTable* configSave = 0;
SerializableTable* saveData = 0;
SerializableTable* editorSave = 0;

#ifdef LUA_ENABLED
#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 1
#include <sol.hpp>
sol::state lua;
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include <Utils.h>
#include <GameMath.h>
#include <File.h>
#include <ASCII85.h>
#include <Assets.h>
#ifdef PLATFORM_EDITOR
#include <EditorStructs.h>
#include <Editor.h>
#include <Build.h>
#else
#include <Runtime.h>
#endif
#include <Data.h>
#include <Serialization.h>
#include <Sound.h>
#include <Input.h>
#include <Render.h>
#include <GLRender.h>
#ifdef LUA_ENABLED
#include <LUAScripting.h>
#endif
#ifdef CSCRIPTING_ENABLED
#include <CScripting.h>
#endif
#include <Scene.h>

void SaveData();

static u32 GameInit()
{
    ChangeLogFlag(LogFlag_GAME);
    
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

    gameState->input.textInputBuffer = AllocateDynamicString(stringAllocator, "", TEXT_INPUT_BUFFER_COUNT);

    renderState = PushStruct(&permanentState->arena, RenderState);
    ResetRenderState();

#ifdef LUA_ENABLED
    ChangeLogFlag(LogFlag_LUA_SCRIPTING);

    LoadLUAScene(TableGetString(&initialConfig, CONFIG_INITLUASCRIPT));
    #ifdef PLATFORM_EDITOR
    RunLUAProtectedFunction(EditorInit)
    #endif
#endif
#ifdef CSCRIPTING_ENABLED
    CInit();
#endif
    
    return 1;
}

static u32 GameUpdate()
{
    ChangeLogFlag(LogFlag_GAME);

    if(gameState->game.sceneChanged) {
        RunLUAProtectedFunction(Start)
        gameState->game.sceneChanged = false;
    }
    
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
    SaveData();

#ifdef LUA_ENABLED
    ChangeLogFlag(LogFlag_LUA_SCRIPTING);

    RunLUAProtectedFunction(End)
#endif
#ifdef CSCRIPTING_ENABLED
    CEnd();
#endif

    return 1;
}

#endif