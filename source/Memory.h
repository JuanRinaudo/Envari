#ifndef MEMORY_H
#define MEMORY_H

#define ZeroStruct(instance) ZeroSize(sizeof(instance), &instance)
inline void ZeroSize(size_t size, void *pointer)
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
void* PushSize_(MemoryArena *arena, size_t size)
{
    AssertMessage(arena->used + size < arena->size, "Not enoguh space in memory arena for allocation");
    void *result = arena->base + arena->used;
    arena->used += size;

    return(result);
}

void* PushSize_(TemporaryMemory *memory, size_t size)
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
static char *PushChar(TemporaryMemory *memory, char singleChar)
{
    char *result = PushChar(memory->arena, singleChar);
    ++memory->used;

    return result;
}

static char *PushString(MemoryArena *arena, const char *string, size_t *stringSize)
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

static char *PushString(MemoryArena *arena, const char *string, size_t size)
{
    char *result = 0;

    if(arena->used + size < arena->size) {
        result = (char*)PushSize(arena, size);
        strncpy(result, string, size);
    }
    else {
        InvalidCodePath;
    }

    return result;
}
char *PushString(MemoryArena *arena, const char *string)
{
    return PushString(arena, string, strlen(string) + 1);
}

static char *PushString(TemporaryMemory *memory, const char *string, size_t *stringSize)
{
    char *result = PushString(memory->arena, string, stringSize);
    memory->used += *stringSize;

    return result;
}

static char *PushString(TemporaryMemory *memory, const char *string, size_t size)
{
    char *result = PushString(memory->arena, string, size);
    memory->used += size;

    return result;
}
static char *PushString(TemporaryMemory *arena, const char *string)
{
    return PushString(arena, string, strlen(string) + 1);
}

static void InitializeArena(MemoryArena *arena, size_t size, void *base, size_t dataSize)
{
    arena->size = size - dataSize;
    arena->base = (u8*)base + dataSize;
    arena->used = 0;
    arena->dataSize = dataSize;
}

static void ResetArena(MemoryArena *arena)
{
    arena->used = 0;
    arena->tempCount = 0;
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
    AssertMessage(arena->used >= tempMemory->used, "Ended temp memory missmatch");
    arena->used -= tempMemory->used;
    Assert(arena->tempCount > 0);
    --arena->tempCount;
}

static void CheckArena(MemoryArena *arena)
{
    Assert(arena->tempCount == 0);
}

static void InitializeStringAllocator(StringAllocator *allocator)
{
#if GAME_EDITOR
    allocator->stringReallocOnAsignLastFrame = 0;
    allocator->stringsAllocatedLastFrame = 0;
    allocator->totalStringsReallocated = 0;
    allocator->totalStringsAllocated = 0;
#endif
}

static char* AllocateString(StringAllocator allocator, size_t size)
{
    return (char*)malloc(size);
}

static char* ResizeString(char* string, size_t size)
{
    return (char*)realloc((void*)string, size);
}

static void FreeString(char* string)
{
    return free(string);
}

static void UpdateStringAllocator(StringAllocator *allocator)
{
#if GAME_EDITOR
    allocator->totalStringsReallocated += allocator->stringReallocOnAsignLastFrame;
    allocator->totalStringsAllocated += allocator->stringsAllocatedLastFrame;

    allocator->stringReallocOnAsignLastFrame = 0;
    allocator->stringsAllocatedLastFrame = 0;
#endif
}

static DynamicString* AllocateDynamicString(StringAllocator *allocator, const char* initialValue, size_t minBufferSize = 0)
{
#if GAME_EDITOR
    allocator->stringsAllocatedLastFrame++;
#endif

    DynamicString* string = (DynamicString*)malloc(sizeof(DynamicString));
    string->allocator = allocator;
    string->size = strlen(initialValue);

    if(minBufferSize <= string->size) {
        minBufferSize = string->size;
    }

    string->allocSize = sizeof(char) * minBufferSize;
    string->value = (char*)malloc(string->allocSize);
    strncpy(string->value, initialValue, minBufferSize);

    return string;
}

static void ResizeDynamicString(DynamicString *string, size_t newSize)
{
    if(string->allocSize != newSize) {
        char* oldData = string->value;

    #if GAME_EDITOR
        string->allocator->stringReallocOnAsignLastFrame++;
    #endif
        string->value = (char*)malloc(newSize);
        string->allocSize = newSize;
        string->size = MIN(string->size, string->allocSize - 1);

        strncpy(string->value, oldData, string->size);
        string->value[string->size + 1] = 0;
        free(oldData);
    }
}

DynamicString& DynamicString::operator = (char* input)
{
    size_t inputLength = strlen(input);

    if(inputLength > allocSize) {
        ResizeDynamicString(this, inputLength + 1);
    }
    
    size = inputLength;
    strcpy(value, input);

    return *this;
}

static void FreeDynamicString(DynamicString* string)
{
    return free(string);
}

#endif