#ifndef EDITOR_H
#define EDITOR_H

static i32   Stricmp(const char* str1, const char* str2)         { i32 d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
static i32   Strnicmp(const char* str1, const char* str2, i32 n) { i32 d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
static char* Strdup(const char *str, size_t *len)                { *len = strlen(str) + 1; void* buf = malloc(*len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, *len); }
static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

static void ClearLog(ConsoleWindow* console);

static void LogString(ConsoleWindow* console, const char* log, ConsoleLogType type);
void Log_(ConsoleWindow* console, ConsoleLogType type, const char* fmt, ...);
#define Log(console, fmt, ...) Log_(console, ConsoleLogType_NORMAL, fmt, ##__VA_ARGS__)
#define LogError(console, fmt, ...) Log_(console, ConsoleLogType_ERROR, fmt, ##__VA_ARGS__)
#define LogCommand(console, fmt, ...) Log_(console, ConsoleLogType_COMMAND, fmt, ##__VA_ARGS__)

ConsoleWindow editorConsole;
static void EditorInit(ConsoleWindow* console);
static void EditorDraw(ConsoleWindow* console);

static HelpWindow editorHelp;
static void EditorInit(HelpWindow* help);
static void EditorDraw(HelpWindow* help);

static TextureDebuggerWindow editorTextureDebugger;
static void EditorInit(TextureDebuggerWindow* debugger);
static void EditorDraw(TextureDebuggerWindow* debugger);

#ifdef LUA_SCRIPTING_ENABLED
static LUADebuggerWindow editorLUADebugger;
static void EditorInit(LUADebuggerWindow* debugger);
static void EditorDraw(LUADebuggerWindow* debugger);
#endif

static i32 TextEditCallback(ConsoleWindow* console, ImGuiInputTextCallbackData* data);
static i32 TextEditCallbackStub(ImGuiInputTextCallbackData* data);

static void ExecCommand(ConsoleWindow* console, const char* command_line);

static void EditorDrawAllOpen();

#endif