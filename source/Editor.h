#ifndef EDITOR_H
#define EDITOR_H

static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, len); }
static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

static void ClearLog(ConsoleWindow* console);

static void LogString(ConsoleWindow* console, const char* log, ConsoleLogType type);
void Log_(ConsoleWindow* console, ConsoleLogType type, const char* fmt, ...);
#define Log(console, fmt, ...) Log_(console, LOGTYPE_NORMAL, fmt, ##__VA_ARGS__)
#define LogError(console, fmt, ...) Log_(console, LOGTYPE_ERROR, fmt, ##__VA_ARGS__)
#define LogCommand(console, fmt, ...) Log_(console, LOGTYPE_COMMAND, fmt, ##__VA_ARGS__)

ConsoleWindow editorConsole;
static void EditorInit(ConsoleWindow* console);
static void EditorDraw(ConsoleWindow* console);

static HelpWindow editorHelp;
static void EditorInit(HelpWindow* help);
static void EditorDraw(HelpWindow* help);

#ifdef LUA_SCRIPTING_ENABLED
static LUADebuggerWindow editorLUADebugger;

static void EditorInit(LUADebuggerWindow* debugger);
static void EditorDraw(LUADebuggerWindow* debugger);
#endif

static int TextEditCallback(ConsoleWindow* console, ImGuiInputTextCallbackData* data);
static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);

static void ExecCommand(ConsoleWindow* console, const char* command_line);

static void EditorDrawAllOpen();

#endif