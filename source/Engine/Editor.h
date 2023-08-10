#ifndef EDITOR_H
#define EDITOR_H

#include "EditorUtils.h"

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

EditorData *editorState;

PreviewWindow editorPreview;
static void EditorInit(PreviewWindow* preview);
static void EditorDraw(PreviewWindow* preview);

AssetsWindow assetsWindow;
static void EditorInit(AssetsWindow* debugger);
static void EditorDraw(AssetsWindow* debugger);

AssetsViewer assetsViewer;
static void EditorInit(AssetsViewer* debugger);
static void EditorDraw(AssetsViewer* debugger);

PerformanceDebuggerWindow editorPerformanceDebugger;
static void EditorInit(PerformanceDebuggerWindow* debugger);
static void EditorDraw(PerformanceDebuggerWindow* debugger);

RenderDebuggerWindow editorRenderDebugger;
static void EditorInit(RenderDebuggerWindow* debugger);
static void EditorDraw(RenderDebuggerWindow* debugger);

MemoryDebuggerWindow editorMemoryDebugger;
static void EditorInit(MemoryDebuggerWindow* debugger);
static void EditorDraw(MemoryDebuggerWindow* debugger);

TextureDebuggerWindow editorTextureDebugger;
static void EditorInit(TextureDebuggerWindow* debugger);
static void EditorDraw(TextureDebuggerWindow* debugger);

SoundDebuggerWindow editorSoundDebugger;
static void EditorInit(SoundDebuggerWindow* debugger);
static void EditorDraw(SoundDebuggerWindow* debugger);

InputDebuggerWindow editorInputDebugger;
static void EditorInit(InputDebuggerWindow* debugger);
static void EditorDraw(InputDebuggerWindow* debugger);

TimeDebuggerWindow editorTimeDebugger;
static void EditorInit(TimeDebuggerWindow* debugger);
static void EditorDraw(TimeDebuggerWindow* debugger);

ShaderDebuggerWindow editorShaderDebugger;
static void EditorInit(ShaderDebuggerWindow* debugger);
static void EditorDraw(ShaderDebuggerWindow* debugger);

#ifdef CSCRIPTING_ENABLED
CScriptingDebuggerWindow editorCScriptingDebugger;
static void EditorInit(CScriptingDebuggerWindow* debugger);
static void EditorDraw(CScriptingDebuggerWindow* debugger);
#endif

#ifdef LUA_ENABLED
LUADebuggerWindow editorLUADebugger;
static void EditorInit(LUADebuggerWindow* debugger);
static void EditorDraw(LUADebuggerWindow* debugger);
#endif

EditorConfigWindow editorConfig;
static void EditorInit(EditorConfigWindow* help);
static void EditorDraw(EditorConfigWindow* help);

HelpWindow editorHelp;
static void EditorInit(HelpWindow* help);
static void EditorDraw(HelpWindow* help);

static i32 TextEditCallback(ConsoleWindow* console, ImGuiInputTextCallbackData* data);
static i32 TextEditCallbackStub(ImGuiInputTextCallbackData* data);

static void ExecCommand(ConsoleWindow* console, const char* command_line);

static void EditorInit();

static Windows86OutputConfig windows86OutputConfig;
static Windows64OutputConfig windows64OutputConfig;
static AndroidOutputConfig androidOutputConfig;
static WASMOutputConfig wasmOutputConfig;

#endif