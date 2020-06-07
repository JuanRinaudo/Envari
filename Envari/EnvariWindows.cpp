#include <windows.h>

#include "GL3W/gl3w.c"

#include "Global.h"
#include "Game.cpp"

#include <GLFW/glfw3.h>

using namespace std;

int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int ShowCode)
{

    vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor; \n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    if (!glfwInit())
        return -1;

    Window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Test", NULL, NULL);

    if (!Window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);

	if (gl3wInit()) {
		return -1;
	}

    GameInit();

    Running = true;
    while (Running)
    {

        glfwPollEvents();

        double gameTime = glfwGetTime();
        global.time.gameTime = (float)gameTime;
        global.time.deltaTime = (float)(gameTime - global.time.lastFrameGameTime);

        global.time.lastFrameGameTime = global.time.gameTime;

        Running = !glfwWindowShouldClose(Window);

        GameLoop();

    }

    glfwTerminate();

    GameEnd();

    return 0;

}