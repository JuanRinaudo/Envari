#include <windows.h>
#include <psapi.h>
#include <chrono>
#include <thread>

#include <string>

#include "CodeGen/FileMap.h"
#include "CodeGen/ShaderMap.h"
#include "CodeGen/EditorConfigMap.h"

#define SHADER_PREFIX "shaders/core/"
#define SOURCE_TYPE const char* const

#include "GL3W/gl3w.c"
#define IMGUI_IMPL_OPENGL_LOADER_GL3W

#define INITLUASCRIPT EDITORCONFIG_INITLUASCRIPT

#include <SDL.h>

#include "IMGUI/imgui.cpp"

#include "STB/stb_truetype.h"
#include "Game.h"

#include "IMGUI/imgui_draw.cpp"
#include "IMGUI/imgui_widgets.cpp"

#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "IMGUI/imgui_impl_sdl.cpp"
#include "IMGUI/imgui_impl_opengl3.cpp"

SDL_Window* sdlWindow;
SDL_DisplayMode displayMode;
SDL_GLContext glContext;

static void CheckInput() {
    for(i32 key = 0; key < KEY_COUNT; ++key) {
        u8 keyState = gameState->input.keyState[key];
        if(keyState == KEY_PRESSED) { gameState->input.keyState[key] = KEY_DOWN; }
        else if(keyState == KEY_RELEASED) { gameState->input.keyState[key] = KEY_UP; }
    }

    for(i32 key = 0; key < MOUSE_COUNT; ++key) {
        u8 mouseState = gameState->input.mouseState[key];
        if(mouseState == KEY_PRESSED) { gameState->input.mouseState[key] = KEY_DOWN; }
        else if(mouseState == KEY_RELEASED) { gameState->input.mouseState[key] = KEY_UP; }
    }
}

i32 CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    i32 ShowCode)
{
    size_t permanentStorageSize = Megabytes(64);
    void* permanentStorage = malloc(permanentStorageSize);

    gameState = (Data *)permanentStorage;
    gameState->memory.permanentStorageSize = permanentStorageSize;
    gameState->memory.permanentStorage = permanentStorage;
    gameState->memory.sceneStorageSize = Megabytes(64);
    gameState->memory.sceneStorage = malloc(gameState->memory.sceneStorageSize);
    gameState->memory.temporalStorageSize = Megabytes(64);
    gameState->memory.temporalStorage = malloc(gameState->memory.temporalStorageSize);

    permanentState = (PermanentData *)gameState->memory.permanentStorage + sizeof(Data);
    sceneState = (SceneData *)gameState->memory.sceneStorage;
    temporalState = (TemporalData *)gameState->memory.temporalStorage;

    InitializeArena(&permanentState->arena, gameState->memory.permanentStorageSize, (u8 *)gameState->memory.permanentStorage, sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, gameState->memory.sceneStorageSize, (u8 *)gameState->memory.sceneStorage, sizeof(SceneData));
    InitializeArena(&temporalState->arena, gameState->memory.temporalStorageSize, (u8 *)gameState->memory.temporalStorage, sizeof(TemporalData));

    DeserializeDataTable(&initialConfig, DATA_EDITORCONFIG_ENVT);

    DeserializeTable(&permanentState->arena, &configSave, "config.save");

    // #TODO (Juan): Check this SDL_INIT_EVERYTHING, check what really needs to be init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GetCurrentDisplayMode(0, &displayMode);
    gameState->render.windowPosition = TableGetV2(&configSave, "windowPosition", V2(-1, -1));
    gameState->render.windowSize = TableGetV2(&configSave, "windowSize", TableGetV2(&initialConfig, EDITORCONFIG_WINDOWSIZE));

    if(gameState->render.windowSize.x <= 10 && gameState->render.windowSize.y <= 10) {
        gameState->render.size.x = displayMode.w * gameState->render.windowSize.x;
        gameState->render.size.y = displayMode.h * gameState->render.windowSize.y;
    }
    else {
        gameState->render.size.x = gameState->render.windowSize.x;
        gameState->render.size.y = gameState->render.windowSize.y;
    }
    gameState->render.windowSize.x = gameState->render.size.x;
    gameState->render.windowSize.y = gameState->render.size.y;

    gameState->render.framebufferEnabled = TableHasKey(initialConfig, EDITORCONFIG_BUFFERSIZE);
    if(gameState->render.framebufferEnabled) {
        v2 bufferSize = TableGetV2(&initialConfig, EDITORCONFIG_BUFFERSIZE);
        if(gameState->render.windowSize.x <= 10 && gameState->render.windowSize.y <= 10) {
            gameState->render.bufferSize.x = gameState->render.size.x * bufferSize.x;
            gameState->render.bufferSize.y = gameState->render.size.y * bufferSize.y;
        }
        else {
            gameState->render.bufferSize.x = bufferSize.x;
            gameState->render.bufferSize.y = bufferSize.y;
        }
    }
    else {
        gameState->render.bufferSize.x = -1;
        gameState->render.bufferSize.y = -1;
    }

    gameState->render.refreshRate = displayMode.refresh_rate;

    char* windowTitle = TableGetString(&initialConfig, EDITORCONFIG_WINDOWTITLE);
    sdlWindow = SDL_CreateWindow(windowTitle, gameState->render.windowPosition.x > 0 ? (i32)gameState->render.windowPosition.x : SDL_WINDOWPOS_UNDEFINED,
        gameState->render.windowPosition.y > 0 ? (i32)gameState->render.windowPosition.y : SDL_WINDOWPOS_UNDEFINED, 
        (i32)gameState->render.windowSize.x, (i32)gameState->render.windowSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!sdlWindow) {
        return -1;
    }

    glContext = SDL_GL_CreateContext(sdlWindow);

	if (gl3wInit()) {
		return -1;
	}

    i32 fpsLimit = TableGetInt(&initialConfig, EDITORCONFIG_FPSLIMIT);
    i32 fpsFixed = TableGetInt(&initialConfig, EDITORCONFIG_FPSFIXED);
    f32 fpsDelta = 1000.0f / fpsLimit;
    i32 vsync = TableGetInt(&initialConfig, EDITORCONFIG_VSYNC);
    SDL_GL_SetSwapInterval(vsync);

    const char* glsl_version = 0;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

#ifdef LUA_SCRIPTING_ENABLED
    ScriptingInit();
#endif

    GL_Init();

    // #NOTE (Juan): Create framebuffer
    if(gameState->render.framebufferEnabled) {
        GL_InitFramebuffer((i32)gameState->render.bufferSize.x, (i32)gameState->render.bufferSize.y);
    }
    else {
        gameState->render.frameBuffer = 0;
    }
    
    DeserializeTable(&permanentState->arena, &saveData, "saveData.save");

    EditorInit();

    gameState->time.gameFrames = -1;
    gameState->time.frames = -1;
    
    GameInit();

    SoundInit();

    HANDLE processHandle = GetCurrentProcess();

    editorRenderDebugger.recording = false;

    gameState->game.running = true;
    auto start = std::chrono::steady_clock::now(); // #NOTE (Juan): Start timer for fps limit
    while (gameState->game.running)
    {
        GetProcessMemoryInfo(processHandle , &editorPerformanceDebugger.memoryCounters, sizeof(PROCESS_MEMORY_COUNTERS));
        i64 updateCyclesStart = __rdtsc();

        u32 startTicks = 0;
        f32 startTime = 0;
        if(fpsFixed > 0)
        {
            gameState->time.deltaTime = 1.0f / fpsFixed;
            gameState->time.startTime += gameState->time.deltaTime;
            startTicks = (u32)(gameState->time.startTime * 1000);
            startTime = gameState->time.startTime;
        }
        else
        {
            startTicks = SDL_GetTicks();
            startTime = startTicks / 1000.0f;
            gameState->time.startTime = startTime;
            gameState->time.deltaTime = startTime - gameState->time.lastFrameGameTime;
        }

        gameState->time.frames++;

        // #NOTE(Juan): Do a fps limit if enabled
        std::chrono::steady_clock::time_point end;
        if(fpsLimit > 0) {
            auto now = std::chrono::steady_clock::now();
            i64 epochTime = now.time_since_epoch().count();
            i64 deltaFPSNanoseconds = (u32)(fpsDelta * 1000000);
            end = now + std::chrono::nanoseconds(deltaFPSNanoseconds - epochTime % deltaFPSNanoseconds);

            auto diff = now - start;
            if(diff >= std::chrono::seconds(1))
            {
                start = now;
            }
        }

        ImGuiIO imguiIO = ImGui::GetIO();
        bool mouseOverPreview = editorPreview.open && editorPreview.cursorInsideWindow;
        bool mouseEnabled = !imguiIO.WantCaptureMouse && !editorPreview.open;
        bool keyboardEnabled = !imguiIO.WantCaptureKeyboard;

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_QUIT: {
                    gameState->game.running = false;
                    break;
                }
                case SDL_WINDOWEVENT: // #NOTE (Juan): Window resize/orientation change
                {
                    if(event.window.event == SDL_WINDOWEVENT_MOVED) {
                        gameState->render.windowPosition.x = (f32)event.window.data1;
                        gameState->render.windowPosition.y = (f32)event.window.data2;
                    }
                    else if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        gameState->render.windowSize.x = (f32)event.window.data1;
                        gameState->render.windowSize.y = (f32)event.window.data2;
                        gameState->render.size.x = gameState->render.windowSize.x;
                        gameState->render.size.y = gameState->render.windowSize.y;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    if(mouseEnabled || mouseOverPreview) {
                        gameState->input.mouseState[event.button.button] = KEY_PRESSED;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    if(mouseEnabled || mouseOverPreview) {
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
                    if(mouseEnabled || mouseOverPreview) {
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
        gameState->time.lastFrameGameTime = startTime;
     
        if(editorRenderDebugger.recording) {
            char test[256] = "dump/frame_001.png";
            test[11] = (gameState->time.gameFrames / 100) % 10 + 48;
            test[12] = (gameState->time.gameFrames / 10) % 10 + 48;
            test[13] = (gameState->time.gameFrames) % 10 + 48;
            GL_DumpTexture(test, 1, 512, 512);
        }

        if(gameState->game.updateRunning) {
            gameState->time.gameTime += gameState->time.deltaTime;
            gameState->time.gameFrames++;
        }

        if(editorPreview.open) {
            SDL_ShowCursor(editorPreview.cursorInsideWindow);
            if(editorPreview.cursorInsideWindow) {
                gameState->input.mousePosition = RenderToViewport(editorPreview.cursorPosition.x, editorPreview.cursorPosition.y, gameState->camera.size, gameState->camera.ratio);

                ImGui::SetMouseCursor(ImGuiMouseCursor_None);
                io.MouseDrawCursor = false;
            }
        }
        else {
            SDL_ShowCursor(mouseEnabled);
            if(mouseEnabled) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_None);
                io.MouseDrawCursor = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(sdlWindow);
        ImGui::NewFrame();

#ifdef LUA_SCRIPTING_ENABLED
        ScriptingWatchChanges();
#endif

        GL_WatchChanges();
        RenderDebugStart();

        if(gameState->render.framebufferEnabled) {
            Begin2D(gameState->render.frameBuffer, (u32)gameState->render.bufferSize.x, (u32)gameState->render.bufferSize.y);
        }
        else {
            Begin2D(0, (u32)gameState->render.size.x, (u32)gameState->render.size.y);
        }

        GameLoop();

        GL_Render();

        EditorDrawAllOpen();

        RenderDebugEnd();
        End2D();

        if(gameState->render.framebufferEnabled) {
            if(!editorPreview.open) {
                // #NOTE (Juan): Render framebuffer to actual screen buffer, save data and then restore it
                f32 tempSize = gameState->camera.size;
                f32 tempRatio = gameState->camera.ratio;
                m44 tempView = gameState->camera.view;
                m44 tempProjection = gameState->camera.projection;

                bool tempWireframeMode = editorRenderDebugger.wireframeMode;

                gameState->camera.size = 1;
                gameState->camera.ratio = (f32)gameState->render.size.x / (f32)gameState->render.size.y;
                gameState->camera.view = IdM44();
                gameState->camera.projection = OrtographicProjection(gameState->camera.size, gameState->camera.ratio, gameState->camera.nearPlane, gameState->camera.farPlane);

                editorRenderDebugger.wireframeMode = false;

                Begin2D(0, (u32)gameState->render.size.x, (u32)gameState->render.size.y);
                DrawOverrideVertices(0, 0);
                DrawClear(0, 0, 0, 1);
                DrawTextureParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
                f32 sizeX = gameState->camera.size * tempRatio;
                DrawTexture(0, gameState->camera.size, sizeX, -gameState->camera.size, gameState->render.renderBuffer);
                GL_Render();
                End2D();
                
                editorRenderDebugger.wireframeMode = tempWireframeMode;

                gameState->camera.size = tempSize;
                gameState->camera.ratio = tempRatio;
                gameState->camera.view = tempView;
                gameState->camera.projection = tempProjection;
            }
            else {
                glViewport(0, 0, (u32)gameState->render.size.x, (u32)gameState->render.size.y);
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        CheckInput();
        
        SDL_GL_SwapWindow(sdlWindow);
        
        if(fpsFixed == 0)
        {
            editorPerformanceDebugger.updateTicks = SDL_GetTicks() - startTicks;
        }
        editorPerformanceDebugger.updateCycles = __rdtsc() - updateCyclesStart;

        if(fpsLimit > 0) {
            std::this_thread::sleep_until(end);
        }
    }

    GL_End();
    
    SDL_GL_DeleteContext(glContext);  

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    EditorEnd();
    GameEnd();

    ma_device_uninit(&soundDevice);

    TableSetV2(&permanentState->arena, &configSave, "windowPosition", gameState->render.windowPosition);
    TableSetV2(&permanentState->arena, &configSave, "windowSize", gameState->render.windowSize);
    SerializeTable(&configSave, "config.save");

    SerializeTable(&saveData, "saveData.save");

    return 0;
}