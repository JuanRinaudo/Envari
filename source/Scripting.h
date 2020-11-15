#ifndef SCRIPTING_H
#define SCRIPTING_H

extern "C" {
    #include "luasocket.h"
    #include "mime.h"
}

#ifdef GAME_EDITOR
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

    #ifdef GAME_EDITOR
    watchListTimes[watchFiles] = std::filesystem::last_write_time(filePath);
    watchFiles++;

    strcat(watchList, filePath);
    strcat(watchList, "@");
    watchListSize += (u32)strlen(filePath) + 1;
    #endif
}

void LoadScriptFile(const char* filePath)
{
    LoadScriptFile((char*)filePath);
}

void ScriptingPanic(sol::optional<std::string> maybe_msg) {
    LogError("Lua is in a panic state and will now abort() the application");
	if (maybe_msg) {
	    LogError("%s", maybe_msg.value().c_str());
	}
	// When this function exits, Lua will exhibit default behavior and abort()
}

i32 ScriptingExceptionHandler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	LogError("An exception occurred in a function, here's what it says");
	if (maybe_exception) {
		const std::exception& exception = *maybe_exception;
        std::string what = exception.what();
	    LogError("(straight from the exception): %s", what.c_str());
	}
	else {
	    LogError("(from the description parameter): %s %d", description.data(), description.size());
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
            auto string = std::filesystem::current_path().string();
            const char* workingDirectory = string.c_str();
            i32 workingDirectorySize = strlen(workingDirectory);

            char* packagePath = PushArray(&temporalState->arena, 512, char);
            
            i32 offset = 0;
            strcpy(packagePath, workingDirectory);
            offset += workingDirectorySize;
            const char* luaRelative = "\\scripts\\?.lua;";
            strcpy(packagePath + offset, luaRelative);
            offset += strlen(luaRelative);
            strcpy(packagePath + offset, workingDirectory);
            offset += workingDirectorySize;
            strcpy(packagePath + offset, "\\scripts\\envari\\?.lua;");

            lua["package"]["path"] = packagePath;
            
            break;
        }
        case sol::lib::math: {
            ScriptingMathBindings();
            break;
        }
        default: {
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

#ifdef GAME_EDITOR
void ScriptingDebugStart()
{
    luaopen_socket_core(lua);
    luaopen_mime_core(lua);

    lua.script("local json = require(\"dkjson\");\n"
    "local debuggee = require(\"vscode-debuggee\")\n"
    "local startResult, breakerType = debuggee.start(json)\n"
    "LogConsole(\"debuggee start -> \" .. tostring(startResult) .. \" \" .. tostring(breakerType))");
}
#endif

void ScriptingWatchChanges()
{
    #ifdef GAME_EDITOR
    i32 nameIndex = 0;
    i32 fileIndex = 0;
    i32 watchIndex = 0;
    char name[100];
    while(watchIndex < watchListSize) {
        if(watchList[watchIndex] == '@') {
            name[nameIndex] = 0;

            auto fileTime = std::filesystem::last_write_time(name);
            if(fileTime != watchListTimes[fileIndex]) {
                Log("Started to reload script %s", name);

                sol::load_result loadResult = lua.load_file(name);

                if(loadResult.valid()) {
                    sol::protected_function_result result = loadResult();
                    if(result.valid()) {

                    }
                    else {
                        sol::error luaError = loadResult;
                        std::string errorReport = luaError.what();
                        LogError("Scripting reload run error");
                        LogError(errorReport.c_str());
                    }
                }
                else {
                    sol::error luaError = loadResult;
                    std::string errorReport = luaError.what();
                    LogError("Scripting reload file error");
                    LogError(errorReport.c_str());
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