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
    CleanCache();

    if(sceneFilepath[0] != 0) {
        strcpy(sceneFilepath, "");

        ResetArena(&sceneState->arena);

        LUAClearFunctionBindings();
        
        RunLUAProtectedFunction(Unload)
    }
}

void LoadLUAScene(const char* luaFilepath)
{
    AssertMessage(strlen(luaFilepath) < SCENE_FILEPATH_SIZE, "LUA file path is too big, make SCENE_FILEPATH_SIZE bigger or change it");

    UnloadLUAScene();

    strcpy(sceneFilepath, luaFilepath);
    LoadLUAScriptFile(luaFilepath);
    RunLUAProtectedFunction(Load)

    gameState->game.sceneChanged = true;
    
#ifdef PLATFORM_EDITOR
    RunLUAProtectedFunction(EditorInit)
#endif
}
#endif

#endif