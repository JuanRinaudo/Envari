#include <emscripten.h>

#include <chrono>
#include <thread>
#include <string>

#include <OptickDummy.h>

#define SHADER_PREFIX "shaders/es/"
#define SOURCE_TYPE const char* const

#define INITLUASCRIPT WASMCONFIG_INITLUASCRIPT

#include <SDL.h>

#include <GLES3/gl3.h>

#define GL_PROFILE_GLES3
#include <WASMDefines.h>
#include <Game.h>

static void main_loop();

extern "C" {
    i32 main(i32 argc, char** argv);
    void main_loaded();
    void main_save();
    void main_end();
}

#include <PlatformCommon.h>

i32 main(i32 argc, char** argv)
{
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

    DeserializeDataTable(&permanentState->arena, &initialConfig, CONFIG_WASMCONFIG_ENVT);

    InitEngine();

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

    InitGL();

    CreateFramebuffer();
    
    DefaultAssets();

#ifdef LUA_ENABLED
    ScriptingInit();
#endif

#if defined(__linux__)
    // #TODO(Juan): Fix this on emscripten Linux WASM compilation, not working for some reason
    EM_ASM(
        FS.mkdir('/save');
        FS.mount(IDBFS, {}, '/save');
        FS.syncfs(true, function (err) {
            ccall('main_loaded', 'v');
        });
    );
#endif

    GameInit();

    SoundInit();

    SDL_ShowCursor(gameState->input.mouseTextureID == 0);

    gameState->game.running = true;

    return 0;
}

void main_loaded()
{
    DeserializeTable(&permanentState->arena, &saveData, GetSavePath());
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
}

void main_save()
{
    EM_ASM(
        FS.syncfs(function (err) { });
    );
}

void main_end()
{
    GameEnd();
}