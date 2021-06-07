#ifndef SCENE_H
#define SCENE_H

#ifdef LUA_ENABLED
#define SCENE_FILEPATH_SIZE 256
char sceneFilepath[SCENE_FILEPATH_SIZE];

const char* GetSceneFilepath()
{
    return sceneFilepath;
}

void UnloadLUAScene()
{
    GL_CleanCache();

    if(sceneFilepath[0] != 0) {
        strcpy(sceneFilepath, "");

        ResetArena(&sceneState->arena);

        lua["Load"] = sol::function();
        lua["Update"] = sol::function();
        lua["Unload"] = sol::function();
        lua["End"] = sol::function();
        
        RunLUAProtectedFunction(Unload)
    }
}

void LoadLUAScene(const char* luaFilepath)
{
    AssertMessage(strlen(luaFilepath) < SCENE_FILEPATH_SIZE, "LUA file path is too big, make SCENE_FILEPATH_SIZE bigger or change it");

    UnloadLUAScene();

    strcpy(sceneFilepath, luaFilepath);

    LoadScriptFile(luaFilepath);

    RunLUAProtectedFunction(Load)
}
#endif

#endif