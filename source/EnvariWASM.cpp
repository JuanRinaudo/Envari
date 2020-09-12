#include <emscripten.h>

#define SOURCE_TYPE const char*

#include <GL/glew.h>

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

static void main_loop()
{
    glfwPollEvents();

    double gameTime = glfwGetTime();
    gameState->time.gameTime = (f32)gameTime;
    gameState->time.deltaTime = (f32)(gameTime - gameState->time.lastFrameGameTime);

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

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(Window);    
}

static void main_end()
{
    glfwTerminate();

    GLEnd();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    GameEnd();
}

int main(int argc, char** argv)
{
    gameMemory = malloc(Megabytes(128));

    gameState = (Data *)gameMemory;
    gameState->memory.permanentStorageSize = Megabytes(32);
    gameState->memory.temporalStorageSize = Megabytes(96);
    gameState->memory.permanentStorage = gameMemory;
    gameState->memory.temporalStorage = (u8 *)gameMemory + gameState->memory.permanentStorageSize;

    permanentState = (PermanentData *)gameState->memory.permanentStorage + sizeof(Data);
    temporalState = (TemporalData *)gameState->memory.temporalStorage;

    if (!glfwInit()) {
        return -1;
    }

    // GLFWmonitor* mainMonitor = glfwGetPrimaryMonitor();
    // const GLFWvidmode* videoMode = glfwGetVideoMode( mainMonitor );
    // gameState->screen.width = FloorToInt(videoMode->width * .5f);
    // gameState->screen.height = FloorToInt(videoMode->height * .5f);
    gameState->screen.width = FloorToInt(1024);
    gameState->screen.height = FloorToInt(768);
    gameState->screen.refreshRate = 60;

    Window = glfwCreateWindow(gameState->screen.width, gameState->screen.height, "OpenGL Test", NULL, NULL);
    glfwSetWindowSizeCallback(Window, WindowResizeCallback);

    if (!Window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);

	if (glewInit()) {
		return -1;
	}

    const char* glsl_version = 0;
    
    // NOTE(Juan): Dear IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ScriptingInit();
    
    GameInit();

    GL_Init();
    coloredProgram = GL_CompileProgram("shaders/gles/colored.vert", "shaders/gles/colored.frag");
    texturedProgram = GL_CompileProgram("shaders/gles/textured.vert", "shaders/gles/textured.frag");

    emscripten_set_main_loop(main_loop, 0, true);

    return 0;
}