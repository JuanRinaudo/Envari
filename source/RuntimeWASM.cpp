#include <emscripten.h>

#include <chrono>
#include <thread>

#include <string>

#define PLATFORM_WASM

#include "CodeGen/FileMap.h"
#include "CodeGen/ShaderMap.h"
#include "CodeGen/WasmConfigMap.h"

#define SHADER_PREFIX "shaders/es/"
#define SOURCE_TYPE const char* const

#define INITLUASCRIPT WASMCONFIG_INITLUASCRIPT

#include <SDL.h>

#define DEFAULT_MIN_FILTER GL_LINEAR_MIPMAP_LINEAR
#define DEFAULT_MAG_FILTER GL_LINEAR
#define FRAMEBUFFER_DEFAULT_FILTER GL_LINEAR

#define STB_TRUETYPE_IMPLEMENTATION
#include "STB/stb_truetype.h"

#include <GLES3/gl3.h>

#define GL_PROFILE_GLES3
#include "Game.h"
#include "PlatformCommon.h"

#ifdef LUA_ENABLED
#include "LUAScriptingBindings.cpp"
#endif

static void main_loop();

i32 main(i32 argc, char** argv)
{
    size_t permanentStorageSize = Megabytes(32);
    void* permanentStorage = malloc(permanentStorageSize);

    gameState = (Data *)permanentStorage;
    gameState->memory.permanentStorageSize = permanentStorageSize;
    gameState->memory.permanentStorage = permanentStorage;
    gameState->memory.sceneStorageSize = Megabytes(32);
    gameState->memory.sceneStorage = malloc(gameState->memory.sceneStorageSize);
    gameState->memory.temporalStorageSize = Megabytes(64);
    gameState->memory.temporalStorage = malloc(gameState->memory.temporalStorageSize);

    permanentState = (PermanentData *)gameState->memory.permanentStorage + sizeof(Data);
    sceneState = (SceneData *)gameState->memory.sceneStorage;
    temporalState = (TemporalData *)gameState->memory.temporalStorage;

    InitializeArena(&permanentState->arena, gameState->memory.permanentStorageSize, (u8 *)gameState->memory.permanentStorage, sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, gameState->memory.sceneStorageSize, (u8 *)gameState->memory.sceneStorage, sizeof(SceneData));
    InitializeArena(&temporalState->arena, gameState->memory.temporalStorageSize, (u8 *)gameState->memory.temporalStorage, sizeof(TemporalData));

    DeserializeDataTable(&initialConfig, DATA_WASMCONFIG_ENVT);

    if(!InitSDL()) {
        return -1;
    }

    if(!SetupWindow()) {
        return -1;
    }

    emscripten_set_main_loop(main_loop, 0, false);

    if(!SetupTime()) {
        return -1;
    }

    Init();

    CreateFramebuffer();
    
    DefaultAssets();

#ifdef LUA_ENABLED
    ScriptingInit();
#endif
    
    // DeserializeTable(&permanentState->arena, &saveData, "saveData.save");

    GameInit();

    SoundInit();

    SDL_ShowCursor(gameState->input.mouseTextureID == 0);

    gameState->game.running = true;

    return 0;
}

static void main_loop()
{
    if(gameState->game.running) {
        TimeTick();
        
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            ProcessEvent(&event);
        }

        CommonBegin2D();

        ScriptingUpdate();
        GameUpdate();

        RenderPass();

        End2D();

        if(gameState->render.framebufferEnabled) {
            RenderFramebuffer();
        }
        
        SDL_GL_SwapWindow(sdlWindow);

        CheckInput();

        WaitFPSLimit();
    }
}

// static void main_end()
// {
//     GameEnd();

//     // SaveConfig();

//     // SerializeTable(&saveData, "saveData.save");
// }