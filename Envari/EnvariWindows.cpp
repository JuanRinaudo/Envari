#include <windows.h>
#include <chrono>
#include <thread>

#define SOURCE_TYPE const char* const

#include "GL3W/gl3w.c"

#include "Defines.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Game.h"
#include "GLRender.h"

#include <GLFW/glfw3.h>

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"

GLFWwindow* Window;

static void WindowResizeCallback(GLFWwindow* targetWindow, int width, int height)
{
    gameState->screen.width = width;
    gameState->screen.height = height;
    
	lua["screen"]["width"] = (gameState->screen).width;
	lua["screen"]["height"] = (gameState->screen).height;
	lua["screen"]["refreshRate"] = (gameState->screen).refreshRate;
}

int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int ShowCode)
{

    gameMemory = malloc(Megabytes(128));

    gameState = (Data *)gameMemory;
    gameState->memory.permanentStorageSize = Megabytes(32);
    gameState->memory.temporalStorageSize = Megabytes(96);
    gameState->memory.permanentStorage = gameMemory;
    gameState->memory.temporalStorage = (u8 *)gameMemory + gameState->memory.permanentStorageSize;

    permanentState = (PermanentData *)gameState->memory.permanentStorage + sizeof(Data);
    temporalState = (TemporalData *)gameState->memory.temporalStorage;

    InitializeArena(&permanentState->arena, (memoryIndex)(gameState->memory.permanentStorageSize - sizeof(PermanentData) - sizeof(Data)), (u8 *)gameState->memory.permanentStorage + sizeof(PermanentData) + sizeof(Data));
    InitializeArena(&temporalState->arena, (memoryIndex)(gameState->memory.temporalStorageSize - sizeof(TemporalData)), (u8 *)gameState->memory.temporalStorage + sizeof(TemporalData));

    ParseDataTable(&initialConfig, "data/windowsConfig.envt");

    if (!glfwInit()) {
        return -1;
    }

    GLFWmonitor* mainMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode( mainMonitor );
    v2 windowSize = TableGetV2(&initialConfig, "windowSize");
    if(windowSize.x <= 1 && windowSize.y <= 1) {
        gameState->screen.width = FloorToInt(videoMode->width * windowSize.x);
        gameState->screen.height = FloorToInt(videoMode->height * windowSize.y);
    }
    else {
        gameState->screen.width = FloorToInt(windowSize.x);
        gameState->screen.height = FloorToInt(windowSize.y);
    }
    gameState->screen.refreshRate = videoMode->refreshRate;

    char* windowTitle = TableGetString(&initialConfig, "windowTitle");
    Window = glfwCreateWindow(gameState->screen.width, gameState->screen.height, windowTitle, NULL, NULL);
    glfwSetWindowSizeCallback(Window, WindowResizeCallback);

    if (!Window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);

	if (gl3wInit()) {
		return -1;
	}

    int fpsLimit = TableGetInt(&initialConfig, "fpsLimit");
    int fpsDelta = 1000 / fpsLimit;
    int vsync = TableGetInt(&initialConfig, "vsync");
    glfwSwapInterval(vsync);

    const char* glsl_version = 0;
    
    // NOTE(Juan): Dear IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    char *scriptsPath = "scripts/";
    ScriptingInit(scriptsPath);

    GameInit();

    GL_Init();
    coloredProgram = GL_CompileProgram("shaders/glcore/colored.vert", "shaders/glcore/colored.frag");
    texturedProgram = GL_CompileProgram("shaders/glcore/textured.vert", "shaders/glcore/textured.frag");

    Running = true;
    auto start = std::chrono::steady_clock::now();
    while (Running)
    {

        double gameTime = glfwGetTime();
        gameState->time.gameTime = (f32)gameTime;
        gameState->time.deltaTime = (f32)(gameTime - gameState->time.lastFrameGameTime);
        gameState->time.frames++;

        auto now = std::chrono::steady_clock::now();
        i64 epochTime = now.time_since_epoch().count() / 1000000;
        auto diff = now - start;
        auto end = now + std::chrono::milliseconds(fpsDelta - epochTime % fpsDelta);
        if(diff >= std::chrono::seconds(1))
        {
            start = now;
        }

        glfwPollEvents();

        gameState->time.lastFrameGameTime = gameState->time.gameTime;

        Running = !glfwWindowShouldClose(Window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        ScriptingWatchChanges();

        Begin2D();

        ScriptingUpdate();
        GameLoop();

        GL_WatchChanges();
        GL_Render();

        End2D();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        std::this_thread::sleep_until(end);
        glfwSwapBuffers(Window);

    }

    glfwTerminate();

    GLEnd();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    GameEnd();

    return 0;

}