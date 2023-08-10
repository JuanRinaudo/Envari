#include <chrono>
#include <thread>
#include <vector>

#include <string>

#define PLATFORM_ANDROID 1
#define GAME_RELEASE 1
#define LUA_ENABLED 1

#define SOL_NO_EXCEPTIONS 1

#define APPNAME "MyApp"
#include <android/log.h>
#define ALOG(fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, fmt, ##__VA_ARGS__);

#include "OptickDummy.h"

#include "../../data/codegen/FileMap.h"
#include "../../data/codegen/ShaderMap.h"
#include "../../data/codegen/AndroidConfigMap.h"

#define SHADER_PREFIX "shaders/es/"
#define SOURCE_TYPE const char* const

#define INITLUASCRIPT ANDROIDCONFIG_INITLUASCRIPT

#include <SDL.h>

#include <GLES3/gl3.h>

#define GL_PROFILE_GLES3

#ifdef LUA_ENABLED
#include <LUAScriptingBindings.cpp>
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "STB/stb_truetype.h"

#include "Game.h"
#include "PlatformCommon.h"

i32 main(i32 argc, char *argv[])
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

    // SetupEnviroment();
    ALOG("%s", SDL_AndroidGetExternalStoragePath());
    std::string workingDirectory(SDL_AndroidGetExternalStoragePath());
    TryCreateDataFolderStructure(workingDirectory);
    filesystem::current_path(workingDirectory);
    CreateDirectoryIfNotExists("temp");
    CreateDirectoryIfNotExists("save");

    DeserializeDataTable(&permanentState->arena, &initialConfig, DATA_ANDROIDCONFIG_ENVT);

    InitEngine();

    if(!InitSDL()) {
        return -1;
    }

    v2 windowSize = V2(640, 640);
    gameState->render.width = FloorToInt(windowSize.x);
    gameState->render.height = FloorToInt(windowSize.y);
    gameState->render.refreshRate = 60;

    const char* windowTitle = "Test";
    // char* windowTitle = TableGetString(&initialConfig, ANDROIDCONFIG_WINDOWTITLE);
    sdlWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gameState->render.width, gameState->render.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!sdlWindow) {
    if(!SetupWindow()) {
        return -1;
    }

    if(!SetupTime()) {
        return -1;
    }

    InitGL();

    CreateFramebuffer();

#ifdef LUA_ENABLED
    LUAScriptingInit();
#endif

   InitGL();

#ifdef LUA_ENABLED
    ScriptingInit();
#endif
    
    DeserializeTable(&permanentState->arena, &saveData, GetSavePath());
    
    GameInit();
    
    DefaultAssets();

    SoundInit();

    gameState->game.running = true;
    while (gameState->game.running)
    {
        f32 gameTime = SDL_GetTicks() / 1000.0f;
        gameState->time.gameTime = (f32)gameTime;
        gameState->time.deltaTime = (f32)(gameTime - gameState->time.realTime);
        gameState->time.realTime = startTime;
        gameState->time.frames++;
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

        glViewport(0,0, gameState->render.width, gameState->render.height);
        glClearColor(1, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
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