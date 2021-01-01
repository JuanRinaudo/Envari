#ifndef MEMORYSTRUCTS_H
#define MEMORYSTRUCTS_H

struct MemoryArena {
    size_t size;
    u8 *base;
    size_t used;

    u32 tempCount;
    size_t dataSize;
};

struct TemporaryMemory
{
    MemoryArena *arena;
    size_t used;
};
    
#endif