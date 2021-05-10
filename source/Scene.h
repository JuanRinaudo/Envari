#ifndef SCENE_H
#define SCENE_H

#ifdef LUA_SCRIPTING_ENABLED
#define SCENE_FILEPATH_SIZE 256
char sceneFilepath[SCENE_FILEPATH_SIZE];

const char* GetSceneFilepath()
{
    return sceneFilepath;
}

void LoadLUAScene(const char* luaFilepath)
{
    GL_CleanCache();

    Assert(strlen(luaFilepath) < SCENE_FILEPATH_SIZE, "LUA file path is too big, make SCENE_FILEPATH_SIZE bigger or change it");

    strcpy(sceneFilepath, luaFilepath);

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