#include <assert.h>
#include <chrono>
#include <thread>
#include <string>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "OptickDummy.h"

#if GAME_RELEASE
#define Assert(Expression) 
#define AssertMessage(Expression, Message) 
#endif

#include "CodeGen/FileMap.h"
#include "CodeGen/ShaderMap.h"
#include "CodeGen/LinuxConfigMap.h"

#define SHADER_PREFIX "shaders/core/"
#define SOURCE_TYPE const char* const

#include <SDL.h>

#include "GL3W/gl3w.c"

#define INITLUASCRIPT LINUXCONFIG_INITLUASCRIPT

#define STB_TRUETYPE_IMPLEMENTATION
#include "STB/stb_truetype.h"

#include "Game.h"
#include "PlatformCommon.h"

i32 main(i32 argc, char** argv)
{
    size_t permanentStorageSize = Megabytes(64);
    void* permanentStorage = malloc(permanentStorageSize);

    permanentState = (PermanentData *)permanentStorage;

    gameState = (Data *)(permanentState + 1);
    gameState->memory.permanentStorageSize = permanentStorageSize;
    gameState->memory.permanentStorage = permanentStorage;
    gameState->memory.sceneStorageSize = Megabytes(64);
    gameState->memory.sceneStorage = malloc(gameState->memory.sceneStorageSize);
    gameState->memory.temporalStorageSize = Megabytes(64);
    gameState->memory.temporalStorage = malloc(gameState->memory.temporalStorageSize);

    gameState->memory.permanentStorage = permanentStorage;
    sceneState = (SceneData *)gameState->memory.sceneStorage;
    temporalState = (TemporalData *)gameState->memory.temporalStorage;

    InitializeArena(&permanentState->arena, gameState->memory.permanentStorageSize, (u8 *)gameState->memory.permanentStorage, sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, gameState->memory.sceneStorageSize, (u8 *)gameState->memory.sceneStorage, sizeof(SceneData));
    InitializeArena(&temporalState->arena, gameState->memory.temporalStorageSize, (u8 *)gameState->memory.temporalStorage, sizeof(TemporalData));

    stringAllocator = PushStruct(&permanentState->arena, StringAllocator);
    InitializeStringAllocator(stringAllocator);

    SetupEnviroment();

    DeserializeDataTable(&permanentState->arena, &initialConfig, DATA_EDITORWINDOWSCONFIG_ENVT);

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
    
    DefaultAssets();

    DeserializeTable(&permanentState->arena, &saveData, GetSavePath());

#ifdef LUA_ENABLED
    ScriptingInit();
#endif
    
    GameInit();

    SoundInit();

    int frameCount = 0;
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
        ScriptingUpdate();
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
    
    GameEnd();

    SaveConfig();

    return 0;
}