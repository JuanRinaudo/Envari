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

struct HelpWindow
{
    bool open;
};

enum DebugMenuAction {
    DebugMenuAction_None,
    DebugMenuAction_GoToFunction,
    DebugMenuAction_BreakOnFunction,
};

enum ConsoleLogType
{
    LOGTYPE_NORMAL,
    LOGTYPE_COMMAND,
    LOGTYPE_ERROR,
};

struct ConsoleLog
{
    char* log;
    i32 count;
    ConsoleLogType type;
};

struct ConsoleWindow
{
    char inputBuffer[CONSOLE_INPUT_BUFFER_COUNT];
    ImVector<ConsoleLog> items;
    ImVector<const char*> commands;
    ImVector<char*> history;
    int historyPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter filter;
    bool autoScroll;
    bool scrollToBottom;
    bool open;
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

#endif