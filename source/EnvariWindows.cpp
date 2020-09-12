#include <windows.h>
#include <chrono>
#include <thread>

#include "CodeGen/FileMap.h"
#include "CodeGen/WindowsConfigMap.h"

#define SOURCE_TYPE const char* const

#include "GL3W/gl3w.c"
#include <SDL.h>

#define KEY_COUNT 500
#define MOUSE_COUNT 8

#include "Defines.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Game.h"
#include "GLRender.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"

SDL_Window* sdlWindow;
SDL_DisplayMode displayMode;
SDL_GLContext glContext;

#define MINIAUDIO_IMPLEMENTATION
#include "Miniaudio/miniaudio.h"

// #undef STB_VORBIS_HEADER_ONLY
// #include "STB/stb_vorbis.h"

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

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);

    (void)pInput;
}

i32 CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    i32 ShowCode)
{

    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    result = ma_decoder_init_file("sound/battle_viking_horn_call_close_01.wav", NULL, &decoder);
    if (result != MA_SUCCESS) {
        return -2;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }

    printf("Press Enter to quit...");
    getchar();

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    return 0;

    // gameMemory = malloc(Megabytes(128));

    // gameState = (Data *)gameMemory;
    // gameState->memory.permanentStorageSize = Megabytes(32);
    // gameState->memory.temporalStorageSize = Megabytes(96);
    // gameState->memory.permanentStorage = gameMemory;
    // gameState->memory.temporalStorage = (u8 *)gameMemory + gameState->memory.permanentStorageSize;

    // permanentState = (PermanentData *)gameState->memory.permanentStorage + sizeof(Data);
    // temporalState = (TemporalData *)gameState->memory.temporalStorage;

    // InitializeArena(&permanentState->arena, (memoryIndex)(gameState->memory.permanentStorageSize - sizeof(PermanentData) - sizeof(Data)), (u8 *)gameState->memory.permanentStorage + sizeof(PermanentData) + sizeof(Data));
    // InitializeArena(&temporalState->arena, (memoryIndex)(gameState->memory.temporalStorageSize - sizeof(TemporalData)), (u8 *)gameState->memory.temporalStorage + sizeof(TemporalData));

    // ParseDataTable(&initialConfig, DATA_WINDOWSCONFIG_ENVT);

    // // #TODO (Juan): Check this SDL_INIT_EVERYTHING, check what really needs to be init
    // if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    //     return -1;
    // }

    // SDL_GetCurrentDisplayMode(0, &displayMode);
    // v2 windowSize = TableGetV2(&initialConfig, WINDOWSCONFIG_WINDOWSIZE);
    // if(windowSize.x <= 1 && windowSize.y <= 1) {
    //     gameState->screen.width = FloorToInt(displayMode.w * windowSize.x);
    //     gameState->screen.height = FloorToInt(displayMode.h * windowSize.y);
    // }
    // else {
    //     gameState->screen.width = FloorToInt(windowSize.x);
    //     gameState->screen.height = FloorToInt(windowSize.y);
    // }

    // v2 bufferSize = TableGetV2(&initialConfig, WINDOWSCONFIG_BUFFERSIZE);
    // if(windowSize.x <= 1 && windowSize.y <= 1) {
    //     gameState->screen.bufferWidth = FloorToInt(gameState->screen.width * bufferSize.x);
    //     gameState->screen.bufferHeight = FloorToInt(gameState->screen.height * bufferSize.y);
    // }
    // else {
    //     gameState->screen.bufferWidth = FloorToInt(bufferSize.x);
    //     gameState->screen.bufferHeight = FloorToInt(bufferSize.y);
    // }

    // gameState->screen.refreshRate = displayMode.refresh_rate;

    // char* windowTitle = TableGetString(&initialConfig, WINDOWSCONFIG_WINDOWTITLE);
    // sdlWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gameState->screen.width, gameState->screen.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // if (!sdlWindow) {
    //     return -1;
    // }

    // glContext = SDL_GL_CreateContext(sdlWindow);

	// if (gl3wInit()) {
	// 	return -1;
	// }

    // i32 fpsLimit = TableGetInt(&initialConfig, WINDOWSCONFIG_FPSLIMIT);
    // i32 fpsDelta = 1000 / fpsLimit;
    // i32 vsync = TableGetInt(&initialConfig, WINDOWSCONFIG_VSYNC);
    // SDL_GL_SetSwapInterval(vsync);

    // const char* glsl_version = 0;
    
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // ImGui::StyleColorsDark();
    // ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
    // ImGui_ImplOpenGL3_Init(glsl_version);

    // ScriptingInit();

    // GameInit();

    // GL_Init();
    // coloredProgram = GL_CompileProgram(SHADERS_GLCORE_COLORED_VERT, SHADERS_GLCORE_COLORED_FRAG);
    // texturedProgram = GL_CompileProgram(SHADERS_GLCORE_TEXTURED_VERT, SHADERS_GLCORE_TEXTURED_FRAG);

    // // #NOTE (Juan): Create framebuffer
    // glGenFramebuffers(1, &frameBuffer);
    // glGenTextures(1, &renderBuffer);
    // glGenRenderbuffers(1, &depthrenderbuffer);

    // glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // glBindTexture(GL_TEXTURE_2D, renderBuffer);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (i32)bufferSize.x, (i32)bufferSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderBuffer, 0);

    // glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (i32)bufferSize.x, (i32)bufferSize.y);
    // glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // glDrawBuffers(1, DrawBuffers);

    // if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //     Log(&editorConsole, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // Running = true;
    // auto start = std::chrono::steady_clock::now(); // #NOTE (Juan): Start timer for fps limit
    // while (Running)
    // {
    //     f32 gameTime = SDL_GetTicks() / 1000.0f;
    //     gameState->time.gameTime = (f32)gameTime;
    //     gameState->time.deltaTime = (f32)(gameTime - gameState->time.lastFrameGameTime);
    //     gameState->time.frames++;

    //     // #NOTE(Juan): Do a fps limit if enabled
    //     std::chrono::steady_clock::time_point end;
    //     if(fpsLimit > 0) {
    //         auto now = std::chrono::steady_clock::now();
    //         i64 epochTime = now.time_since_epoch().count() / 1000000;
    //         auto diff = now - start;
    //         end = now + std::chrono::milliseconds(fpsDelta - epochTime % fpsDelta);
    //         if(diff >= std::chrono::seconds(1))
    //         {
    //             start = now;
    //         }
    //     }

    //     ImGuiIO imguiIO = ImGui::GetIO();
    //     bool mouseEnabled = !imguiIO.WantCaptureMouse;
    //     bool keyboardEnabled = !imguiIO.WantCaptureKeyboard;
        
    //     SDL_Event event;
    //     while(SDL_PollEvent(&event)) {
    //         ImGui_ImplSDL2_ProcessEvent(&event);
    //         switch (event.type) {
    //             case SDL_QUIT: {
    //                 Running = false;
    //                 break;
    //             }
    //             case SDL_WINDOWEVENT: // #NOTE (Juan): Window resize/orientation change
    //             {
    //                 if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    //                 {
    //                     gameState->screen.width = event.window.data1;
    //                     gameState->screen.height = event.window.data2;
    //                     ScriptingUpdateScreen();
    //                 }
    //                 break;
    //             }
    //             case SDL_MOUSEBUTTONDOWN: {
    //                 if(mouseEnabled) {
    //                     gameState->input.mouseState[event.button.button] = KEY_PRESSED;
    //                 }
    //                 break;
    //             }
    //             case SDL_MOUSEBUTTONUP: {
    //                 if(mouseEnabled) {
    //                     gameState->input.mouseState[event.button.button] = KEY_RELEASED;
    //                 }
    //                 break;
    //             }
    //             case SDL_MOUSEMOTION: {
    //                 if(mouseEnabled) {
    //                     gameState->input.mouseScreenPosition.x = (f32)event.button.x;
    //                     gameState->input.mouseScreenPosition.y = (f32)event.button.y;

    //                     gameState->input.mousePosition = ScreenToViewport(gameState->input.mouseScreenPosition, gameState->camera.size, gameState->camera.ratio);
    //                 }
    //                 break;
    //             }
    //             case SDL_MOUSEWHEEL:
    //                 if(mouseEnabled) {
    //                     gameState->input.mouseWheel += event.wheel.y;
    //                 }
    //                 break;
    //             case SDL_KEYDOWN: {
    //                 if(keyboardEnabled) {
    //                     gameState->input.keyState[event.key.keysym.sym] = KEY_PRESSED;
    //                 }
    //                 break;
    //             }
    //             case SDL_KEYUP: {
    //                 if(keyboardEnabled) {
    //                     gameState->input.keyState[event.key.keysym.sym] = KEY_RELEASED;
    //                 }
    //                 break;
    //             }
    //             default:
    //                 break;
    //         }
    //     }

    //     gameState->time.lastFrameGameTime = gameState->time.gameTime;

    //     ImGui_ImplOpenGL3_NewFrame();
    //     ImGui_ImplSDL2_NewFrame(sdlWindow);
    //     ImGui::NewFrame();

    //     ScriptingWatchChanges();
    //     GL_WatchChanges();
        
    //     ScriptingUpdateIO();

    //     Begin2D(frameBuffer, (u32)bufferSize.x, (u32)bufferSize.y);

    //     GameLoop();
    //     GL_Render();

    //     End2D();

    //     // #NOTE (Juan): Render framebuffer to actual screen buffer, save data and then restore it
    //     f32 tempSize = gameState->camera.size;
    //     f32 tempRatio = gameState->camera.ratio;
    //     m44 tempView = gameState->camera.view;
    //     m44 tempProjection = gameState->camera.projection;

    //     gameState->camera.size = 1;
    //     gameState->camera.ratio = (f32)gameState->screen.width / (f32)gameState->screen.height;
    //     gameState->camera.view = IdM44();
    //     gameState->camera.projection = OrtographicProjection(gameState->camera.size, gameState->camera.ratio, gameState->camera.nearPlane, gameState->camera.farPlane);
    //     Begin2D(0, (u32)gameState->screen.width, (u32)gameState->screen.height);
    //     PushRenderOverrideVertices(0, 0);
    //     PushRenderClear(0, 0, 0, 1);
    //     PushRenderTextureParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
    //     f32 sizeX = gameState->camera.size * tempRatio;
    //     PushRenderTexture(V2(-sizeX, gameState->camera.size) * 0.5f, V2(sizeX, -gameState->camera.size), renderBuffer);
    //     GL_Render();
    //     End2D();

    //     gameState->camera.size = tempSize;
    //     gameState->camera.ratio = tempRatio;
    //     gameState->camera.view = tempView;
    //     gameState->camera.projection = tempProjection;

    //     ImGui::Render();
    //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
    //     SDL_GL_SwapWindow(sdlWindow);

    //     CheckInput();

    //     if(fpsLimit > 0) {
    //         std::this_thread::sleep_until(end);
    //     }
    // }

    // GLEnd();
    
    // SDL_GL_DeleteContext(glContext);  

    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplSDL2_Shutdown();
    // ImGui::DestroyContext();

    // GameEnd();

    // return 0;
}