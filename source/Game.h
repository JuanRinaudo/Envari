#ifndef GAME_H
#define GAME_H

#include <cstdlib>

#include "Miniaudio/miniaudio.h"
// #include "ZSTD/zstddeclib.c"
#include "Defines.h"
#include "Constants.h"
#include "Templates.h"
#include "UTF8.h"
#include "MemoryStructs.h"
#include "Memory.h"

#ifdef PLATFORM_EDITOR
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_customs.h"
#endif

#include "MathStructs.h"
#include "GameStructs.h"

DataTable* initialConfig = NULL;

RenderState *renderState;

Data *gameState;
StringAllocator *stringAllocator;
PermanentData *permanentState;
SceneData *sceneState;
TemporalData *temporalState;
TemporaryMemory renderTemporaryMemory;
TemporaryMemory renderStepTemporaryMemory;
#ifdef PLATFORM_EDITOR
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
#ifdef PLATFORM_EDITOR
#include "EditorStructs.h"
#include "Editor.h"
#include "Build.h"
#else
#include "Runtime.h"
#endif
#include "Data.h"
#include "Serialization.h"
#include "Sound.h"
#include "Input.h"
#include "Render.h"
#include "GLRender.h"
#ifdef LUA_ENABLED
#include "LUAScripting.h"
#endif
#include "Scene.h"

void SaveData();

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

    gameState->input.textInputBuffer = AllocateDynamicString(stringAllocator, "", TEXT_INPUT_BUFFER_COUNT);

    renderState = PushStruct(&permanentState->arena, RenderState);
    ResetRenderState();

#ifdef LUA_ENABLED
    ChangeLogFlag(LogFlag_SCRIPTING);

    LoadLUAScene(TableGetString(&initialConfig, INITLUASCRIPT));
    RunLUAProtectedFunction(EditorInit)
#endif
    
    return 1;
}

bool cppDemoVersion = false;

auto batchPositions = std::vector<v2>{};
auto batchColors = std::vector<v4>{};

f32 divider = 10;
f32 radius = 30 / divider;
f32 colorOffset = 0.1f;
f32 ySpeed = 3;
f32 yOffset = 8;
f32 perLineOffset = 0.55f;
v2 distance = V2(20 / divider, 25 / divider);

v2i count = V2I(-1, -1);

u32 lastCircleCount = 0;

static u32 GameUpdate()
{
    if(gameState->game.updateRunning) {
        f32 fps = (f32)(1 / gameState->time.deltaTime);
    }

    if(gameState->input.mouseTextureID) {
        DrawColor(1, 1, 1, 1);
        DrawTexture(gameState->input.mousePosition.x, gameState->input.mousePosition.y, gameState->input.mouseTextureSize.x, gameState->input.mouseTextureSize.y, gameState->input.mouseTextureID);
    }

    // DrawSetLayer(0, false);
    // DrawSetTransform();

    // DrawClear(0, 0, 0, 1);
    // DrawColor(1, 0, 0, 1);

    // f32 t = gameState->time.gameTime;
    // f32 st = Sin(t);
    // f32 sst = st * 0.5f + 0.5f;
    // f32 ast = Abs(Sin(t));
    // f32 mt = Mod(t, 1);

    // if(count.x < 0 || count.y < 0) {
    //     count = V2I(FloorToInt(gameState->render.bufferSize.x / distance.x), FloorToInt(gameState->render.bufferSize.y / distance.y));
    // }
    
    // u32 circleCount = count.x * count.y;
    // if(circleCount != lastCircleCount) {
    //     Log("Circle count: %d", circleCount);
    // }
    // lastCircleCount = circleCount;

    // batchPositions.resize(circleCount);
    // batchColors.resize(circleCount);

    // u32 k = 0;
    // for(u32 j = 0; j < count.y; ++j) {
    //     for(u32 i = 0; i < count.x; ++i) {
    //         f32 colorB = Sin(i * 0.2f + t * 0.5f + j * perLineOffset) * 0.5f + 0.5f;
    //         // DrawColor(0, colorB, 0, 1);
    //         // DrawCircle(0 + i * distance.x, j * distance.y + Sin(i + t * ySpeed) * yOffset, radius, 8);
    //         batchPositions[k] = V2(0 + i * distance.x, j * distance.y + Sin(i + t * ySpeed) * yOffset);
    //         batchColors[k] = V4(0, colorB, 1, 1);
    //         ++k;
    //     }
    // }

    // DrawInstancedCirclesColored(batchPositions, batchColors, radius, 8);
    // DrawInstancedCircles(batchPositions, radius, 8);

    return 1;
}

static u32 GameEnd()
{
    SaveData();

    #ifdef LUA_ENABLED
    ChangeLogFlag(LogFlag_SCRIPTING);

    RunLUAProtectedFunction(End)
    #endif

    return 1;
}

#endif