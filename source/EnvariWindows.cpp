#include <windows.h>
#include <chrono>
#include <thread>

#define SOURCE_TYPE const char* const

#include "GL3W/gl3w.c"
#include <GLFW/glfw3.h>

#define KEY_COUNT GLFW_KEY_LAST
#define MOUSE_COUNT GLFW_MOUSE_BUTTON_LAST

#include "Defines.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Game.h"
#include "GLRender.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"

GLFWwindow* Window;

static void WindowResizeCallback(GLFWwindow* targetWindow, i32 width, i32 height)
{
    gameState->screen.width = width;
    gameState->screen.height = height;
    
	lua["screen"]["width"] = (gameState->screen).width;
	lua["screen"]["height"] = (gameState->screen).height;
	lua["screen"]["refreshRate"] = (gameState->screen).refreshRate;
}

static void KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
    if(action == GLFW_RELEASE || action == GLFW_PRESS) {
        gameState->input.keyState[key] = action == GLFW_RELEASE ? KEY_RELEASED : KEY_PRESSED;
    }
}

static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    gameState->input.mouseScreenPosition.x = (f32)xpos;
    gameState->input.mouseScreenPosition.y = (f32)ypos;

    gameState->input.mousePosition = ScreenToViewport(gameState->input.mouseScreenPosition, gameState->camera.size, gameState->camera.ratio);
}
    
static void MouseButtonCallback(GLFWwindow* window, i32 key, i32 action, i32 mods)
{    
    if(action == GLFW_RELEASE || action == GLFW_PRESS) {
        gameState->input.mouseState[key] = action == GLFW_RELEASE ? KEY_RELEASED : KEY_PRESSED;
    }
}

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

    v2 bufferSize = TableGetV2(&initialConfig, "bufferSize");
    if(windowSize.x <= 1 && windowSize.y <= 1) {
        gameState->screen.bufferWidth = FloorToInt(gameState->screen.width * bufferSize.x);
        gameState->screen.bufferHeight = FloorToInt(gameState->screen.height * bufferSize.y);
    }
    else {
        gameState->screen.bufferWidth = FloorToInt(bufferSize.x);
        gameState->screen.bufferHeight = FloorToInt(bufferSize.y);
    }

    gameState->screen.refreshRate = videoMode->refreshRate;

    char* windowTitle = TableGetString(&initialConfig, "windowTitle");
    Window = glfwCreateWindow(gameState->screen.width, gameState->screen.height, windowTitle, NULL, NULL);
    glfwSetWindowSizeCallback(Window, WindowResizeCallback);

    if (!Window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);

	if (gl3wInit()) {
		return -1;
	}

    i32 fpsLimit = TableGetInt(&initialConfig, "fpsLimit");
    i32 fpsDelta = 1000 / fpsLimit;
    i32 vsync = TableGetInt(&initialConfig, "vsync");
    glfwSwapInterval(vsync);

    const char* glsl_version = 0;

    // Input
    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetCursorPosCallback(Window, MousePositionCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);
    
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

    // #NOTE (Juan): Framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glGenTextures(1, &renderBuffer);
    glGenRenderbuffers(1, &depthrenderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glBindTexture(GL_TEXTURE_2D, renderBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (i32)bufferSize.x, (i32)bufferSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderBuffer, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (i32)bufferSize.x, (i32)bufferSize.y);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        console.AddLog("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    Running = true;
    auto start = std::chrono::steady_clock::now();
    while (Running)
    {
        double gameTime = glfwGetTime();
        gameState->time.gameTime = (f32)gameTime;
        gameState->time.deltaTime = (f32)(gameTime - gameState->time.lastFrameGameTime);
        gameState->time.frames++;

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

        glfwPollEvents();

        gameState->time.lastFrameGameTime = gameState->time.gameTime;

        Running = !glfwWindowShouldClose(Window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        ScriptingWatchChanges();

        GL_WatchChanges();

        Begin2D(frameBuffer, (u32)bufferSize.x, (u32)bufferSize.y);

        ScriptingUpdate();
        GameLoop();

        GL_Render();

        End2D();

        f32 tempSize = gameState->camera.size;
        f32 tempRatio = gameState->camera.ratio;
        m44 tempProjection = gameState->camera.projection;

        gameState->camera.size = 1;
        gameState->camera.ratio = (f32)gameState->screen.width / (f32)gameState->screen.height;
        gameState->camera.projection = OrtographicProjection(gameState->camera.size, gameState->camera.ratio, gameState->camera.nearPlane, gameState->camera.farPlane);
        Begin2D(0, (u32)gameState->screen.width, (u32)gameState->screen.height);
        PushRenderOverrideVertices(0, 0);
        PushRenderClear(0, 0, 0, 1);
        PushRenderTextureParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
        PushRenderTexture(V2(-gameState->camera.size, gameState->camera.size) * 0.5f, V2(gameState->camera.size, -gameState->camera.size), renderBuffer);
        GL_Render();
        End2D();

        gameState->camera.size = tempSize;
        gameState->camera.ratio = tempRatio;
        gameState->camera.projection = tempProjection;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(fpsLimit > 0) {
            std::this_thread::sleep_until(end);
        }
        
        glfwSwapBuffers(Window);

        CheckInput();
    }

    glfwTerminate();

    GLEnd();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    GameEnd();

    return 0;

}