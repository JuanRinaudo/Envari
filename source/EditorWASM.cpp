#include <emscripten.h>

#include <chrono>
#include <thread>
#include <string>

#define OPTICK_EVENT(...)
#define OPTICK_CATEGORY(NAME, CATEGORY)
#define OPTICK_FRAME(NAME)
#define OPTICK_THREAD(THREAD_NAME)
#define OPTICK_START_THREAD(THREAD_NAME)
#define OPTICK_STOP_THREAD()
#define OPTICK_TAG(NAME, DATA)
#define OPTICK_EVENT_DYNAMIC(NAME)	
#define OPTICK_PUSH_DYNAMIC(NAME)		
#define OPTICK_PUSH(NAME)				
#define OPTICK_POP()		
#define OPTICK_CUSTOM_EVENT(DESCRIPTION)
#define OPTICK_STORAGE_REGISTER(STORAGE_NAME)
#define OPTICK_STORAGE_EVENT(STORAGE, DESCRIPTION, CPU_TIMESTAMP_START, CPU_TIMESTAMP_FINISH)
#define OPTICK_STORAGE_PUSH(STORAGE, DESCRIPTION, CPU_TIMESTAMP_START)
#define OPTICK_STORAGE_POP(STORAGE, CPU_TIMESTAMP_FINISH)				
#define OPTICK_SET_STATE_CHANGED_CALLBACK(CALLBACK)
#define OPTICK_SET_MEMORY_ALLOCATOR(ALLOCATE_FUNCTION, DEALLOCATE_FUNCTION)	
#define OPTICK_SHUTDOWN()
#define OPTICK_GPU_INIT_D3D12(DEVICE, CMD_QUEUES, NUM_CMD_QUEUS)
#define OPTICK_GPU_INIT_VULKAN(DEVICES, PHYSICAL_DEVICES, CMD_QUEUES, CMD_QUEUES_FAMILY, NUM_CMD_QUEUS)
#define OPTICK_GPU_CONTEXT(...)
#define OPTICK_GPU_EVENT(NAME)
#define OPTICK_GPU_FLIP(SWAP_CHAIN)
#define OPTICK_UPDATE()
#define OPTICK_FRAME_FLIP(...)
#define OPTICK_FRAME_EVENT(FRAME_TYPE, ...)
#define OPTICK_START_CAPTURE(...)
#define OPTICK_STOP_CAPTURE()
#define OPTICK_SAVE_CAPTURE(...)
#define OPTICK_APP(NAME)

#include "CodeGen/FileMap.h"
#include "CodeGen/ShaderMap.h"
#include "CodeGen/EditorWasmConfigMap.h"

#define SHADER_PREFIX "shaders/es/"
#define SOURCE_TYPE const char* const

#define ENVARI_PLATFORM_NAME "EditorWASM"

#define INITLUASCRIPT EDITORWASMCONFIG_INITLUASCRIPT

#include <SDL.h>

#define DEFAULT_MIN_FILTER GL_LINEAR_MIPMAP_LINEAR
#define DEFAULT_MAG_FILTER GL_LINEAR
#define FRAMEBUFFER_DEFAULT_FILTER GL_LINEAR

#include "IMGUI/imgui.cpp"

#include "STB/stb_truetype.h"

#include <GL/gl.h>
#include <GLES3/gl3.h>

#define GL_PROFILE_GLES3
#include "Game.h"

static void main_loop();

extern "C" {
    i32 main(i32 argc, char** argv);
    void main_loaded();
    void main_save();
    void main_end();
}

#include "Editor.cpp"

#undef GL_VERTEX_ARRAY_BINDING
#include "IMGUI/imgui_demo.cpp"
#include "IMGUI/imgui_draw.cpp"
#include "IMGUI/imgui_widgets.cpp"
#include "IMGUI/imgui_customs.cpp"
#include "IMGUI/imgui_tables.cpp"

#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "IMGUI/imgui_impl_sdl.cpp"
#include "IMGUI/imgui_impl_opengl3.cpp"

#include "PlatformCommon.h"
#include "EditorCommon.h"

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

    InitEngine();

    DeserializeDataTable(&permanentState->arena, &initialConfig, DATA_WASMCONFIG_ENVT);

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

    InitImGui();

    InitGL();

    CreateFramebuffer();
    
    DefaultAssets();

    EditorInit();

#ifdef LUA_ENABLED
    ScriptingInit();
#endif

    EM_ASM(
        FS.mkdir('/save');
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
    if(gameState->game.running) {
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
            GameUpdate();

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
#ifdef LUA_ENABLED
    RunLUAProtectedFunction(EditorEnd);
#endif
    EditorEnd();
    GameEnd();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}