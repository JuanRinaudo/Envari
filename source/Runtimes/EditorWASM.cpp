#include <emscripten.h>

#include <chrono>
#include <thread>
#include <string>

#include "OptickDummy.h"

#define SHADER_PREFIX "shaders/es/"
#define SOURCE_TYPE const char* const

#define INITLUASCRIPT EDITORWASMCONFIG_INITLUASCRIPT

#include <SDL.h>

#include <imgui.cpp>

#include <GL/gl.h>
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

#include <Editor.cpp>

#undef GL_VERTEX_ARRAY_BINDING
#include <IMGUI/imgui_demo.cpp>
#include <IMGUI/imgui_draw.cpp>
#include <IMGUI/imgui_widgets.cpp>
#include <IMGUI/imgui_customs.cpp>
#include <IMGUI/imgui_tables.cpp>

#include <IMGUI/imgui_impl_sdl.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include <IMGUI/imgui_impl_sdl.cpp>
#include <IMGUI/imgui_impl_opengl3.cpp>

#include <PlatformCommon.h>
#include <EditorCommon.h>

int frameCount = 0;

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
    gameState->memory.editorStorageSize = Megabytes(32);
    gameState->memory.editorStorage = malloc(gameState->memory.editorStorageSize);

    permanentState = (PermanentData *)gameState->memory.permanentStorage + sizeof(Data);
    sceneState = (SceneData *)gameState->memory.sceneStorage;
    temporalState = (TemporalData *)gameState->memory.temporalStorage;
    editorState = (EditorData *)gameState->memory.editorStorage;
    editorState->editorFrameRunning = true;

    InitializeArena(&permanentState->arena, gameState->memory.permanentStorageSize, (u8 *)gameState->memory.permanentStorage, sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&sceneState->arena, gameState->memory.sceneStorageSize, (u8 *)gameState->memory.sceneStorage, sizeof(SceneData));
    InitializeArena(&temporalState->arena, gameState->memory.temporalStorageSize, (u8 *)gameState->memory.temporalStorage, sizeof(TemporalData));
    InitializeArena(&editorState->arena, gameState->memory.editorStorageSize, (u8 *)gameState->memory.editorStorage, sizeof(EditorData));

    stringAllocator = PushStruct(&permanentState->arena, StringAllocator);
    InitializeStringAllocator(stringAllocator);

    SetupEnviroment();

    DeserializeDataTable(&permanentState->arena, &initialConfig, DATA_EDITORWASMCONFIG_ENVT);

    InitEngine();

    if(!InitSDL()) {
        return -1;
    }

    if(!SetupWindow()) {
        return -1;
    }

    if(!SetupTime()) {
        return -1;
    }

    emscripten_set_main_loop(main_loop, gameState->time.fpsFixed > 0 ? gameState->time.fpsFixed : 0, false);

    InitImGui();

    InitGL();

    CreateFramebuffer();
    
    DefaultAssets();

    EditorInit();

#ifdef LUA_ENABLED
    ScriptingInit();
#endif

    EM_ASM(
        FS.mount(IDBFS, {}, '/save');
        FS.syncfs(true, function (err) {
            ccall('main_loaded', 'v');
        });
    );

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
    while(gameState->game.running) {
        TimeTick();
        
        ImGuiIO imguiIO = ImGui::GetIO();
        mouseOverWindow = editorPreview.open && editorPreview.cursorInsideWindow;
        mouseEnabled = !imguiIO.WantCaptureMouse && !editorPreview.open;
        keyboardEnabled = !imguiIO.WantCaptureKeyboard;
        
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            ProcessEvent(&event);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(sdlWindow);
        ImGui::NewFrame();

        if(editorState->editorFrameRunning || editorState->playNextFrame) {
            RenderDebugStart();

            CommonBegin2D();
#ifdef LUA_ENABLED
            ScriptingUpdate();
#endif
            EngineUpdate();

            RenderPass();

            RenderDebugEnd();
            End2D();
        }
        else {
            RenderDebugStart();
            RenderPass();
            RenderDebugEnd();
        }

        if(gameState->render.framebufferEnabled) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        EditorDrawAll();
        if(!editorState->layoutInited) {
            editorState->layoutInited = true;
            EditorDefaultLayout();
        }

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
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        CheckInput();
        
        SDL_GL_SwapWindow(sdlWindow);

        ++frameCount;
        if(frameCount >= editorTimeDebugger.framesMultiplier) {
            WaitFPSLimit();
            frameCount = 0;
            break;
        }
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
#ifdef LUA_ENABLED
    RunLUAProtectedFunction(EditorEnd);
#endif
    EditorEnd();
    GameEnd();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}