#ifndef C_SCRIPTING_WINDOWS_H
#define C_SCRIPTING_WINDOWS_H

const char* CSCRIPTING_DLL = "cscripting.dll";

static void CScriptingInit()
{
    ChangeLogFlag(LogFlag_C_SCRIPTING);

    CScriptingStop();

    if (!clibrary) {
        
    }
    else {
    }
}

static void CScriptingStop()
{
    cInitFunction = NULL;
    cUpdateFunction = NULL;
    cEndFunction = NULL;
    cEditorInitFunction = NULL;
    cEditorUpdateFunction = NULL;
    cEditorEndFunction = NULL;
    cEditorConsoleDebugBarFunction = NULL;
    cEditorShaderReloadFunction = NULL;
    cFocusChangeFunction = NULL;

    if(clibrary) {
        
    }
}

#endif