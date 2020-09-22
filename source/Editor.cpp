static void ClearLog(ConsoleWindow* console)
{
    for (int i = 0; i < console->items.Size; i++) {
        free(console->items[i].log);
    }
    console->items.clear();
}

static void LogString(ConsoleWindow* console, const char* log, ConsoleLogType type = LOGTYPE_NORMAL)
{
    ConsoleLog* lastLog = console->items.Size > 0 ? &console->items.back() : 0;
    if(lastLog && strcmp(log, lastLog->log) == 0) {
        lastLog->count++;
    }
    else {
        ConsoleLog newLog;
        newLog.log = Strdup(log);
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
    console->autoScroll = true;
    console->scrollToBottom = false;

    console->open = true;
    
    Log(console, "Envari Console Start");
}

#ifdef LUA_SCRIPTING_ENABLED
static void EditorInit(LUADebuggerWindow* debugger)
{
    debugger->open = true;

    if(TableHasKey(&initialConfig, WINDOWSCONFIG_INITLUASCRIPT)) {
        debugger->currentFile = (char*)LoadFileToMemory(TableGetString(&initialConfig, WINDOWSCONFIG_INITLUASCRIPT), "rb", &debugger->currentFileSize);
    }
}
#endif

static void EditorInit(HelpWindow* help)
{
    help->open = true;
}

static int TextEditCallback(ConsoleWindow* console, ImGuiInputTextCallbackData* data)
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
            for (int i = 0; i < console->commands.Size; i++) {
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
                data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                int match_len = (int)(word_end - word_start);
                while(true) {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
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
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end-word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                Log(console, "Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++) {
                    Log(console, "- %s\n", candidates[i]);
                }
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory: {
            // Example of HISTORY
            const int prev_history_pos = console->historyPos;
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

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    ConsoleWindow* console = (ConsoleWindow*)data->UserData;
    return TextEditCallback(console, data);
}

static void ExecCommand(ConsoleWindow* console, const char* command_line)
{
    LogString(console, command_line, LOGTYPE_COMMAND);

    // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
    console->historyPos = -1;
    for (int i = console->history.Size-1; i >= 0; i--) {
        if (Stricmp(console->history[i], command_line) == 0) {
            free(console->history[i]);
            console->history.erase(console->history.begin() + i);
            break;
        }
    }
    console->history.push_back(Strdup(command_line));

    // Process command
    if (Stricmp(command_line, "CLEAR") == 0) {
        ClearLog(console);
    }
    else if (Stricmp(command_line, "HELP") == 0) {
        Log(console, "Commands:");
        for (int i = 0; i < console->commands.Size; i++) {
            Log(console, "- %s", console->commands[i]);
        }
    }
    else if (Stricmp(command_line, "HISTORY") == 0) {
        int first = console->history.Size - 10;
        for (int i = first > 0 ? first : 0; i < console->history.Size; i++) {
            Log(console, "%3d: %s\n", i, console->history[i]);
        }
    }
    else {
        Log(console, "Unknown command: '%s'\n", command_line);
    }

    // On commad input, we scroll to bottom even if autoScroll==false
    console->scrollToBottom = true;
}

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
        if (ImGui::BeginMenu("Debug"))
        {
#ifdef LUA_SCRIPTING_ENABLED
            if (ImGui::MenuItem("Debug LUA")) { EditorInit(&editorLUADebugger); }
            ImGui::EndMenu();
#endif
        }
        if (ImGui::MenuItem("Help")) { EditorInit(&editorHelp); }
        ImGui::EndMenuBar();
    }
    
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            console->open = false;
        ImGui::EndPopup();
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
    console->filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Clear")) ClearLog(console);
        ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1));
    if (copy_to_clipboard) {
        ImGui::LogToClipboard();
    }

    ImGui::BeginColumns("Logs", 2);
    ImGui::PushTextWrapPos(ImGui::GetWindowSize().x * 0.92f);
    ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x * 0.92f);

    for (int i = 0; i < console->items.Size; i++) {
        ConsoleLog* currentLog = &console->items[i];
        if (!console->filter.PassFilter(currentLog->log)) {
            continue;
        }

        switch(currentLog->type) {
            case LOGTYPE_ERROR: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            } break;
            case LOGTYPE_COMMAND: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
            } break;
        }

        ImGui::TextUnformatted(currentLog->log);
        
        if (currentLog->type != LOGTYPE_NORMAL) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::PopTextWrapPos();
    ImGui::NextColumn();

    for (int i = 0; i < console->items.Size; i++) {
        ConsoleLog* currentLog = &console->items[i];
        if (!console->filter.PassFilter(currentLog->log)) {
            continue;
        }
        
        switch(currentLog->type) {
            case LOGTYPE_ERROR: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            } break;
            case LOGTYPE_COMMAND: {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
            } break;
        }
        
        ImGui::Text("%d", currentLog->count);
        
        if (currentLog->type != LOGTYPE_NORMAL) {
            ImGui::PopStyleColor();
        }
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

#ifdef LUA_SCRIPTING_ENABLED
static void EditorDraw(LUADebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("LUA Debugger", &debugger->open, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    DebugMenuAction menuAction = DebugMenuAction_None;
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
                menuAction = DebugMenuAction_GoToFunction;
            }
            ImGui::EndMenu();
        }
        
        if(ImGui::BeginMenu("Debug")) {            
            if (ImGui::MenuItem("Break on function")) {
                menuAction = DebugMenuAction_BreakOnFunction;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if(menuAction == DebugMenuAction_GoToFunction) { ImGui::OpenPopup("GoToFunction"); }
    if(menuAction == DebugMenuAction_BreakOnFunction) { ImGui::OpenPopup("BreakOnFunction"); }

    if (ImGui::BeginPopup("GoToFunction"))
    {
        ImGui::Text("Go to function");

        if (ImGui::InputText("Input", debugger->inputBuffer, IM_ARRAYSIZE(debugger->inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue, 0, 0)) {
            char* s = debugger->inputBuffer;

	        lua_Debug debugInfo;
            int function = lua_getglobal(lua, debugger->inputBuffer);
            if(function > 0) {
                lua_getinfo(lua, ">S", &debugInfo);
                Log(&editorConsole, "Function found %s:%d", debugInfo.source, debugInfo.linedefined);

                if(debugger->currentFile) {
                    UnloadFileFromMemory(debugger->currentFile);
                }

                debugger->currentFile = (char*)LoadFileToMemory(debugInfo.source + 1, "rb", &debugger->currentFileSize);
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
            int function = lua_getglobal(lua, debugger->inputBuffer);
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

    if(debugger->debugging) {
        if (ImGui::SmallButton("Stop")) {
            debugger->debugging = false;
            Log(&editorConsole, "Debugging stopped");
        }

        ImGui::SameLine();

        if (ImGui::SmallButton("Step")) {
            
        }

        ImGui::Separator();
    }
    else {
        if (ImGui::SmallButton("Start")) {
            debugger->debugging = true;
            Log(&editorConsole, "Debugging started");
            
            LoadLUALibrary(sol::lib::debug);
        }
    }

    ImGui::Separator();

    ImGui::Text("Memory: %d", lua.memory_used());
    if (ImGui::SmallButton("Force GC")) {
        lua.collect_garbage();
    }

    ImGui::Separator();

    if(debugger->currentFile) {
        i32 line = 1;
        i32 index = 0;

        ImGui::BeginColumns("Text Editor", 2, ImGuiColumnsFlags_NoResize);
        ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x * 0.08f);
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
        while(index <= debugger->currentFileSize) {
            if(debugger->currentFile[index] == '\r') { ++index; }
            if(debugger->currentFile[index] == '\n' || debugger->currentFile[index] == 0) {
                ImGui::TextUnformatted(debugger->currentFile + lastIndex, debugger->currentFile + index);
                lastIndex = index + 1;
                ++line;
            }
            ++index;
        }

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

static void EditorDrawAllOpen()
{
    EditorDraw(&editorConsole);
#ifdef LUA_SCRIPTING_ENABLED
    EditorDraw(&editorLUADebugger);
#endif
    EditorDraw(&editorHelp);
}