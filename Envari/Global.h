#pragma once

#include "Defines.h"

#if __EMSCRIPTEN__
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

GLFWwindow* Window;

struct TimeData {
	float lastFrameGameTime;
	float gameTime;
	float deltaTime;
};

struct Data {
	TimeData time;
};

const char* vertexShaderSource;
const char* fragmentShaderSource;

Data global;

// NOTE(Juan): Temp test data, should be deleated
bool Running = false;
bool FullScreen = false;

bool test = true;
f32 testSpeed = 0.1f;

u32 VBO;
u32 VAO;
unsigned int shaderProgram;