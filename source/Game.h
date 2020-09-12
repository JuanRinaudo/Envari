#if !defined(GAME_H)
#define GAME_H

#include "IMGUI/imgui.h"
#include "LUA/sol.hpp"

#include "Defines.h"

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "STB/stb_ds.h"
// #define STB_VORBIS_HEADER_ONLY
// #include "STB/stb_vorbis.h"

#if !defined(STB_TRUETYPE_IMPLEMENTATION)
#define STB_TRUETYPE_IMPLEMENTATION
#include "STB/stb_truetype.h"
#endif

#define KEY_UP 0
#define KEY_RELEASED 1
#define KEY_PRESSED 2
#define KEY_DOWN 3

#include "Memory.h"

struct Screen {
    int refreshRate;
    int width;
    int height;
    int bufferWidth;
    int bufferHeight;
};

struct Camera {
    f32 size;
    f32 ratio;
    f32 nearPlane;
    f32 farPlane;
    m44 view;
    m44 projection;
};

struct TimeData {
    f32 lastFrameGameTime;
    f32 gameTime;
    f32 deltaTime;
    i64 frames;
};

struct Memory {
    void *permanentStorage;
    u64 permanentStorageSize;
    void *temporalStorage;
    u64 temporalStorageSize;
};

struct Input
{
    v2 mousePosition;
    v2 mouseScreenPosition;
    i32 mouseWheel;
    u8 mouseState[MOUSE_COUNT];
    u8 keyState[KEY_COUNT];
};

struct Data {
    Camera camera;
    Screen screen;
    TimeData time;
    Memory memory;
    Input input;
};

struct PermanentData {
    b32 initialized;
    MemoryArena arena;
};

struct TemporalData {
    b32 initialized;
    MemoryArena arena;
};

sol::state lua;

enum DataType {
    data_Int,
    data_Float,
    data_String,
    data_V2,
};

struct DataTable {
    char* key;
    char* value;
};
DataTable* initialConfig = NULL;

u32 frameBuffer;
u32 renderBuffer;
u32 depthrenderbuffer;
u32 DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

void *gameMemory;
Data *gameState;
PermanentData *permanentState;
TemporalData *temporalState;
TemporaryMemory renderTemporaryMemory;

// NOTE(Juan): Temp test data, should be deleated
b32 Running = false;
b32 FullScreen = false;

static u32 GameInit();
static u32 GameLoop();
static u32 GameEnd();

#include "Editor.h"
#include "Game.cpp"

#endif