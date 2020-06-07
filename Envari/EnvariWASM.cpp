#include <emscripten.h>

#include "Global.h"
#include "Game.cpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

void main_loop()
{

    glfwPollEvents();

    double gameTime = glfwGetTime();
    global.time.gameTime = (float)gameTime;
    global.time.deltaTime = (float)(gameTime - global.time.lastFrameGameTime);

    global.time.lastFrameGameTime = global.time.gameTime;

    GameLoop();
    
}

int main(int argc, char** argv)
{

    vertexShaderSource = "#version 300 es\n"
        "in vec3 VertexPosition;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);\n"
        "}\0";

    fragmentShaderSource = "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    if (!glfwInit())
    {

    }

    Window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Test", NULL, NULL);

    if (!Window)
    {
        glfwTerminate();
    }

    glfwMakeContextCurrent(Window);

    GameInit();

    emscripten_set_main_loop(main_loop, 0, true);

    return 0;

}