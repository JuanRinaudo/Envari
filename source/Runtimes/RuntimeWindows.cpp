#include <windows.h>
#include <psapi.h>
#include <chrono>
#include <thread>

#include <string>

#include <OptickDummy.h>

#if GAME_RELEASE
#define Assert(Expression)
#define AssertMessage(Expression, Message) 
#endif

#include "../../data/codegen/FileMap.h"
#include "../../data/codegen/ShaderMap.h"
#include "../../data/codegen/WindowsConfigMap.h"

#define SHADER_PREFIX "shaders/core/"

#include <gl3w.c>

#define INITLUASCRIPT WINDOWSCONFIG_INITLUASCRIPT

#include <SDL.h>

#include <Game.h>
#include <PlatformCommon.h>

#define GAME_SLOW

i32 CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    i32 ShowCode)
{    
#ifdef GAME_SLOW
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    size_t permanentStorageSize = Megabytes(32);
    void* permanentStorage = malloc(permanentStorageSize);

    gameState = (Data *)permanentStorage;
    gameState->memory.permanentStorageSize = permanentStorageSize;
    gameState->memory.permanentStorage = permanentStorage;
    gameState->memory.sceneStorageSize = Megabytes(32);
    gameState->memory.sceneStorage = malloc(gameState->memory.sceneStorageSize);
    gameState->memory.temporalStorageSize = Megabytes(32);
    gameState->memory.temporalStorage = malloc(gameState->memory.temporalStorageSize);

    permanentState = (PermanentData *)gameState->memory.permanentStorage + sizeof(Data);
    sceneState = (SceneData *)gameState->memory.sceneStorage;
    temporalState = (TemporalData *)gameState->memory.temporalStorage;

    InitializeArena(&permanentState->arena, gameState->memory.permanentStorageSize, (u8 *)gameState->memory.permanentStorage, sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, gameState->memory.sceneStorageSize, (u8 *)gameState->memory.sceneStorage, sizeof(SceneData));
    InitializeArena(&temporalState->arena, gameState->memory.temporalStorageSize, (u8 *)gameState->memory.temporalStorage, sizeof(TemporalData));

    stringAllocator = PushStruct(&permanentState->arena, StringAllocator);
    InitializeStringAllocator(stringAllocator);

    SetupEnviroment();

    DeserializeDataTable(&permanentState->arena, &initialConfig, CONFIG_WINDOWSCONFIG_ENVT);

    InitEngine();

    if(!InitSDL()) {
        return -1;
    }

    if(!SetupWindow()) {
        return -1;
    }

	if (gl3wInit()) {
		return -1;
	}

    if(!SetupTime()) {
        return -1;
    }

    InitGL();

    CreateFramebuffer();
    
    DeserializeTable(&permanentState->arena, &saveData, GetSavePath());

#ifdef LUA_ENABLED
    LUAScriptingInit();
#endif
    
    GameInit();
    
    DefaultAssets();

    SoundInit();

    gameState->game.running = true;
    while (gameState->game.running)
    {
        TimeTick();

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            ProcessEvent(&event);
        }
    
        CommonShowCursor();

        CommonBegin2D();

#ifdef LUA_ENABLED
        LUAScriptingUpdate();
#endif
        EngineUpdate();

        RenderPass();

        End2D();

        if(gameState->render.framebufferEnabled) {
            RenderFramebuffer();
        }
        
        SDL_GL_SwapWindow(sdlWindow);

        CheckInput();

        WaitFPSLimit();
    }

#ifdef GAME_SLOW
    FreeConsole();
#endif

    GameEnd();

    SaveConfig();

    return 0;
}