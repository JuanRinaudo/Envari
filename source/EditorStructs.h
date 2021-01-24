#ifndef EDITORSTRUCTS_H
#define EDITORSTRUCTS_H

enum PreviewMenuAction {
    PreviewMenuAction_NONE,
    PreviewMenuAction_CHANGE_SIZE,
};

enum DebugMenuAction {
    DebugMenuAction_NONE,
    DebugMenuAction_GO_TO_FUNCTION,
    DebugMenuAction_BREAK_ON_FUNCTION,
};

struct ConsoleLog
{
    char* log;
    size_t logSize;
    char* file;
    size_t fileSize;
    u32 line;
    u32 count;
    ConsoleLogType type;
};

struct PreviewWindow
{
    bool open;
    bool focused;

    bool cursorInsideWindow;
    v2 cursorPosition;
    v2i changeSize;
    bool showData;
};

struct ConsoleWindow
{
    bool open;

    char inputBuffer[CONSOLE_INPUT_BUFFER_COUNT];
    ImVector<ConsoleLog> items;
    ImVector<const char*> commands;
    ImVector<char*> history;
    i32 historyPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter filter;
    bool autoScroll;
    bool scrollToBottom;
};

enum TextureInspect
{
    TextureInspect_ALL,
    TextureInspect_CACHE,
};

struct PerformanceDebuggerWindow
{
    bool open;

    u64 updateTime;
    u64 updateCycles;
    u64 luaUpdateTime;
    u64 luaUpdateCycles;
    PROCESS_MEMORY_COUNTERS memoryCounters;
};

struct RenderDebuggerWindow
{
    bool open;

    bool recording;
    i32 renderMemory;
    i32 drawCount;
    i32 programChanges;

    bool wireframeMode;
};

struct MemoryDebuggerWindow
{
    bool open;
};

struct TextureDebuggerWindow
{
    bool open;

    i32 textureIndex;
    i32 textureLevel;
    i32 textureWidth;
    i32 textureHeight;
    TextureInspect inspectMode;
};

struct SoundDebuggerWindow
{
    bool open;

    i32 cacheIndex;
};

#ifdef LUA_SCRIPTING_ENABLED
struct LUADebuggerWindow
{
    bool open;

    bool debugging;
    char inputBuffer[256];
    char* currentFile;
    u32 currentFileSize;
};
#endif

struct HelpWindow
{
    bool open;
};

#endif