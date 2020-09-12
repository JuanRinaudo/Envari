#if !defined(EDITOR_H)
#define EDITOR_H

struct ConsoleWindow;

struct HelWindow
{
    bool open;
};

enum DebugMenuAction {
    DebugMenuAction_None,
    DebugMenuAction_GoToFunction,
    DebugMenuAction_BreakOnFunction,
};

struct LUADebuggerWindow
{
    bool open;

    bool debugging;
    char inputBuffer[256];
    char* currentFile;
    u32 currentFileSize;
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
    char inputBuffer[256];
    ImVector<ConsoleLog> items;
    ImVector<const char*> commands;
    ImVector<char*> history;
    int historyPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter filter;
    bool autoScroll;
    bool scrollToBottom;
    bool open;
};

static ConsoleWindow editorConsole;
static LUADebuggerWindow editorLUADebugger;
static HelWindow editorHelp;

static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, len); }
static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

static void ClearLog(ConsoleWindow* console);

static void LogString(ConsoleWindow* console, const char* log, ConsoleLogType type);
static void Log_(ConsoleWindow* console, ConsoleLogType type, const char* fmt, ...);
#define Log(console, fmt, ...) Log_(console, LOGTYPE_NORMAL, fmt, ##__VA_ARGS__)
#define LogError(console, fmt, ...) Log_(console, LOGTYPE_ERROR, fmt, ##__VA_ARGS__)
#define LogCommand(console, fmt, ...) Log_(console, LOGTYPE_COMMAND, fmt, ##__VA_ARGS__)

static void EditorInit(ConsoleWindow* console);
static void EditorInit(LUADebuggerWindow* debugger);
static void EditorInit(HelWindow* help);

static int TextEditCallback(ConsoleWindow* console, ImGuiInputTextCallbackData* data);
static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);

static void ExecCommand(ConsoleWindow* console, const char* command_line);
static void EditorDraw(ConsoleWindow* console);

static void EditorDraw(LUADebuggerWindow* debugger);
static void EditorDraw(HelWindow* help);

static void EditorDrawAllOpen();

#endif