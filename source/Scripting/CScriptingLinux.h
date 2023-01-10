#ifndef C_SCRIPTING_LINUX_H
#define C_SCRIPTING_LINUX_H

#include <dlfcn.h>

const char* CSCRIPTING_SO = "cscripting.so";

static void CScriptingInit()
{
    ChangeLogFlag(LogFlag_C_SCRIPTING);

    char* error;

    CScriptingStop();
    
    clibrary = dlopen(CSCRIPTING_SO, RTLD_NOW | RTLD_GLOBAL);

    if (!clibrary) {
        Log("%s\n", dlerror());
    }
    else {
        cInitFunction = (void (*)())dlsym(clibrary, "CInit");
        cUpdateFunction = (void (*)())dlsym(clibrary, "CUpdate");
        cEndFunction = (void (*)())dlsym(clibrary, "CEnd");
        cEditorInitFunction = (void (*)())dlsym(clibrary, "CEditorInit");
        cEditorUpdateFunction = (void (*)())dlsym(clibrary, "CEditorUpdate");
        cEditorEndFunction = (void (*)())dlsym(clibrary, "CEditorEnd");
        cEditorConsoleDebugBarFunction = (void (*)())dlsym(clibrary, "CEditorConsoleDebugBar");
        cEditorShaderReloadFunction = (void (*)())dlsym(clibrary, "CEditorShaderReload");
        cFocusChangeFunction = (void (*)())dlsym(clibrary, "CFocusChange");
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
        dlclose(clibrary);
        clibrary = NULL;
    }
}

#endif