#include <windows.h>
#include <psapi.h>
#include <chrono>
#include <thread>

#include <string>

#define PLATFORM_WINDOWS

#include "CodeGen/FileMap.h"
#include "CodeGen/ShaderMap.h"
#include "CodeGen/WindowsConfigMap.h"

#define SHADER_PREFIX "shaders/core/"
#define SOURCE_TYPE const char* const

#include "GL3W/gl3w.c"
#define IMGUI_IMPL_OPENGL_LOADER_GL3W

#define INITLUASCRIPT WINDOWSCONFIG_INITLUASCRIPT

#include <SDL.h>

#define DEFAULT_MIN_FILTER GL_LINEAR_MIPMAP_LINEAR
#define DEFAULT_MAG_FILTER GL_LINEAR_MIPMAP_LINEAR
#define FRAMEBUFFER_DEFAULT_FILTER GL_LINEAR

#define STB_TRUETYPE_IMPLEMENTATION
#include "STB/stb_truetype.h"

#include "Game.h"
#include "PlatformCommon.h"

#define GAME_SLOW

i32 CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    i32 ShowCode)
{
    size_t permanentStorageSize = Megabytes(32);
    void* permanentStorage = malloc(permanentStorageSize);
    
#ifdef GAME_SLOW
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

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

    DeserializeDataTable(&initialConfig, DATA_WINDOWSCONFIG_ENVT);

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

    GL_Init();

    CreateFramebuffer();
    
    GL_DefaultAssets();

#ifdef LUA_ENABLED
    ScriptingInit();
#endif
    
    DeserializeTable(&permanentState->arena, &saveData, "saveData.save");
    
    GameInit();

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

        ScriptingUpdate();
        GameUpdate();

        GL_Render();

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

    SerializeTable(&saveData, "saveData.save");

    return 0;
}