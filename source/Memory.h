#if !defined(MEMORY_H)
#define MEMORY_H

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

static void* PushSize_(TemporaryMemory *memory, memoryIndex size)
{
    void *result = PushSize_(memory->arena, size);
    memory->used += size;
    return(result);
}

static char *PushChar(MemoryArena *arena, char singleChar)
{
    char *result = 0;

    if(arena->used + 1 < arena->size) {
        result = (char*)PushSize(arena, 1);
        *result = singleChar;
    }
    else {
        InvalidCodePath;
    }

    return result;
}

static char *PushString(MemoryArena *arena, const char *string, u32 *stringSize)
{
    char *result = 0;

    u32 size = (u32)strlen(string) + 1;
    *stringSize = size;

    if(arena->used + size < arena->size) {
        result = (char*)PushSize(arena, size);
        strcpy(result, string);
    }
    else {
        InvalidCodePath;
    }

    return result;
}

static char *PushString(MemoryArena *arena, const char *string, u32 size)
{
    char *result = 0;

    if(arena->used + size < arena->size) {
        result = (char*)PushSize(arena, size);
        strcpy(result, string);
    }
    else {
        InvalidCodePath;
    }

    return result;
}
static char *PushString(MemoryArena *arena, const char *string)
{
    return PushString(arena, string, strlen(string) + 1);
}

static char *PushString(TemporaryMemory *memory, const char *string, u32 *stringSize)
{
    char *result = PushString(memory->arena, string, stringSize);
    memory->used += *stringSize;

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

#endif