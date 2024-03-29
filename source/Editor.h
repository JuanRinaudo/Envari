#ifndef EDITOR_H
#define EDITOR_H

#include "EditorUtils.h"

static i32   Stricmp(const char* str1, const char* str2)         { i32 d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
static i32   Strnicmp(const char* str1, const char* str2, i32 n) { i32 d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
static char* Strdup(const char *str, size_t *len)                { *len = strlen(str) + 1; void* buf = malloc(*len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, *len); }
static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

static void ClearLog(ConsoleWindow* console);

static void LogString(ConsoleWindow* console, const char* log, ConsoleLogType type);
extern void Log_(ConsoleWindow* console, ConsoleLogType type, const char* file, u32 line, const char* fmt, ...);
#define Log(fmt, ...) Log_(&editorConsole, ConsoleLogType_NORMAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LogCommand(fmt, ...) Log_(&editorConsole, ConsoleLogType_COMMAND, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LogWarning(fmt, ...) Log_(&editorConsole, ConsoleLogType_WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LogError(fmt, ...) Log_(&editorConsole, ConsoleLogType_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

extern void ChangeLogFlag_(u32 newFlag);
#define ChangeLogFlag(newFlag) ChangeLogFlag_(newFlag)

ConsoleWindow editorConsole;
static void EditorInit(ConsoleWindow* console);
static void EditorDraw(ConsoleWindow* console);

PreviewWindow editorPreview;
static void EditorInit(PreviewWindow* preview);
static void EditorDraw(PreviewWindow* preview);

static AssetsWindow assetsWindow;
static void EditorInit(AssetsWindow* debugger);
static void EditorDraw(AssetsWindow* debugger);

static PerformanceDebuggerWindow editorPerformanceDebugger;
static void EditorInit(PerformanceDebuggerWindow* debugger);
static void EditorDraw(PerformanceDebuggerWindow* debugger);

static RenderDebuggerWindow editorRenderDebugger;
static void EditorInit(RenderDebuggerWindow* debugger);
static void EditorDraw(RenderDebuggerWindow* debugger);

static MemoryDebuggerWindow editorMemoryDebugger;
static void EditorInit(MemoryDebuggerWindow* debugger);
static void EditorDraw(MemoryDebuggerWindow* debugger);

static TextureDebuggerWindow editorTextureDebugger;
static void EditorInit(TextureDebuggerWindow* debugger);
static void EditorDraw(TextureDebuggerWindow* debugger);

static SoundDebuggerWindow editorSoundDebugger;
static void EditorInit(SoundDebuggerWindow* debugger);
static void EditorDraw(SoundDebuggerWindow* debugger);

static InputDebuggerWindow editorInputDebugger;
static void EditorInit(InputDebuggerWindow* debugger);
static void EditorDraw(InputDebuggerWindow* debugger);

static TimeDebuggerWindow editorTimeDebugger;
static void EditorInit(TimeDebuggerWindow* debugger);
static void EditorDraw(TimeDebuggerWindow* debugger);

#ifdef LUA_ENABLED
static LUADebuggerWindow editorLUADebugger;
static void EditorInit(LUADebuggerWindow* debugger);
static void EditorDraw(LUADebuggerWindow* debugger);
#endif

static EditorConfigWindow editorConfig;
static void EditorInit(EditorConfigWindow* help);
static void EditorDraw(EditorConfigWindow* help);

static HelpWindow editorHelp;
static void EditorInit(HelpWindow* help);
static void EditorDraw(HelpWindow* help);

static i32 TextEditCallback(ConsoleWindow* console, ImGuiInputTextCallbackData* data);
static i32 TextEditCallbackStub(ImGuiInputTextCallbackData* data);

static void ExecCommand(ConsoleWindow* console, const char* command_line);

static void EditorInit();
static void EditorDrawAllOpen();

static Windows86OutputConfig windows86OutputConfig;
static Windows64OutputConfig windows64OutputConfig;
static AndroidOutputConfig androidOutputConfig;
static WASMOutputConfig wasmOutputConfig;

#endif