#ifndef LUA_SCRIPTING_H
#define LUA_SCRIPTING_H

#define RunLUAProtectedFunction(FUNCTION) sol::protected_function Func ## FUNCTION (lua[#FUNCTION]); \
ChangeLogFlag(LogFlag_SCRIPTING_FUNCTIONS); \
if(Func ## FUNCTION .valid()) { \
    sol::protected_function_result result = Func ## FUNCTION (); \
    if (!result.valid()) { \
        sol::error error = result; \
        std::string what = error.what(); \
        LogError("%s", what.c_str()); \
    } \
} \
else { \
    LogWarning("Function '"#FUNCTION"', not valid"); \
}

// extern "C" {
//     #include "luasocket.h"
//     #include "mime.h"
// }

extern void ScriptingBindings();
extern void ScriptingMathBindings();

void LoadScriptString(const char* string)
{
    sol::load_result loadResult = lua.load(string);

    if(loadResult.valid()) {
        sol::protected_function_result result = loadResult();
        if(!result.valid()) {
            sol::error luaError = loadResult;
            std::string errorReport = luaError.what();
            LogError(errorReport.c_str());
        }
    }
    else {
        sol::error luaError = loadResult;
        std::string errorReport = luaError.what();
        LogError(errorReport.c_str());
    }
}

void LoadScriptFile(const char* filePath)
{
    sol::load_result loadResult = lua.load_file(filePath);

    if(loadResult.valid()) {
        sol::protected_function_result result = loadResult();
        if(!result.valid()) {
            sol::error luaError = loadResult;
            std::string errorReport = luaError.what();
            LogError(errorReport.c_str());
        }
    }
    else {
        sol::error luaError = loadResult;
        std::string errorReport = luaError.what();
        LogError(errorReport.c_str());
    }

    #ifdef PLATFORM_EDITOR
    if(!Strstrn(editorLUADebugger.watchList, filePath, (i32)editorLUADebugger.watchListSize)) {
        editorLUADebugger.watchListTimes[editorLUADebugger.watchFiles] = filesystem::last_write_time(filePath);
        editorLUADebugger.watchFiles++;

        strcpy(editorLUADebugger.watchList + editorLUADebugger.watchListSize, filePath);
        editorLUADebugger.watchListSize += strlen(filePath) + 1;
        editorLUADebugger.watchList[editorLUADebugger.watchListSize] = 0;

        assert(editorLUADebugger.watchListSize < ArrayCount(editorLUADebugger.watchList));
    }
    #endif
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
            auto string = filesystem::current_path().string();
            const char* workingDirectory = string.c_str();
            size_t workingDirectorySize = strlen(workingDirectory);

            char* packagePath = PushArray(&temporalState->arena, 512, char);
            
            size_t offset = 0;
            strcpy(packagePath, workingDirectory);
            offset += workingDirectorySize;
            const char* luaRelative = "/scripts/?.lua;";
            strcpy(packagePath + offset, luaRelative);
            offset += strlen(luaRelative);
            strcpy(packagePath + offset, workingDirectory);
            offset += workingDirectorySize;
            strcpy(packagePath + offset, "/scripts/envari/?.lua;");

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

void ScriptingDummy() {

}

void ScriptingInit()
{
    lua.set_panic(sol::c_call<decltype(&ScriptingPanic), &ScriptingPanic>);
	lua.set_exception_handler(&ScriptingExceptionHandler);

    ScriptingBindings();

    lua["Load"] = ScriptingDummy;
    lua["Update"] = ScriptingDummy;
    lua["Unload"] = ScriptingDummy;
    lua["EditorInit"] = ScriptingDummy;
    lua["EditorUpdate"] = ScriptingDummy;
    lua["EditorEnd"] = ScriptingDummy;
    lua["EditorConsoleDebugBar"] = ScriptingDummy;
    lua["EditorShaderReload"] = ScriptingDummy;
    lua["FocusChange"] = ScriptingDummy;
}

void ScriptingReset()
{
    lua = {};
    ScriptingInit();
}

static u32 ScriptingUpdate()
{
    ChangeLogFlag(LogFlag_SCRIPTING);

    RunLUAProtectedFunction(Update)

    return 1;
}

#if PLATFORM_EDITOR && PLATFORM_WINDOWS
void ScriptingDebugStart()
{
    // luaopen_socket_core(lua);
    // luaopen_mime_core(lua);

    // lua.script("local json = require(\"dkjson\");\n"
    // "local debuggee = require(\"vscode-debuggee\")\n"
    // "local startResult, breakerType = debuggee.start(json)\n"
    // "LogConsole(\"debuggee start -> \" .. tostring(startResult) .. \" \" .. tostring(breakerType))");
}
#endif

void ScriptingWatchChanges()
{
    #ifdef PLATFORM_EDITOR
    i32 nameIndex = 0;
    i32 fileIndex = 0;
    i32 watchIndex = 0;
    char name[LUA_FILENAME_MAX];
    while(watchIndex < editorLUADebugger.watchListSize) {
        if(editorLUADebugger.watchList[watchIndex] == 0) {
            name[nameIndex] = 0;

            auto fileTime = filesystem::last_write_time(name);
            if(fileTime != editorLUADebugger.watchListTimes[fileIndex]) {
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

                editorLUADebugger.watchListTimes[fileIndex] = fileTime;
            }

            fileIndex++;
            nameIndex = -1;
        }
        else {
            name[nameIndex] = editorLUADebugger.watchList[watchIndex];
        }
        nameIndex++;
        watchIndex++;
    }
    #endif
}

#if PLATFORM_EDITOR
void GetWatchValue(i32 watchType, char* name, char* valueBuffer)
{
    i32 pushCount = 0;
    u32 type = LUA_TNIL;
    bool parsingArrayIndex = false;
    if(strchr(name, '.') || strchr(name, '[')) {
        char innerBuffer[WATCH_BUFFER_SIZE_EXT];
        u32 nameIndex = 0;
        u32 bufferIndex = 0;
        size_t nameSize = strlen(name);
        bool firstSection = true;
        while(nameIndex < nameSize) {
            if(name[nameIndex] == '[' || name[nameIndex] == ']' || name[nameIndex] == '.') {
                innerBuffer[bufferIndex] = 0;
                if(firstSection) {
                    type = lua_getglobal(lua, innerBuffer);
                    pushCount++;
                    firstSection = false;
                    bufferIndex = 0;
                }
                
                if(name[nameIndex] == '[' || name[nameIndex] == '.') {
                    if(bufferIndex > 0) {
                        type = lua_getfield(lua, -1, innerBuffer);
                        pushCount++;
                    }

                    if(name[nameIndex] == '[') {
                        parsingArrayIndex = true;
                    }
                }
                else if(name[nameIndex] == ']') {
                    if(parsingArrayIndex) {
                        innerBuffer[bufferIndex] = 0;
                        parsingArrayIndex = false;
                        i32 index = StringToInt(innerBuffer);
                        type = lua_rawgeti(lua, -1, index);
                        pushCount++;
                        bufferIndex = 0;

                        if(type == LUA_TNIL) {
                            // #TODO (Juan): Handle nil
                            lua_pop(lua, pushCount);
                            return;
                        }
                    }
                    else {
                        lua_pop(lua, pushCount);
                        return;
                    }
                }
                else {
                    lua_pop(lua, pushCount);
                    return;
                }

                if(type == LUA_TNIL) {
                    // #TODO (Juan): Handle nil
                    lua_pop(lua, pushCount);
                    return;
                }

                bufferIndex = 0;
            }
            else {
                innerBuffer[bufferIndex] = name[nameIndex];
                ++bufferIndex;
            }

            ++nameIndex;
        }

        if(bufferIndex > 0) {
            innerBuffer[bufferIndex] = 0;
            type = lua_getfield(lua, -1, innerBuffer);
            pushCount++;
        }
    }
    else {
        type = lua_getglobal(lua, name);
        pushCount++;
    }
        
    if(type == LUA_TNIL || parsingArrayIndex) {
        // #TODO (Juan): Handle error
        lua_pop(lua, pushCount);
        return;
    }

    if(watchType == WatchType_AUTO) {
        if(lua_isinteger(lua, -1)) {
            i32 value = (i32)lua_tointeger(lua, -1);
            sprintf(valueBuffer, "%d", value);
        }
        else if(lua_isnumber(lua, -1)) {
            float value = (float)lua_tonumber(lua, -1);
            sprintf(valueBuffer, "%f", value);
        }
        else if(lua_isboolean(lua, -1)) {
            bool value = (bool)lua_toboolean(lua, -1);
            sprintf(valueBuffer, value ? "true" : "false");
        }
        else if(lua_isstring(lua, -1)) {
            const char* value = lua_tostring(lua, -1);
            sprintf(valueBuffer, "%s", value);
        }
        else if(lua_istable(lua, -1)) {
            u64 length = lua_rawlen(lua, -1);
            if(length) {
                sprintf(valueBuffer, "[array](%llu)", length);
            }
            else {
                sprintf(valueBuffer, "[table]");
            }
        }
    }
    else if(watchType == WatchType_INT && lua_isinteger(lua, -1)) {
        i32 value = (i32)lua_tointeger(lua, -1);
        sprintf(valueBuffer, "%d", value);
    }
    else if(watchType == WatchType_FLOAT && lua_isnumber(lua, -1)) {
        float value = (float)lua_tonumber(lua, -1);
        sprintf(valueBuffer, "%f", value);
    }
    else if(watchType == WatchType_BOOL && lua_isboolean(lua, -1)) {
        bool value = (bool)lua_toboolean(lua, -1);
        sprintf(valueBuffer, value ? "true" : "false");
    }
    else if(watchType == WatchType_STRING && lua_isstring(lua, -1)) {
        const char* value = lua_tostring(lua, -1);
        sprintf(valueBuffer, "%s", value);
    }

    lua_pop(lua, pushCount);
}
#endif

#endif