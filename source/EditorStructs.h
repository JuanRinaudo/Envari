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

enum WatchType {
    WatchType_AUTO,
    WatchType_INT,
    WatchType_FLOAT,
    WatchType_BOOL,
    WatchType_STRING,
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
    f32 previewOffsetY;
};

struct ConsoleWindow
{
    bool open;

    ImVector<ConsoleLog> items;
    ImVector<const char*> commands;
    ImVector<char*> history;
    i32 historyPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter filter;
    bool autoScroll;
    bool scrollToBottom;
    i32 logFlags;
    
    char inputBuffer[CONSOLE_INPUT_BUFFER_COUNT];
};

enum TextureInspect
{
    TextureInspect_ALL,
    TextureInspect_CACHE,
};

struct AssetsWindow
{
    bool open;
};

struct PerformanceDebuggerWindow
{
    bool open;

    u64 updateTime;
    u64 updateCycles;
    u64 luaUpdateTime;
    u64 luaUpdateCycles;
};

struct RenderDebuggerWindow
{
    bool open;

    bool recording;
    i32 renderMemory;
    i32 drawCount;
    i32 programChanges;

    bool wireframeMode;

    i32 renderDebugTarget;
    bool renderDebugTargetChanged;
};

struct MemoryDebuggerWindow
{
    bool open;
    PROCESS_MEMORY_COUNTERS memoryCounters;
};

struct TextureDebuggerWindow
{
    bool open;

    bool textureChanged;
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
    i32 bufferOffset;
    f32* bufferToShow;
    f32 bufferToShowMin[SOUND_CHANNELS];
    f32 bufferToShowMax[SOUND_CHANNELS];
};

struct InputDebuggerWindow
{
    bool open;
};

struct TimeDebuggerWindow
{
    bool open;

    i32 debuggerOffset = 0;

    f32* frameTimeBuffer;
    f32 frameTimeMin;
    f32 frameTimeMax;
    f32* fpsBuffer;
    f32 fpsMin;
    f32 fpsMax;
};

#ifdef LUA_ENABLED
#define WATCH_BUFFER_SIZE 64
#define WATCH_BUFFER_SIZE_EXT WATCH_BUFFER_SIZE + 1 
#define WATCH_BUFFER_COUNT 8
struct LUADebuggerWindow
{
    bool open;

    bool debugging;
    char inputBuffer[CONSOLE_INPUT_BUFFER_COUNT];
    char currentFileName[LUA_FILENAME_MAX];
    char* currentFileBuffer;
    u32 currentFileBufferSize;

    bool codeOpen;    
    bool watchOpen;

    i32 watchType[WATCH_BUFFER_COUNT];
    char watchBuffer[WATCH_BUFFER_SIZE_EXT * WATCH_BUFFER_COUNT];
};
#endif

struct HelpWindow
{
    bool open;
};

#endif