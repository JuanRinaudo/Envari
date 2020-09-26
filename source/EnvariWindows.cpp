#include <chrono>
#include <thread>

#include <string>

#include "CodeGen/FileMap.h"
#include "CodeGen/WindowsConfigMap.h"

#define SOURCE_TYPE const char* const

#include "GL3W/gl3w.c"
#define IMGUI_IMPL_OPENGL_LOADER_GL3W

#define INITLUASCRIPT WINDOWSCONFIG_INITLUASCRIPT

#include <SDL.h>

#include "IMGUI/imgui.cpp"

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

    InitializeArena(&permanentState->arena, (memoryIndex)(gameState->memory.permanentStorageSize - sizeof(PermanentData) - sizeof(Data)), (u8 *)gameState->memory.permanentStorage + sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, (memoryIndex)(gameState->memory.sceneStorageSize - sizeof(SceneData)), (u8 *)gameState->memory.sceneStorage + sizeof(SceneData));
    InitializeArena(&temporalState->arena, (memoryIndex)(gameState->memory.temporalStorageSize - sizeof(TemporalData)), (u8 *)gameState->memory.temporalStorage + sizeof(TemporalData));

    ParseDataTable(&initialConfig, DATA_WINDOWSCONFIG_ENVT);

    // #TODO (Juan): Check this SDL_INIT_EVERYTHING, check what really needs to be init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GetCurrentDisplayMode(0, &displayMode);
    v2 windowSize = TableGetV2(&initialConfig, WINDOWSCONFIG_WINDOWSIZE);
    if(windowSize.x <= 10 && windowSize.y <= 10) {
        gameState->render.width = FloorToInt(displayMode.w * windowSize.x);
        gameState->render.height = FloorToInt(displayMode.h * windowSize.y);
    }
    else {
        gameState->render.width = FloorToInt(windowSize.x);
        gameState->render.height = FloorToInt(windowSize.y);
    }

    gameState->render.framebufferEnabled = TableHasKey(&initialConfig, WINDOWSCONFIG_BUFFERSIZE);
    if(gameState->render.framebufferEnabled) {
        v2 bufferSize = TableGetV2(&initialConfig, WINDOWSCONFIG_BUFFERSIZE);
        if(windowSize.x <= 10 && windowSize.y <= 10) {
            gameState->render.bufferWidth = FloorToInt(gameState->render.width * bufferSize.x);
            gameState->render.bufferHeight = FloorToInt(gameState->render.height * bufferSize.y);
        }
        else {
            gameState->render.bufferWidth = FloorToInt(bufferSize.x);
            gameState->render.bufferHeight = FloorToInt(bufferSize.y);
        }
    }
    else {
        gameState->render.bufferWidth = -1;
        gameState->render.bufferHeight = -1;
    }

    gameState->render.refreshRate = displayMode.refresh_rate;

    char* windowTitle = TableGetString(&initialConfig, WINDOWSCONFIG_WINDOWTITLE);
    sdlWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gameState->render.width, gameState->render.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!sdlWindow) {
        return -1;
    }

    glContext = SDL_GL_CreateContext(sdlWindow);

	if (gl3wInit()) {
		return -1;
	}

    i32 fpsLimit = TableGetInt(&initialConfig, WINDOWSCONFIG_FPSLIMIT);
    i32 fpsDelta = 1000 / fpsLimit;
    i32 vsync = TableGetInt(&initialConfig, WINDOWSCONFIG_VSYNC);
    SDL_GL_SetSwapInterval(vsync);

    const char* glsl_version = 0;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

#ifdef LUA_SCRIPTING_ENABLED
    ScriptingInit();
#endif

    GameInit();

    GL_Init();
    coloredProgram = GL_CompileProgram(SHADERS_GLCORE_COLORED_VERT, SHADERS_GLCORE_COLORED_FRAG);
    texturedProgram = GL_CompileProgram(SHADERS_GLCORE_TEXTURED_VERT, SHADERS_GLCORE_TEXTURED_FRAG);

    // #NOTE (Juan): Create framebuffer
    if(gameState->render.framebufferEnabled) {
        GL_InitFramebuffer(gameState->render.bufferWidth, gameState->render.bufferHeight);
    }
    else {
        gameState->render.frameBuffer = 0;
    }

    SoundInit();

    gameState->game.running = true;
    auto start = std::chrono::steady_clock::now(); // #NOTE (Juan): Start timer for fps limit
    while (gameState->game.running)
    {
        f32 gameTime = SDL_GetTicks() / 1000.0f;
        gameState->time.gameTime = (f32)gameTime;
        gameState->time.deltaTime = (f32)(gameTime - gameState->time.lastFrameGameTime);
        gameState->time.frames++;

        // #NOTE(Juan): Do a fps limit if enabled
        std::chrono::steady_clock::time_point end;
        if(fpsLimit > 0) {
            auto now = std::chrono::steady_clock::now();
            i64 epochTime = now.time_since_epoch().count() / 1000000;
            auto diff = now - start;
            end = now + std::chrono::milliseconds(fpsDelta - epochTime % fpsDelta);
            if(diff >= std::chrono::seconds(1))
            {
                start = now;
            }
        }

        ImGuiIO imguiIO = ImGui::GetIO();
        bool mouseEnabled = !imguiIO.WantCaptureMouse;
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

                        gameState->input.mousePosition = ScreenToViewport(gameState->input.mouseScreenPosition.x, gameState->input.mouseScreenPosition.y, gameState->camera.size, gameState->camera.ratio);
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

        gameState->time.lastFrameGameTime = gameState->time.gameTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(sdlWindow);
        ImGui::NewFrame();

#ifdef LUA_SCRIPTING_ENABLED
        ScriptingWatchChanges();
#endif

        GL_WatchChanges();        

        if(gameState->render.framebufferEnabled) {
            Begin2D(gameState->render.frameBuffer, (u32)gameState->render.bufferWidth, (u32)gameState->render.bufferHeight);
        }
        else {
            Begin2D(0, (u32)gameState->render.width, (u32)gameState->render.height);
        }

        GameLoop();
        GL_Render();

        End2D();

        if(gameState->render.framebufferEnabled) {
            // #NOTE (Juan): Render framebuffer to actual screen buffer, save data and then restore it
            f32 tempSize = gameState->camera.size;
            f32 tempRatio = gameState->camera.ratio;
            m44 tempView = gameState->camera.view;
            m44 tempProjection = gameState->camera.projection;

            gameState->camera.size = 1;
            gameState->camera.ratio = (f32)gameState->render.width / (f32)gameState->render.height;
            gameState->camera.view = IdM44();
            gameState->camera.projection = OrtographicProjection(gameState->camera.size, gameState->camera.ratio, gameState->camera.nearPlane, gameState->camera.farPlane);
            Begin2D(0, (u32)gameState->render.width, (u32)gameState->render.height);
            DrawOverrideVertices(0, 0);
            DrawClear(0, 0, 0, 1);
            DrawTextureParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
            f32 sizeX = gameState->camera.size * tempRatio;
            DrawTexture(-sizeX * 0.5f, gameState->camera.size * 0.5f, sizeX, -gameState->camera.size, gameState->render.renderBuffer);
            GL_Render();
            End2D();

            gameState->camera.size = tempSize;
            gameState->camera.ratio = tempRatio;
            gameState->camera.view = tempView;
            gameState->camera.projection = tempProjection;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_GL_SwapWindow(sdlWindow);

        CheckInput();

        if(fpsLimit > 0) {
            std::this_thread::sleep_until(end);
        }
    }

    GL_End();
    
    SDL_GL_DeleteContext(glContext);  

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    GameEnd();

    ma_device_uninit(&soundDevice);

    return 0;
}