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

    RunLUAProtectedFunction(Init)
}
#endif

#endif