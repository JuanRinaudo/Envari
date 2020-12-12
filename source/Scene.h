#ifndef SCENE_H
#define SCENE_H

#ifdef LUA_SCRIPTING_ENABLED
void LoadLUAScene(const char* luaFilepath)
{
    GL_CleanCache();

    ResetArena(&sceneState->arena);

    lua["Init"] = sol::function();
    lua["Update"] = sol::function();
    lua["Unload"] = sol::function();
    lua["End"] = sol::function();

    LoadScriptFile(luaFilepath);

    sol::protected_function Init(lua["Init"]);
    if(Init.valid()) {
        sol::protected_function_result result = Init();
        if (!result.valid()) {
            sol::error error = result;
		    std::string what = error.what();
            LogError("%s", what.c_str());
        }
    }
    else {
        LogError("Error on script 'Init', not valid");
    }
}
#endif

#endif