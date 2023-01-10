#ifndef C_SCRIPTING_H
#define C_SCRIPTING_H

void* clibrary;

void (*cInitFunction)(void);
void (*cUpdateFunction)(void);
void (*cEndFunction)(void);
void (*cEditorInitFunction)(void);
void (*cEditorUpdateFunction)(void);
void (*cEditorEndFunction)(void);
void (*cEditorConsoleDebugBarFunction)(void);
void (*cEditorShaderReloadFunction)(void);
void (*cFocusChangeFunction)(void);

extern "C" {
    void CInit() { if(cInitFunction) (*cInitFunction)(); }
    void CUpdate() { if(cUpdateFunction) (*cUpdateFunction)(); }
    void CEnd() { if(cEndFunction) (*cEndFunction)(); }
    void CEditorInit() { if(cEditorInitFunction) (*cEditorInitFunction)(); }
    void CEditorUpdate() { if(cEditorUpdateFunction) (*cEditorUpdateFunction)(); }
    void CEditorEnd() { if(cEditorConsoleDebugBarFunction) (*cEditorConsoleDebugBarFunction)(); }
    void CEditorConsoleDebugBar() { if(cEditorConsoleDebugBarFunction) (*cEditorConsoleDebugBarFunction)(); }
    void CEditorShaderReload() { if(cEditorShaderReloadFunction) (*cEditorShaderReloadFunction)(); }
    void CFocusChange() { if(cFocusChangeFunction) (*cFocusChangeFunction)(); }
}

static void CScriptingInit();
static void CScriptingStop();

#if PLATFORM_LINUX
#include <CScriptingLinux.h>
#elif PLATFORM_WINDOWS
#include <CScriptingWindows.h>
#endif

#endif