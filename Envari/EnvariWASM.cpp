#include <emscripten.h>

#include "Defines.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Game.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"

GLFWwindow* Window;

void WindowResizeCallback(GLFWwindow* targetWindow, int width, int height)
{    
    gameState->screen.width = width;
    gameState->screen.height = height;
}

void main_loop()
{

    glfwPollEvents();

    double gameTime = glfwGetTime();
    gameState->time.gameTime = (float)gameTime;
    gameState->time.deltaTime = (float)(gameTime - gameState->time.lastFrameGameTime);

    gameState->time.lastFrameGameTime = gameState->time.gameTime;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    GameLoop();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(Window);
    
}

void main_end()
{

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}

int main(int argc, char** argv)
{

    gameMemory = malloc(Megabytes(128));

    if (!glfwInit())
    {

    }

    GLFWmonitor* mainMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode( mainMonitor );
    global.screen.width = videoMode->width;
    global.screen.height = videoMode->height;
    global.screen.refreshRate = videoMode->refreshRate;

    Window = glfwCreateWindow(global.screen.width, global.screen.height, "OpenGL Test", NULL, NULL);
    glfwSetWindowSizeCallback(Window, WindowResizeCallback);

    if (!Window)
    {
        glfwTerminate();
    }

    glfwMakeContextCurrent(Window);

    const u8* glsl_version = 0;
    
    // NOTE(Juan): Dear IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    GameInit();

    GLCompileProgram(vertexShaderSource, fragmentShaderSource);

    emscripten_set_main_loop(main_loop, 0, true);

    return 0;

}