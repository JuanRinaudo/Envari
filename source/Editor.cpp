static void ClearLog(EnvariConsole* console)
{
    for (int i = 0; i < console->items.Size; i++) {
        free(console->items[i]);
    }
    console->items.clear();
}

static void AddBasicLog(EnvariConsole* console, const char* log)
{
    console->items.push_back(Strdup(log));
}

static void AddLog(EnvariConsole* console, const char* fmt, ...) IM_FMTARGS(2)
{
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf)-1] = 0;
    va_end(args);
    console->items.push_back(Strdup(buf));
}

static void EditorInit(EnvariConsole* console)
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
    
    AddLog(console, "Envari Console Start");
}

static void EditorInit(EnvariLUADebugger* debugger)
{
    debugger->open = true;

    if(TableHasKey(&initialConfig, "initLuaScript")) {
        debugger->currentFile = (char*)LoadFileToMemory(TableGetString(&initialConfig, "initLuaScript"), "rb", &debugger->fileSize);
    }

    LoadLUALibrary(sol::lib::debug);
}

static void EditorInit(EnvariHelp* help)
{
    help->open = true;
}

static int TextEditCallback(EnvariConsole* console, ImGuiInputTextCallbackData* data)
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
                AddLog(console, "No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
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
                AddLog(console, "Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++) {
                    AddLog(console, "- %s\n", candidates[i]);
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
    EnvariConsole* console = (EnvariConsole*)data->UserData;
    return TextEditCallback(console, data);
}

static void ExecCommand(EnvariConsole* console, const char* command_line)
{
    AddLog(console, "# %s\n", command_line);

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
        AddLog(console, "Commands:");
        for (int i = 0; i < console->commands.Size; i++) {
            AddLog(console, "- %s", console->commands[i]);
        }
    }
    else if (Stricmp(command_line, "HISTORY") == 0) {
        int first = console->history.Size - 10;
        for (int i = first > 0 ? first : 0; i < console->history.Size; i++) {
            AddLog(console, "%3d: %s\n", i, console->history[i]);
        }
    }
    else {
        AddLog(console, "Unknown command: '%s'\n", command_line);
    }

    // On commad input, we scroll to bottom even if autoScroll==false
    console->scrollToBottom = true;
}

static void EditorDraw(EnvariConsole* console)
{
    if(!console->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiCond_FirstUseEver);

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
            if (ImGui::MenuItem("Debug LUA")) { EditorInit(&editorLUADebugger); }
            ImGui::EndMenu();
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

    for (int i = 0; i < console->items.Size; i++) {
        const char* item = console->items[i];
        if (!console->filter.PassFilter(item)) {
            continue;
        }

        // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
        bool pop_color = false;
        if (strstr(item, "[error]")) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            pop_color = true;
        }
        else if (strncmp(item, "# ", 2) == 0) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
            pop_color = true;
        }
        ImGui::TextUnformatted(item);
        if (pop_color) {
            ImGui::PopStyleColor();
        }
    }
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

static void EditorDraw(EnvariLUADebugger* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("LUA Debugger", &debugger->open, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    DebugMenuAction menuAction = NONE;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("WIP")) { }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Go to function")) {
                menuAction = LUA_DEBUG_GOTOFUNCTION;
            }
            ImGui::SameLine();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(menuAction == LUA_DEBUG_GOTOFUNCTION) { ImGui::OpenPopup("GoToFunction"); }

    if (ImGui::BeginPopup("GoToFunction"))
    {
        ImGui::Text("Go to function");

        if (ImGui::InputText("Input", debugger->inputBuffer, IM_ARRAYSIZE(debugger->inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue, 0, 0)) {
            char* s = debugger->inputBuffer;

	        lua_Debug debugInfo;
            int function = lua_getglobal(lua, debugger->inputBuffer);
            if(function > 0) {
                lua_getinfo(lua, ">S", &debugInfo);
                AddLog(&editorConsole, "Function found %s:%d", debugInfo.source, debugInfo.linedefined);
            }
            else {
                AddLog(&editorConsole, "Function not found %s", debugger->inputBuffer);
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if(debugger->debugging) {
        if (ImGui::SmallButton("Stop")) {
            debugger->debugging = false;
            AddLog(&editorConsole, "Debugging stopped");
        }

        ImGui::SameLine();

        if (ImGui::SmallButton("Step")) {
            
        }

        ImGui::Separator();
    }
    else {
        if (ImGui::SmallButton("Start")) {
            debugger->debugging = true;
            AddLog(&editorConsole, "Debugging started");

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
        ImGui::Text("%s", debugger->currentFile);
    }
    else {
        ImGui::Text("No file loaded");
    }

    ImGui::End();
}

static void EditorDraw(EnvariHelp* help)
{    
    if(!help->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,200), ImGuiCond_FirstUseEver);
    
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
    EditorDraw(&editorLUADebugger);
    EditorDraw(&editorHelp);
}