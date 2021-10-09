#ifndef MEMORYSTRUCTS_H
#define MEMORYSTRUCTS_H

struct MemoryArena
{
    size_t size;
    u8* base;
    size_t used;

    u32 tempCount;
    size_t dataSize;
};

struct TemporaryMemory
{
    MemoryArena* arena;
    size_t used;
};

struct StringAllocator
{
#if PLATFORM_EDITOR
    size_t stringReallocOnAsignLastFrame;
    size_t stringsAllocatedLastFrame;
    size_t totalStringsReallocated;
    size_t totalStringsAllocated;
#endif
};

struct DynamicString
{
    StringAllocator* allocator;
    size_t allocSize;
    size_t size;
    char* value;

    DynamicString& operator=(char* input);
};

#endif