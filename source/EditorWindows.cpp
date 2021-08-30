#include <windows.h>
#include <assert.h>
#include <psapi.h>
#include <chrono>
#include <thread>

#include <string>

#define Assert(Expression) assert(Expression)
#define AssertMessage(Expression, Message) assert(Expression && Message)

#define PLATFORM_WINDOWS
#define PLATFORM_EDITOR

#include "CodeGen/FileMap.h"
#include "CodeGen/ShaderMap.h"
#include "CodeGen/EditorConfigMap.h"

#define SHADER_PREFIX "shaders/core/"
#define SOURCE_TYPE const char* const

#include "GL3W/gl3w.c"
#define IMGUI_IMPL_OPENGL_LOADER_GL3W

#define ENVARI_PLATFORM "Editor"

#define INITLUASCRIPT EDITORCONFIG_INITLUASCRIPT

#include <SDL.h>

#define DEFAULT_MIN_FILTER GL_LINEAR_MIPMAP_LINEAR
#define DEFAULT_MAG_FILTER GL_LINEAR_MIPMAP_LINEAR
#define FRAMEBUFFER_DEFAULT_FILTER GL_LINEAR

#include "IMGUI/imgui.cpp"

#include "STB/stb_truetype.h"
#include "Game.h"
#include "PlatformCommon.h"

#include "IMGUI/imgui_demo.cpp"
#include "IMGUI/imgui_draw.cpp"
#include "IMGUI/imgui_widgets.cpp"
#include "IMGUI/imgui_customs.cpp"
#include "IMGUI/imgui_tables.cpp"

#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "IMGUI/imgui_impl_sdl.cpp"
#include "IMGUI/imgui_impl_opengl3.cpp"

i32 CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    i32 ShowCode)
{
    size_t permanentStorageSize = Megabytes(64);
    void* permanentStorage = malloc(permanentStorageSize);

    permanentState = (PermanentData *)permanentStorage;

    gameState = (Data *)(permanentState + 1);
    gameState->memory.permanentStorageSize = permanentStorageSize;
    gameState->memory.permanentStorage = permanentStorage;
    gameState->memory.sceneStorageSize = Megabytes(64);
    gameState->memory.sceneStorage = malloc(gameState->memory.sceneStorageSize);
    gameState->memory.editorStorageSize = Megabytes(64);
    gameState->memory.editorStorage = malloc(gameState->memory.editorStorageSize);
    gameState->memory.temporalStorageSize = Megabytes(64);
    gameState->memory.temporalStorage = malloc(gameState->memory.temporalStorageSize);

    gameState->memory.permanentStorage = permanentStorage;
    sceneState = (SceneData *)gameState->memory.sceneStorage;
    editorState = (EditorData *)gameState->memory.editorStorage;
    editorState->editorFrameRunning = true;
    temporalState = (TemporalData *)gameState->memory.temporalStorage;

    InitializeArena(&permanentState->arena, gameState->memory.permanentStorageSize, (u8 *)gameState->memory.permanentStorage, sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, gameState->memory.sceneStorageSize, (u8 *)gameState->memory.sceneStorage, sizeof(SceneData));
    InitializeArena(&editorState->arena, gameState->memory.editorStorageSize, (u8 *)gameState->memory.editorStorage, sizeof(EditorData));
    InitializeArena(&temporalState->arena, gameState->memory.temporalStorageSize, (u8 *)gameState->memory.temporalStorage, sizeof(TemporalData));

    stringAllocator = PushStruct(&permanentState->arena, StringAllocator);
    InitializeStringAllocator(stringAllocator);

    InitEngine();

    DeserializeDataTable(&permanentState->arena, &initialConfig, DATA_EDITORCONFIG_ENVT);

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

    const char* glsl_version = 0;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imguiIO = ImGui::GetIO();
    imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Viewports not working correctly. Fix and re-enable
    imguiIO.ConfigWindowsMoveFromTitleBarOnly = true;
    
    ImGuiStyle* style = &ImGui::GetStyle();
    if (imguiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style->WindowRounding = 0.0f;
        style->Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    InitGL();

    CreateFramebuffer();
    
    DefaultAssets();

    DeserializeTable(&permanentState->arena, &editorSave, EDITOR_SAVE_PATH);
    DeserializeTable(&permanentState->arena, &saveData, GetSavePath());
    
    EditorInit();

#ifdef LUA_ENABLED
    ScriptingInit();
#endif
    
    GameInit();

    SoundInit();

    editorTimeDebugger.frameTimeBuffer = (f32*)malloc(sizeof(f32) * TIME_BUFFER_SIZE);
    editorTimeDebugger.frameTimeMax = 1;
    editorTimeDebugger.fpsBuffer = (f32*)malloc(sizeof(f32) * TIME_BUFFER_SIZE);
    editorTimeDebugger.fpsMax = 1;

    HANDLE processHandle = GetCurrentProcess();

    editorRenderDebugger.recording = false;

    gameState->game.running = true;
    while (gameState->game.running)
    {
        GetProcessMemoryInfo(processHandle, &editorMemoryDebugger.memoryCounters, sizeof(PROCESS_MEMORY_COUNTERS));

        LARGE_INTEGER performanceStart;
        QueryPerformanceCounter(&performanceStart);
        i64 updateCyclesStart = __rdtsc();

        TimeTick();

        ImGuiIO imguiIO = ImGui::GetIO();
        mouseOverWindow = editorPreview.open && editorPreview.cursorInsideWindow;
        mouseEnabled = !imguiIO.WantCaptureMouse && !editorPreview.open;
        keyboardEnabled = !imguiIO.WantCaptureKeyboard;

        editorTimeDebugger.frameTimeBuffer[editorTimeDebugger.debuggerOffset] = 1000.0f / imguiIO.Framerate;
        if(gameState->time.gameTime > 2 && editorTimeDebugger.frameTimeBuffer[editorTimeDebugger.debuggerOffset] > editorTimeDebugger.frameTimeMax) {
            editorTimeDebugger.frameTimeMax = editorTimeDebugger.frameTimeBuffer[editorTimeDebugger.debuggerOffset] * 2;
        }
        editorTimeDebugger.fpsBuffer[editorTimeDebugger.debuggerOffset] = imguiIO.Framerate;
        if(gameState->time.gameTime > 2 && editorTimeDebugger.fpsBuffer[editorTimeDebugger.debuggerOffset] > editorTimeDebugger.fpsMax) {
            editorTimeDebugger.fpsMax = editorTimeDebugger.fpsBuffer[editorTimeDebugger.debuggerOffset] * 2;
        }
        editorTimeDebugger.debuggerOffset = (editorTimeDebugger.debuggerOffset + 1) % TIME_BUFFER_SIZE;

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            ProcessEvent(&event);
        }
     
        if(editorRenderDebugger.recording) {
            char test[256] = "dump/frame_001.png";
            test[11] = (gameState->time.gameFrames / 100) % 10 + 48;
            test[12] = (gameState->time.gameFrames / 10) % 10 + 48;
            test[13] = (gameState->time.gameFrames) % 10 + 48;
            DumpTexture(test, 1, 512, 512);
        }

        if(editorPreview.open && editorPreview.focused) {
            SDL_ShowCursor(editorPreview.cursorInsideWindow);
            if(editorPreview.cursorInsideWindow) {
                v2 lastMousePosition = ViewportToBuffer(editorPreview.lastCursorPosition.x, editorPreview.lastCursorPosition.y);
                gameState->input.mousePosition = ViewportToBuffer(editorPreview.cursorPosition.x, editorPreview.cursorPosition.y);
                gameState->input.mouseDeltaPosition.x = gameState->input.mousePosition.x - lastMousePosition.x;
                gameState->input.mouseDeltaPosition.y = gameState->input.mousePosition.y - lastMousePosition.y;

                if(gameState->input.mouseTextureID != 0) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
                    imguiIO.MouseDrawCursor = false;
                }
            }
        }
        else {
            SDL_ShowCursor(mouseEnabled);
            if(mouseEnabled) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_None);
                imguiIO.MouseDrawCursor = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(sdlWindow);
        ImGui::NewFrame();

        UpdateStringAllocator(stringAllocator);

#ifdef LUA_ENABLED
        ScriptingWatchChanges();
#endif

        WatchChanges();

        LARGE_INTEGER luaPerformanceStart = {};
        LARGE_INTEGER luaPerformanceEnd = {};
        i64 luaUpdateCyclesStart = __rdtsc();
        i64 luaUpdateCyclesEnd = __rdtsc();

        if(editorState->editorFrameRunning || editorState->playNextFrame) {
            RenderDebugStart();

            CommonBegin2D();

            QueryPerformanceCounter(&luaPerformanceStart);
#ifdef LUA_ENABLED
            ScriptingUpdate();
#endif
            QueryPerformanceCounter(&luaPerformanceEnd);

            GameUpdate();

            RenderPass();

            RenderDebugEnd();
            End2D();
        }
        else {
            if(gameState->render.framebufferEnabled) {
                glBindFramebuffer(GL_FRAMEBUFFER, gameState->render.frameBuffer);
                glViewport(0,0, (u32)gameState->render.bufferSize.x, (u32)gameState->render.bufferSize.y);
            }
            else {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(0,0, (u32)gameState->render.size.x, (u32)gameState->render.size.y);
            }

            RenderDebugStart();
            RenderPass();
            RenderDebugEnd();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        EditorDrawAll();

#ifdef LUA_ENABLED
        RunLUAProtectedFunction(EditorUpdate)
#endif

        if(gameState->render.framebufferEnabled) {
            if(!editorPreview.open) {
                // #NOTE (Juan): Render framebuffer to actual screen buffer, save data and then restore it
                bool tempWireframeMode = editorRenderDebugger.wireframeMode;
                editorRenderDebugger.wireframeMode = false;

                RenderFramebuffer();
                
                editorRenderDebugger.wireframeMode = tempWireframeMode;
            }
            else {
                glViewport(0, 0, (u32)gameState->render.size.x, (u32)gameState->render.size.y);
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (imguiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        CheckInput();
        
        SDL_GL_SwapWindow(sdlWindow);
        
        LARGE_INTEGER performanceEnd;
        QueryPerformanceCounter(&performanceEnd);
        i64 updateCyclesEnd = __rdtsc();
        editorPerformanceDebugger.updateTime = performanceEnd.QuadPart - performanceStart.QuadPart;
        editorPerformanceDebugger.luaUpdateTime = luaPerformanceEnd.QuadPart - luaPerformanceStart.QuadPart;
        editorPerformanceDebugger.updateCycles = updateCyclesEnd - updateCyclesStart;
        editorPerformanceDebugger.luaUpdateCycles = luaUpdateCyclesEnd - luaUpdateCyclesStart;

        WaitFPSLimit();
    }
    
#ifdef LUA_ENABLED
    RunLUAProtectedFunction(EditorEnd);
#endif
    EditorEnd();
    GameEnd();

    SaveConfig();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}