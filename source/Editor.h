#if !defined(EDITOR_H)
#define EDITOR_H

struct EnvariConsole;

struct EnvariHelp
{
    bool open;
};

enum DebugMenuAction {
    NONE,
    LUA_DEBUG_GOTOFUNCTION,
};

struct EnvariLUADebugger
{
    bool open;

    bool debugging;
    char inputBuffer[256];
    char* currentFile;
    u32 fileSize;
};

struct EnvariConsole
{
    char inputBuffer[256];
    ImVector<char*> items;
    ImVector<const char*> commands;
    ImVector<char*> history;
    int historyPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter filter;
    bool autoScroll;
    bool scrollToBottom;
    bool open;
};

static EnvariConsole editorConsole;
static EnvariLUADebugger editorLUADebugger;
static EnvariHelp editorHelp;

static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, len); }
static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

static void ClearLog(EnvariConsole* console);

static void AddBasicLog(EnvariConsole* console, const char* log);
static void AddLog(EnvariConsole* console, const char* fmt, ...) IM_FMTARGS(2);

static void EditorInit(EnvariConsole* console);
static void EditorInit(EnvariLUADebugger* debugger);
static void EditorInit(EnvariHelp* help);

static int TextEditCallback(EnvariConsole* console, ImGuiInputTextCallbackData* data);
static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);

static void ExecCommand(EnvariConsole* console, const char* command_line);
static void EditorDraw(EnvariConsole* console);

static void EditorDraw(EnvariLUADebugger* debugger);
static void EditorDraw(EnvariHelp* help);

static void EditorDrawAllOpen();

#endif