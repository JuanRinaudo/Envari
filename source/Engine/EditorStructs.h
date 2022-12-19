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

enum RuntimePlatform {
    RuntimePlatform_WINDOWS_86,
    RuntimePlatform_WINDOWS_64,
    RuntimePlatform_ANDROID,
    RuntimePlatform_WASM
};

struct EditorCore
{
    f32 lastWatchSecond = 0;
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

    bool linearFiltering;
    u32 filterMethod;
    bool cursorInsideWindow;
    v2 lastCursorPosition;
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

enum RecordingFormat
{
    RecordingFormat_PNG,
    RecordingFormat_BMP,
    RecordingFormat_TGA,
    RecordingFormat_JPG,
    RecordingFormat_HDR,
};

enum AssetType
{
    AssetType_NONE,
    AssetType_UNKNOWN,
    AssetType_IMAGE,
    AssetType_TEXT,
    AssetType_SOUND,
};

enum TimeFormat
{
    TimeFormat_FRAMES,
    TimeFormat_TIME,
};

struct AssetsWindow
{
    bool open;
    u32 pathLevel;
    filesystem::path currentPath;
};

struct AssetsViewer
{
    bool open;
    AssetType lastAssetType;
    AssetType assetType;
    filesystem::path targetAssetPath;
    void* targetAsset;
};

struct PerformanceDebuggerWindow
{
    bool open;

    f32 updateTime;
    f32 updateMinTime;
    f32 updateMaxTime;
    u64 updateCycles;
    f32 luaUpdateTime;
    f32 luaUpdateMinTime;
    f32 luaUpdateMaxTime;
    u64 luaUpdateCycles;
};

struct RenderDebuggerWindow
{
    bool open;

    bool recording;
    RecordingFormat recordingFormat;
    i32 jpgQuality;
    size_t renderMemory;
    i32 drawCount;
    i32 programChanges;

    bool wireframeMode;

    i32 renderDebugTarget;
    bool renderDebugTargetChanged;
};

struct MemoryDebuggerWindow
{
    bool open;
#if PLATFORM_WINDOWS
    PROCESS_MEMORY_COUNTERS memoryCounters;
#endif
};

struct TextureDebuggerWindow
{
    bool open;

    bool textureChanged;
    i32 textureIndex;
    i32 textureLevel;
    v2i textureSize;
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
    bool timeloop;
    i32 framesMultiplier;
    f32 timeScale;
    TimeFormat loopFormat;
    i32 loopStartFrame;
    i32 loopEndFrame;
    f32 loopStartTime;
    f32 loopEndTime;
};

struct WatchedProgram {
    u32 vertexShader;
    u32 fragmentShader;
    u32 shaderProgram;
    char vertexFilepath[SHADER_FILENAME_MAX];
    char fragmentFilepath[SHADER_FILENAME_MAX];
    filesystem::file_time_type vertexTime;
    filesystem::file_time_type fragmentTime;
};

#define WATCHED_PROGRAMS_MAX_COUNT 50
struct ShaderDebuggerWindow
{
    bool open;

    bool debugging;
    TextBuffer vertexShaderBuffer;
    TextBuffer fragmentShaderBuffer;

    bool programIndexChanged;
    i32 programIndex;
    u32 targetID;
    u32 programID;

    i32 watchedProgramsCount = 0;
    WatchedProgram watchedPrograms[WATCHED_PROGRAMS_MAX_COUNT];
};

#ifdef LUA_ENABLED
struct WatchedFileCache {
    char* key;
    char* value;
};

#define WATCHLIST_SIZE 4096
#define WATCHLIST_ELEMENTS 32
#define WATCH_BUFFER_SIZE 64
#define WATCH_BUFFER_SIZE_EXT WATCH_BUFFER_SIZE + 1 
#define WATCH_BUFFER_COUNT 16
struct LUADebuggerWindow
{
    bool open;
    
    ImGuiID dockspaceID;

    bool debugging;
    char inputBuffer[CONSOLE_INPUT_BUFFER_COUNT];
    char currentFileName[LUA_FILENAME_MAX];
    i32 lastFileIndex;
    i32 currentFileIndex;
    i32 lastFileWatchIndex;
    i32 currentFileWatchIndex;
    char* currentFileBuffer;
    size_t currentFileBufferSize;
    
    char watchList[WATCHLIST_SIZE];
    std::filesystem::file_time_type watchListTimes[WATCHLIST_ELEMENTS];
    bool watchListEdited[WATCHLIST_ELEMENTS];
    size_t watchListSize = 0;
    u32 watchFiles = 0;

    WatchedFileCache* watchedFileCache;

    bool codeOpen;
    bool watchOpen;
    bool stackOpen;

    i32 watchType[WATCH_BUFFER_COUNT];
    char watchBuffer[WATCH_BUFFER_SIZE_EXT * WATCH_BUFFER_COUNT];
};
#endif

struct Windows86OutputConfig
{
    DynamicString* outputPath;
};

struct Windows64OutputConfig
{
    DynamicString* outputPath;
};

struct AndroidOutputConfig
{
    DynamicString* outputPath;
};

struct WASMOutputConfig
{
    DynamicString* outputPath;
};

struct EditorConfigWindow
{
    bool open;

    DynamicString* runtimesPath;
    DynamicString* dataPath;
};

struct HelpWindow
{
    bool open;
};

#endif