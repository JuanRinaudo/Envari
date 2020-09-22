#ifndef SCRIPTING_H
#define SCRIPTING_H

#ifdef GAME_INTERNAL
char watchList[200];
std::filesystem::file_time_type watchListTimes[20];
u32 watchListSize = 0;
u32 watchFiles = 0;
#endif

extern void ScriptingInitBindings();
extern void ScriptingMathBindings();

void LoadScriptFile(char* filePath)
{
    sol::load_result loadResult = lua.load_file(filePath);

    if(loadResult.valid()) {
        sol::protected_function_result result = loadResult();
        if(result.valid()) {
        }
        else {
            sol::error luaError = loadResult;
            std::string errorReport = luaError.what();
        }
    }
    else {
        sol::error luaError = loadResult;
        std::string errorReport = luaError.what();
    }

    #ifdef GAME_INTERNAL
    watchListTimes[watchFiles] = std::filesystem::last_write_time(filePath);
    watchFiles++;

    strcat(watchList, filePath);
    strcat(watchList, "@");
    watchListSize += (u32)strlen(filePath) + 1;
    #endif
}

void ScriptingPanic(sol::optional<std::string> maybe_msg) {
    LogError(&editorConsole, "Lua is in a panic state and will now abort() the application");
	if (maybe_msg) {
	    LogError(&editorConsole, "%s", maybe_msg.value().c_str());
	}
	// When this function exits, Lua will exhibit default behavior and abort()
}

int ScriptingExceptionHandler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	LogError(&editorConsole, "An exception occurred in a function, here's what it says");
	if (maybe_exception) {
		const std::exception& exception = *maybe_exception;
        std::string what = exception.what();
	    LogError(&editorConsole, "(straight from the exception): %s", what.c_str());
	}
	else {
	    LogError(&editorConsole, "(from the description parameter): %s %d", description.data(), description.size());
	}

	// #NOTE (Juan): Push string description to stack for function to recieve
	return sol::stack::push(L, description);
}

void LoadLUALibrary(sol::lib library)
{
    lua.open_libraries(library);

    // #NOTE (Juan): Add library extensions
    switch(library) {
        case sol::lib::package: {
            const char* workingDirectory = std::filesystem::current_path().string().c_str();
            i32 workingDirectorySize = strlen(workingDirectory);

            char* packagePath = PushArray(&temporalState->arena, workingDirectorySize + 7, char);
            
            i32 offset = 0;
            strcpy(packagePath, workingDirectory);
            offset += workingDirectorySize;
            strcpy(packagePath + offset, "/?.lua;");

            lua["package"]["path"] = packagePath;
            
            break;
        }
        case sol::lib::math: {
            ScriptingMathBindings();
            break;
        }
    }
}

void ScriptingInit()
{
    lua.set_panic(sol::c_call<decltype(&ScriptingPanic), &ScriptingPanic>);
	lua.set_exception_handler(&ScriptingExceptionHandler);

    ScriptingInitBindings();
}

void ScriptingWatchChanges()
{
    #ifdef GAME_INTERNAL
    i32 nameIndex = 0;
    i32 fileIndex = 0;
    i32 watchIndex = 0;
    char name[100];
    while(watchIndex < watchListSize) {
        if(watchList[watchIndex] == '@') {
            name[nameIndex] = 0;

            auto fileTime = std::filesystem::last_write_time(name);
            if(fileTime != watchListTimes[fileIndex]) {
                Log(&editorConsole, "Started to reload script %s", name);

                sol::load_result loadResult = lua.load_file(name);

                if(loadResult.valid()) {
                    sol::protected_function_result result = loadResult();
                    if(result.valid()) {

                    }
                    else {
                        sol::error luaError = loadResult;
                        std::string errorReport = luaError.what();
                        LogError(&editorConsole, "Scripting reload run error");
                        LogError(&editorConsole, errorReport.c_str());
                    }
                }
                else {
                    sol::error luaError = loadResult;
                    std::string errorReport = luaError.what();
                    LogError(&editorConsole, "Scripting reload file error");
                    LogError(&editorConsole, errorReport.c_str());
                }

                watchListTimes[fileIndex] = fileTime;
            }

            fileIndex++;
            nameIndex = -1;
        }
        else {
            name[nameIndex] = watchList[watchIndex];
        }
        nameIndex++;
        watchIndex++;
    }
    #endif
}

#endif