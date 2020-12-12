#ifndef EDITORSTRUCTS_H
#define EDITORSTRUCTS_H

struct DataTokenizer {
    bool active;
    void* memory;
    u32 memorySize;
    char* dataString;
    u32 dataIndex;
    char currentChar;
    char tokenBuffer[DATA_MAX_TOKEN_COUNT];
    u32 tokenBufferIndex;
    u32 currentLine;
    i32 tokenLineCount;
    bool onComment;
    bool parsingString;
};

enum DebugMenuAction {
    DebugMenuAction_NONE,
    DebugMenuAction_GO_TO_FUNCTION,
    DebugMenuAction_BREAK_ON_FUNCTION,
};

struct ConsoleLog
{
    char* log;
    size_t size;
    u32 count;
    ConsoleLogType type;
};

struct PreviewWindow
{
    bool open;

    bool cursorInsideWindow;
    v2 cursorPosition;
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

    u32 updateTicks;
    i64 updateCycles;
    u32 luaUpdateTicks;
    i64 luaUpdateCycles;
};

struct RenderDebuggerWindow
{
    bool open;

    i32 renderMemory;
    i32 drawCount;
    i32 programChanges;

    bool wireframeMode;
};

struct MemoryDebuggerWindow
{
    bool open;
    PROCESS_MEMORY_COUNTERS memoryCounters;
};

struct TextureDebuggerWindow
{
    bool open;

    i32 textureIndex;
    i32 textureWidth;
    i32 textureHeight;
    TextureInspect inspectMode;
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