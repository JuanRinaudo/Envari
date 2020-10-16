static void ClearLog(ConsoleWindow* console)
{
    for (i32 i = 0; i < console->items.Size; i++) {
        free(console->items[i].log);
    }
    console->items.clear();
}

static void LogString(ConsoleWindow* console, const char* log, ConsoleLogType type = ConsoleLogType_NORMAL)
{
    ConsoleLog* lastLog = console->items.Size > 0 ? &console->items.back() : 0;
    if(lastLog && strcmp(log, lastLog->log) == 0) {
        lastLog->count++;
    }
    else {
        ConsoleLog newLog;
        newLog.log = Strdup(log, &newLog.size);
        newLog.count = 1;
        newLog.type = type;
        console->items.push_back(newLog);
    }
}

void Log_(ConsoleWindow* console, ConsoleLogType type, const char* fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, ArrayCount(buffer), fmt, args);
    va_end(args);
    buffer[ArrayCount(buffer)-1] = 0;
    LogString(console, buffer, type);
}

static void EditorInit(ConsoleWindow* console)
{
    ClearLog(console);
    memset(console->inputBuffer, 0, sizeof(console->inputBuffer));
    console->historyPos = -1;
    console->commands.push_back("HELP");
    console->commands.push_back("HISTORY");
    console->commands.push_back("CLEAR");
#ifdef LUA_SCRIPTING_ENABLED
    console->commands.push_back("LUA");
#endif
    console->autoScroll = true;
    console->scrollToBottom = false;

    console->open = true;
    
    Log(console, "Envari Console Start");
}

static void EditorInit(PreviewWindow* preview)
{
    preview->open = true;
}

static void EditorInit(RenderDebuggerWindow* debugger)
{
    debugger->open = true;
}

static void EditorInit(MemoryDebuggerWindow* debugger)
{
    debugger->open = true;
}

static void EditorInit(TextureDebuggerWindow* debugger)
{
    debugger->open = true;

    debugger->textureID = 0;
    debugger->inspectMode = TextureInspect_CACHE;
}

#ifdef LUA_SCRIPTING_ENABLED
static void EditorInit(LUADebuggerWindow* debugger)
{
    debugger->open = true;

    if(TableHasKey(initialConfig, INITLUASCRIPT)) {
        debugger->currentFile = (char*)LoadFileToMemory(TableGetString(&initialConfig, INITLUASCRIPT), FILE_MODE_READ_BINARY, &debugger->currentFileSize);
    }
}
#endif

static void EditorInit(HelpWindow* help)
{
    help->open = true;
}

static i32 TextEditCallback(ConsoleWindow* console, ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCompletion: {
            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (i32 i = 0; i < console->commands.Size; i++) {
                if (Strnicmp(console->commands[i], word_start, (int)(word_end-word_start)) == 0) {
                    candidates.push_back(console->commands[i]);
                }
            }

            if (candidates.Size == 0) {
                // No match
                Log(console, "No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
            }
            else if (candidates.Size == 1) {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                data->DeleteChars((i32)(word_start-data->Buf), (i32)(word_end-word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                i32 match_len = (i32)(word_end - word_start);
                while(true) {
                    i32 c = 0;
                    bool all_candidates_matches = true;
                    for (i32 i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((i32)(word_start - data->Buf), (i32)(word_end-word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                Log(console, "Possible matches:\n");
                for (i32 i = 0; i < candidates.Size; i++) {
                    Log(console, "- %s\n", candidates[i]);
                }
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory: {
            // Example of HISTORY
            const i32 prev_history_pos = console->historyPos;
            if (data->EventKey == ImGuiKey_UpArrow) {
                if (console->historyPos == -1) {
                    console->historyPos = console->history.Size - 1;
                }
                else if (console->historyPos > 0) {
                    console->historyPos--;
                }
            }
            else if (data->EventKey == ImGuiKey_DownArrow) {
                if (console->historyPos != -1) {
                    if (++console->historyPos >= console->history.Size) {
                        console->historyPos = -1;
                    }
                }
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != console->historyPos) {
                const char* history_str = (console->historyPos >= 0) ? console->history[console->historyPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
    }
    return 0;
}

static i32 TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    ConsoleWindow* console = (ConsoleWindow*)data->UserData;
    return TextEditCallback(console, data);
}

static void ExecCommand(ConsoleWindow* console, const char* command_line)
{
    // #NOTE(Juan): Separate the command from the arguments
    char command[CONSOLE_INPUT_BUFFER_COUNT];
    strcpy(command, command_line);
    i32 argumentStart = 0;
    while(command[argumentStart] > SPECIAL_ASCII_CHAR_OFFSET) {
        argumentStart++;
    }
    command[argumentStart] = 0;

    LogString(console, command, ConsoleLogType_COMMAND);

    // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
    console->historyPos = -1;
    for (i32 i = console->history.Size-1; i >= 0; i--) {
        if (Stricmp(console->history[i], command_line) == 0) {
            free(console->history[i]);
            console->history.erase(console->history.begin() + i);
            break;
        }
    }
    size_t commandLineSize = 0;
    console->history.push_back(Strdup(command_line, &commandLineSize));

    // Process command
    if (Stricmp(command, "CLEAR") == 0) {
        ClearLog(console);
    }
    else if (Stricmp(command, "HELP") == 0) {
        Log(console, "Commands:");
        for (i32 i = 0; i < console->commands.Size; i++) {
            Log(console, "- %s", console->commands[i]);
        }
    }
    else if (Stricmp(command, "HISTORY") == 0) {
        i32 first = console->history.Size - 10;
        for (i32 i = first > 0 ? first : 0; i < console->history.Size; i++) {
            Log(console, "%3d: %s\n", i, console->history[i]);
        }
    }
#ifdef LUA_SCRIPTING_ENABLED
    else if (Stricmp(command, "LUA") == 0) {
        lua.script(command_line + argumentStart);
    }
#endif
    else {
        Log(console, "Unknown command: '%s'\n", command_line);
    }

    // On commad input, we scroll to bottom even if autoScroll==false
    console->scrollToBottom = true;
}

ConsoleLog *inspectedLog = 0;
static void EditorDraw(ConsoleWindow* console)
{
    if(!console->open) { return; };
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowSize(ImVec2(600,600), ImGuiCond_FirstUseEver);

    if (console->open && !ImGui::Begin("Console", &console->open, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("WIP")) { }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Preview")) { EditorInit(&editorPreview); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Render")) { EditorInit(&editorRenderDebugger); }
            if (ImGui::MenuItem("Memory")) { EditorInit(&editorMemoryDebugger); }
            if (ImGui::MenuItem("Textures")) { EditorInit(&editorTextureDebugger); }
#ifdef LUA_SCRIPTING_ENABLED
            if (ImGui::MenuItem("LUA")) { EditorInit(&editorLUADebugger); }
#endif
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About")) { EditorInit(&editorHelp); }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            console->open = false;
        ImGui::EndPopup();

        ImGui::OpenPopup("GoToFunction");
    }

    if (ImGui::SmallButton("Clear")) {
        ClearLog(console);
    }
    
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    
    ImGui::SameLine();
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &console->autoScroll);
        ImGui::EndPopup();
    }

    // Options, filter
    if (ImGui::Button("Options")) {
        ImGui::OpenPopup("Options");
    }
    ImGui::SameLine();
    console->filter.Draw("Filter", 180);
    ImGui::Separator();

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1));
    if (copy_to_clipboard) {
        ImGui::LogToClipboard();
    }

    // ImGui::PushTextWrapPos(ImGui::GetWindowSize().x * 0.92f);
    // ImGui::PopTextWrapPos();

    ImGui::BeginColumns("Logs", 2);
    ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x * 0.92f);

    for (i32 i = 0; i < console->items.Size; i++) {
        ConsoleLog* currentLog = &console->items[i];
        if (!console->filter.PassFilter(currentLog->log)) {
            continue;
        }

        switch(currentLog->type) {
            case ConsoleLogType_NORMAL: {
                
            } break;
            case ConsoleLogType_ERROR: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            } break;
            case ConsoleLogType_COMMAND: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
            } break;
        }

        i32 lastIndex = 0;
        i32 index = 0;
        while(index < currentLog->size) {
            if(currentLog->log[index] == '\n') {
                if(lastIndex != 0) {
                    ImGui::SameLine();
                }
                ImGui::TextUnformatted(currentLog->log + lastIndex, currentLog->log + index);
                ImGui::SameLine();
                ImGui::TextUnformatted(" ; ");
                lastIndex = index + 1;
            }
            ++index;
        }
        if(lastIndex != 0) {
            ImGui::SameLine();
        }
        ImGui::TextUnformatted(currentLog->log + lastIndex, currentLog->log + currentLog->size - 1);
        
        if (currentLog->type != ConsoleLogType_NORMAL) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::NextColumn();

    for (i32 i = 0; i < console->items.Size; i++) {
        ConsoleLog* currentLog = &console->items[i];
        if (!console->filter.PassFilter(currentLog->log)) {
            continue;
        }
        
        switch(currentLog->type) {
            case ConsoleLogType_NORMAL: {
                
            } break;
            case ConsoleLogType_ERROR: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            } break;
            case ConsoleLogType_COMMAND: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
            } break;
        }
        
        ImGui::Text("%d", currentLog->count);

        if(ImGui::IsItemClicked()) {
            ImGui::OpenPopup("LogDetails");
            inspectedLog = currentLog;
        }
        
        if (currentLog->type != ConsoleLogType_NORMAL) {
            ImGui::PopStyleColor();
        }
    }

    if (inspectedLog && ImGui::BeginPopup("LogDetails")) {
        ImGui::TextUnformatted(inspectedLog->log);
        ImGui::EndPopup();
    } else {
        inspectedLog = 0;
    }

    ImGui::EndColumns();

    if (copy_to_clipboard) {
        ImGui::LogFinish();
    }

    if (console->scrollToBottom || (console->autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
        ImGui::SetScrollHereY(1.0f);
    }
    console->scrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    if (ImGui::InputText("Input", console->inputBuffer, IM_ARRAYSIZE(console->inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)&console)) {
        char* s = console->inputBuffer;
        Strtrim(s);
        if (s[0])
            ExecCommand(console, s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus) {
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    ImGui::End();
}

static void EditorDraw(PreviewWindow* preview)
{
    if(!preview->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (!ImGui::Begin("Preview", &preview->open))
    {
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }
    ImGui::PopStyleVar();

    ImVec2 size = ImGui::GetWindowSize();
    
    ImVec2 contentStart = ImGui::GetWindowContentRegionMin();
    size.y -= contentStart.y;

    gameState->render.size.y = size.y;
    gameState->render.size.x = size.y * gameState->camera.ratio;
    
    f32 offsetX = (size.x - gameState->render.size.x) * 0.5f;

    ImGui::SetCursorPos(ImVec2(offsetX, contentStart.y));

    ImVec2 cursorPosition = ImGui::GetMousePos();
    ImVec2 previewMin = ImGui::GetWindowPos();
    previewMin.x += offsetX;
    ImVec2 previewMax = previewMin + ImGui::GetWindowSize();
    previewMax.x -= offsetX * 2;
    preview->cursorInsideWindow = cursorPosition.x > previewMin.x && cursorPosition.x < previewMax.x && cursorPosition.y > previewMin.y && cursorPosition.y < previewMax.y;
    preview->cursorPosition = V2(Clamp(cursorPosition.x - previewMin.x, 0, gameState->render.size.x), Clamp(cursorPosition.y - previewMin.y - contentStart.y, 0, gameState->render.size.y));
    
    ImGui::Image((ImTextureID)gameState->render.frameBuffer, ImVec2((f32)gameState->render.size.x, (f32)gameState->render.size.y), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::SetCursorPos(ImVec2(5, 20));
    ImGui::Text("Render Size: %d, %d", (i32)gameState->render.size.x, (i32)gameState->render.size.y);
    ImGui::SetCursorPos(ImVec2(5, 40));
    ImGui::Text("Buffer Size: %d, %d", (i32)gameState->render.bufferSize.x, (i32)gameState->render.bufferSize.y);

    ImGui::End();
}

static void EditorDraw(RenderDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Render Debugger", &debugger->open))
    {
        ImGui::End();
        return;
    }
    
    ImGui::Text("Render memory size: %d", debugger->renderMemory);
    ImGui::Text("Draw count: %d", debugger->drawCount);
    ImGui::Text("Program changes: %d", debugger->programChanges);
    
    ImGui::End();
}

static void EditorDraw(MemoryDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Memory Debugger", &debugger->open))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Permanent memory: %d / %d", permanentState->arena.used, permanentState->arena.size);
    ImGui::Text("Scene memory: %d / %d", sceneState->arena.used, sceneState->arena.size);
    ImGui::Text("Temporal memory: %d / %d", temporalState->arena.used, temporalState->arena.size);

#if LUA_SCRIPTING_ENABLED
    ImGui::Separator();
    ImGui::Text("LUA Memory: %d", (int)lua.memory_used());
    if (ImGui::SmallButton("Force GC")) {
        lua.collect_garbage();
    }
#endif
    
    ImGui::End();
}

static void EditorDraw(TextureDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Texture Debugger", &debugger->open))
    {
        ImGui::End();
        return;
    }

    if (ImGui::SmallButton("Cache")) {
        debugger->inspectMode = TextureInspect_CACHE;
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("All")) {
        debugger->inspectMode = TextureInspect_ALL;
    }

    ImGui::Separator();

    i32 textureID = 0;

    i32 textureCacheSize = shlen(textureCache);
    if(debugger->inspectMode == TextureInspect_CACHE && debugger->textureID < textureCacheSize)
    {
        ImGui::Text("Texture Cache ID: %d / %d", debugger->textureID + 1, textureCacheSize);
        
        ImGui::SameLine();
        if (ImGui::SmallButton("Prev")) {
            debugger->textureID--;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Next")) {
            debugger->textureID++;
        }

        if(debugger->textureID < 0) { debugger->textureID = textureCacheSize - 1; }
        if(debugger->textureID >= textureCacheSize) { debugger->textureID = 0; }

        GLTexture cachedTexture = textureCache[debugger->textureID].value;

        ImGui::Text("Texture Size: %d x %d", cachedTexture.width, cachedTexture.height);

        ImGui::Separator();

        debugger->textureWidth = cachedTexture.width;
        debugger->textureHeight = cachedTexture.height;

        textureID = cachedTexture.textureID;
    }
    else if(debugger->inspectMode == TextureInspect_ALL) {
        ImGui::InputInt("Texture ID", &debugger->textureID, 1, 1);
        ImGui::InputInt("Texture Width", &debugger->textureWidth, 1, 1);
        ImGui::InputInt("Texture Height", &debugger->textureHeight, 1, 1);

        textureID = debugger->textureID;
    }

    ImGui::Image((ImTextureID)textureID, ImVec2((f32)debugger->textureWidth, (f32)debugger->textureHeight));

    ImGui::End();
}

#ifdef LUA_SCRIPTING_ENABLED
static void EditorDraw(LUADebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (!ImGui::Begin("LUA Debugger", &debugger->open, ImGuiWindowFlags_MenuBar))
    {
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }
    ImGui::PopStyleVar();

    DebugMenuAction menuAction = DebugMenuAction_NONE;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("WIP")) { }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Go"))
        {
            if (ImGui::MenuItem("Go to function")) {
                menuAction = DebugMenuAction_GO_TO_FUNCTION;
            }
            ImGui::EndMenu();
        }
        
        if(ImGui::BeginMenu("Debug")) {
            if(debugger->debugging) {
                if (ImGui::MenuItem("Stop", "F5")) {
                    debugger->debugging = false;
                    Log(&editorConsole, "Debugging stopped");
                }
                if (ImGui::MenuItem("Step", "F10")) {
                    
                }
            }
            else {
                if (ImGui::MenuItem("Start", "F5")) {
                    debugger->debugging = true;
                    Log(&editorConsole, "Debugging started");

                    ScriptingDebugStart();
                    
                    LoadLUALibrary(sol::lib::debug);
                }
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Break on function")) {
                menuAction = DebugMenuAction_BREAK_ON_FUNCTION;
            }
            
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if(menuAction == DebugMenuAction_GO_TO_FUNCTION) { ImGui::OpenPopup("GoToFunction"); }
    if(menuAction == DebugMenuAction_BREAK_ON_FUNCTION) { ImGui::OpenPopup("BreakOnFunction"); }

    if (ImGui::BeginPopup("GoToFunction"))
    {
        ImGui::Text("Go to function");

        if (ImGui::InputText("Input", debugger->inputBuffer, IM_ARRAYSIZE(debugger->inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue, 0, 0)) {
            char* s = debugger->inputBuffer;

	        lua_Debug debugInfo;
            i32 function = lua_getglobal(lua, debugger->inputBuffer);
            if(function > 0) {
                lua_getinfo(lua, ">S", &debugInfo);
                Log(&editorConsole, "Function found %s:%d", debugInfo.source, debugInfo.linedefined);

                if(debugger->currentFile) {
                    UnloadFileFromMemory(debugger->currentFile);
                }

                debugger->currentFile = (char*)LoadFileToMemory(debugInfo.source + 1, FILE_MODE_READ_BINARY, &debugger->currentFileSize);
            }
            else {
                Log(&editorConsole, "Function not found %s", debugger->inputBuffer);
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("BreakOnFunction"))
    {
        ImGui::Text("Break on function");
        
        if (ImGui::InputText("Input", debugger->inputBuffer, IM_ARRAYSIZE(debugger->inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue, 0, 0)) {
            char* s = debugger->inputBuffer;

	        lua_Debug debugInfo;
            i32 function = lua_getglobal(lua, debugger->inputBuffer);
            if(function > 0) {
                lua_getinfo(lua, ">S", &debugInfo);
                Log(&editorConsole, "Function found %s:%d", debugInfo.source, debugInfo.linedefined);

                // lua_sethook(lua, 0, 0, 0);
                // lua_sethook(lua, function, LUA_MASKCALL, 0);
            }
            else {
                Log(&editorConsole, "Function not found %s", debugger->inputBuffer);
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if(debugger->currentFile) {
        ImVec2 size = ImGui::GetWindowSize();
        i32 line = 1;
        i32 index = 0;

        ImGui::BeginColumns("Text Editor", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
        f32 lineSize = size.x * 0.08f;
        ImGui::SetColumnWidth(0, lineSize);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::Dummy(ImVec2(0, 3));
        while(index <= debugger->currentFileSize) {
            if(debugger->currentFile[index] == '\r') { ++index; }
            if(debugger->currentFile[index] == '\n' || debugger->currentFile[index] == 0) {
                ImGui::Text("%d         ", line);
                // if(ImGui::IsItemClicked()) {
	            //     LogCommand(&editorConsole, "%d", line);
                // }
                ++line;
            }
            ++index;
        }

        ImGui::NextColumn();

        index = 0;
        i32 lastIndex = 0;
        ImGui::Dummy(ImVec2(0, 3));
        while(index <= debugger->currentFileSize) {
            if(debugger->currentFile[index] == '\r') { ++index; }
            if(debugger->currentFile[index] == '\n' || debugger->currentFile[index] == 0) {
                ImGui::TextUnformatted(debugger->currentFile + lastIndex, debugger->currentFile + index);
                lastIndex = index + 1;
                ++line;
            }
            ++index;
        }
        // size.x -= lineSize;
        // ImGui::InputTextMultiline("", debugger->currentFile, debugger->currentFileSize, size);

        ImGui::PopStyleVar();
        ImGui::EndColumns();
    }
    else {
        ImGui::Text("No file loaded");
    }

    ImGui::End();
}
#endif

static void EditorDraw(HelpWindow* help)
{    
    if(!help->open) { return; };
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowSize(ImVec2(600,600), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("Help", &help->open, ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::Separator();
        
    ImGui::Text("Envari help");

    ImGui::Separator();

    ImGui::End();
}

static void EditorInit()
{
    SerializableTable* editorSave = 0;
    DeserializeTable(&permanentState->arena, &editorSave, "editor.save");
    
    editorConsole.open = TableGetBool(&editorSave, "editorConsoleOpen");
    if(editorConsole.open) { EditorInit(&editorConsole); }

    editorPreview.open = TableGetBool(&editorSave, "editorPreviewOpen");
    if(editorPreview.open) { EditorInit(&editorPreview); }

    editorRenderDebugger.open = TableGetBool(&editorSave, "editorRenderDebuggerOpen");
    if(editorRenderDebugger.open) { EditorInit(&editorRenderDebugger); }

    editorMemoryDebugger.open = TableGetBool(&editorSave, "editorMemoryDebuggerOpen");
    if(editorMemoryDebugger.open) { EditorInit(&editorMemoryDebugger); }

    editorTextureDebugger.open = TableGetBool(&editorSave, "editorTextureDebuggerOpen");
    if(editorTextureDebugger.open) { EditorInit(&editorTextureDebugger); }

#ifdef LUA_SCRIPTING_ENABLED
    editorLUADebugger.open = TableGetBool(&editorSave, "editorLUADebuggerOpen");
    if(editorLUADebugger.open) { EditorInit(&editorLUADebugger); }
#endif
}

static void EditorDrawAllOpen()
{
    EditorDraw(&editorConsole);
    EditorDraw(&editorPreview);
    EditorDraw(&editorRenderDebugger);
    EditorDraw(&editorMemoryDebugger);
    EditorDraw(&editorTextureDebugger);
#ifdef LUA_SCRIPTING_ENABLED
    EditorDraw(&editorLUADebugger);
#endif
    EditorDraw(&editorHelp);
}

static void EditorEnd()
{
    SerializableTable* editorSave = 0;
    TableSetBool(&permanentState->arena, editorSave, "editorConsoleOpen", editorConsole.open);
    TableSetBool(&permanentState->arena, editorSave, "editorPreviewOpen", editorPreview.open);
    TableSetBool(&permanentState->arena, editorSave, "editorRenderDebuggerOpen", editorRenderDebugger.open);
    TableSetBool(&permanentState->arena, editorSave, "editorMemoryDebuggerOpen", editorMemoryDebugger.open);
    TableSetBool(&permanentState->arena, editorSave, "editorTextureDebuggerOpen", editorTextureDebugger.open);
#ifdef LUA_SCRIPTING_ENABLED
    TableSetBool(&permanentState->arena, editorSave, "editorLUADebuggerOpen", editorLUADebugger.open);
#endif
    SerializeTable(&editorSave, "editor.save");
}