#if !defined(GAME_H)
#define GAME_H

#include "Defines.h"
#include "Editor.h"
#include "LUA/sol.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "STB/stb_ds.h"

#if !defined(STB_TRUETYPE_IMPLEMENTATION)
#define STB_TRUETYPE_IMPLEMENTATION
#include "STB/stb_truetype.h"
#endif

struct MemoryArena {
    memoryIndex size;
    u8 *base;
    memoryIndex used;

    u32 tempCount;
};

struct TemporaryMemory
{
    MemoryArena *arena;
    memoryIndex used;
};

#define ZeroStruct(instance) ZeroSize(sizeof(instance), &instance)
inline void ZeroSize(memoryIndex size, void *pointer)
{
    // TODO(Juan): Check this for performance
    u8 *byte = (u8 *)pointer;
    while(size--)
    {
        *byte++ = 0;
    }
}

#define PushStruct(arena, type) (type *)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type *)PushSize_(arena, ((count)*sizeof(type)))
#define PushSize(arena, size) PushSize_(arena, size)
static void* PushSize_(MemoryArena *arena, memoryIndex size)
{
    Assert(arena->used + size < arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return(result);
}

static char* PushString(MemoryArena *arena, const char* string, u32 *stringSize)
{
    u32 size = strlen(string) + 1;
    *stringSize = size;

    Assert(arena->used + size < arena->size);

    char *result = (char*)PushSize(arena, sizeof(char) * size);
    strcpy(result, string);
    arena->used += size;

    return result;
}

static void InitializeArena(MemoryArena *arena, memoryIndex size, void *base)
{
    arena->size = size;
    arena->base = (u8*)base;
    arena->used = 0;
}

static TemporaryMemory BeginTemporaryMemory(MemoryArena *arena)
{
    TemporaryMemory result;

    result.arena = arena;
    result.used = arena->used;

    ++arena->tempCount;

    return(result);
}

static void EndTemporaryMemory(TemporaryMemory *tempMemory)
{
    MemoryArena *arena = tempMemory->arena;
    Assert(arena->used >= tempMemory->used);
    arena->used -= tempMemory->used;
    Assert(arena->tempCount > 0);
    --arena->tempCount;
}

static void CheckArena(MemoryArena *arena)
{
    Assert(arena->tempCount == 0);
}

struct Screen {
	int refreshRate;
	int width;
	int height;
};

struct Camera {
    f32 size;
    f32 ratio;
    f32 nearPlane;
    f32 farPlane;
};

struct TimeData {
	f32 lastFrameGameTime;
	f32 gameTime;
	f32 deltaTime;
};

struct Memory {
	void *permanentStorage;
    u64 permanentStorageSize;
	void *temporalStorage;
    u64 temporalStorageSize;
};

struct DemoData {
    b32 backgroundR;
    b32 backgroundG;
    b32 backgroundB;
    f32 colorSpeed;
    f32 triangleTopX;
    v4 renderColor;
};

struct Data {
    Camera camera;
	Screen screen;
	TimeData time;
	Memory memory;
    
    DemoData demo;
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

static EnvariConsole console;
bool consoleOpen;

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

#include "Game.cpp"

#endif