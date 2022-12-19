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

    inline char operator[](i32 i) const { Assert(value != NULL); return value[i]; }
    DynamicString& operator=(char* input);
};

struct TextBuffer
{
    size_t size;
    char* value;

    inline char operator[](i32 i) const { Assert(value != NULL); return value[i]; }
    TextBuffer& operator=(char* input) { value = input; return *this; }
    operator const char*() { return value; }
    operator char*() { return value; }
    operator void*() { return value; }
};

#endif