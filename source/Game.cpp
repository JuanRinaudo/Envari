#define SOL_ALL_SAFETIES_ON 1

#include "IMGUI/imgui.h"

#include "File.h"
#include "Data.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Render.h"
#include "GLRender.h"
#include "Input.h"
#include "Scripting.h"

static u32 GameInit()
{
    EditorInit(&editorConsole);

    LoadScriptFile(TableGetString(&initialConfig, WINDOWSCONFIG_INITLUASCRIPT));

    sol::protected_function Init(lua["Init"]);
    if(Init.valid()) {
        sol::protected_function_result result = Init();
        if (!result.valid()) {
            sol::error error = result;
		    std::string what = error.what();
            LogError(&editorConsole, "%s", what.c_str());
        }
    }
    else {
        LogError(&editorConsole, "Error on script 'Init', not valid");
    }
    
    return 0;
}

static u32 GameLoop()
{
    f32 fps = (f32)(1 / gameState->time.deltaTime);

    EditorDrawAllOpen();
    
    sol::protected_function Update(lua["Update"]);
    if(Update.valid()) {
        sol::protected_function_result result = Update();
        if (!result.valid()) {
            sol::error error = result;
		    std::string what = error.what();
            LogError(&editorConsole, "%s", what.c_str());
        }
    }
    else {
        LogError(&editorConsole, "Error on script 'Update', not valid");
    }

    // ImGui::ShowDemoWindow();

    return 0;
}

static u32 GameEnd()
{
    return 0;
}

#include "Editor.cpp"