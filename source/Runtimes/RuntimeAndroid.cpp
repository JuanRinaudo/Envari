#include <chrono>
#include <thread>

#include <string>

#define SOURCE_TYPE const char* const

#define INITLUASCRIPT ANDROIDCONFIG_INITLUASCRIPT

#include <SDL.h>

#include <GLES3/gl3.h>

#define GL_PROFILE_GLES3
#include "Game.h"

#ifdef LUA_ENABLED
#include <LUAScriptingBindings.cpp>
#endif

i32 main(i32 argc, char *argv[])
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

    InitializeArena(&permanentState->arena, (size_t)(gameState->memory.permanentStorageSize - sizeof(PermanentData) - sizeof(Data)), (u8 *)gameState->memory.permanentStorage + sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, (size_t)(gameState->memory.sceneStorageSize - sizeof(SceneData)), (u8 *)gameState->memory.sceneStorage + sizeof(SceneData));
    InitializeArena(&temporalState->arena, (size_t)(gameState->memory.temporalStorageSize - sizeof(TemporalData)), (u8 *)gameState->memory.temporalStorage + sizeof(TemporalData));

    // stringAllocator = PushStruct(&permanentState->arena, StringAllocator);
    // InitializeStringAllocator(stringAllocator);

    SetupEnviroment();

//     DeserializeDataTable(&permanentState->arena, &initialConfig, CONFIG_ANDROIDCONFIG_ENVT);

    // InitEngine();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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
        return -1;
    }

    glContext = SDL_GL_CreateContext(sdlWindow);

    const char* glsl_version = 0;

#ifdef LUA_ENABLED
    LUAScriptingInit();
#endif

   InitGL();

    GameInit();

    SoundInit();

    gameState->game.running = true;
    auto start = std::chrono::steady_clock::now(); // #NOTE (Juan): Start timer for fps limit
    while (gameState->game.running)
    {
        f32 gameTime = SDL_GetTicks() / 1000.0f;
        gameState->time.gameTime = (f32)gameTime;
        gameState->time.deltaTime = (f32)(gameTime - gameState->time.realTime);
        gameState->time.realTime = startTime;
        gameState->time.frames++;

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    gameState->game.running = false;
                    break;
                }
                case SDL_WINDOWEVENT: // #NOTE (Juan): Window resize/orientation change
                {
                    if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        gameState->render.width = event.window.data1;
                        gameState->render.height = event.window.data2;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    if(mouseEnabled) {
                        gameState->input.mouseState[event.button.button] = KEY_PRESSED;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    if(mouseEnabled) {
                        gameState->input.mouseState[event.button.button] = KEY_RELEASED;
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    if(mouseEnabled) {
                        gameState->input.mouseScreenPosition.x = (f32)event.button.x;
                        gameState->input.mouseScreenPosition.y = (f32)event.button.y;

                        gameState->input.mousePosition = RenderToViewport(gameState->input.mouseScreenPosition.x, gameState->input.mouseScreenPosition.y, gameState->camera.size, gameState->camera.ratio);
                    }
                    break;
                }
                case SDL_MOUSEWHEEL:
                    if(mouseEnabled) {
                        gameState->input.mouseWheel += event.wheel.y;
                    }
                    break;
                case SDL_KEYDOWN: {
                    if(keyboardEnabled) {
                        gameState->input.keyState[event.key.keysym.scancode] = KEY_PRESSED;
                    }
                    break;
                }
                case SDL_KEYUP: {
                    if(keyboardEnabled) {
                        gameState->input.keyState[event.key.keysym.scancode] = KEY_RELEASED;
                    }
                    break;
                }
                default:
                    break;
            }
        }

#ifdef LUA_ENABLED
        LUAScriptingUpdate();
#endif
        EngineUpdate();

        glViewport(0,0, gameState->render.width, gameState->render.height);
        glClearColor(1, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        SDL_GL_SwapWindow(sdlWindow);

        CheckInput();
    }

    GameEnd();

    return 0;
}