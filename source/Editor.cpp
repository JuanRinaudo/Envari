const char* watchTypeNames[] = { "Auto", "Int", "Float", "Bool", "Char", "String" };

EditorLogFlag currentLogFlag = EditorLogFlag_NONE;

static void ClearLog(ConsoleWindow* console)
{
    for (i32 i = 0; i < console->items.Size; i++) {
        free(console->items[i].log);
    }
    console->items.clear();
}

static void LogString(ConsoleWindow* console, const char* log, ConsoleLogType type = ConsoleLogType_NORMAL, u32 line = 0, const char* file = "")
{
    ConsoleLog* lastLog = console->items.Size > 0 ? &console->items.back() : 0;
    if(lastLog && strcmp(log, lastLog->log) == 0) {
        lastLog->count++;
    }
    else {
        ConsoleLog newLog;
        newLog.log = Strdup(log, &newLog.logSize);
        newLog.count = 1;
        newLog.file = Strdup(file, &newLog.fileSize);
        newLog.line = line;
        newLog.type = type;
        console->items.push_back(newLog);
    }
}

void Log_(ConsoleWindow* console, ConsoleLogType type, const char* file, u32 line, const char* fmt, ...)
{
    if(currentLogFlag == EditorLogFlag_NONE || (currentLogFlag & console->logFlags) > 0) {
        char buffer[1024];
        va_list args;
        va_start(args, fmt);
        i32 size = vsnprintf(buffer, ArrayCount(buffer), fmt, args);
        va_end(args);
        buffer[ArrayCount(buffer)-1] = 0;
        LogString(console, buffer, type, line, file);
    }
}

extern void ChangeLogFlag_(u32 newFlag)
{
    currentLogFlag = (EditorLogFlag)newFlag;
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
    console->logFlags = EditorLogFlag_NONE;

    console->open = true;
    
    Log("Envari Console Start");
}

static void EditorInit(PreviewWindow* preview)
{
    
}

static void EditorInit(PerformanceDebuggerWindow* debugger)
{
    
}

static void EditorInit(RenderDebuggerWindow* debugger)
{
    
}

static void EditorInit(MemoryDebuggerWindow* debugger)
{
    
}

static void EditorInit(TextureDebuggerWindow* debugger)
{
    debugger->textureIndex = 0;
    debugger->inspectMode = TextureInspect_CACHE;
}

static void EditorInit(SoundDebuggerWindow* debugger)
{
    
}

static void EditorInit(InputDebuggerWindow* debugger)
{
    
}

static void EditorInit(TimeDebuggerWindow* debugger)
{
    
}

#ifdef LUA_SCRIPTING_ENABLED
static void EditorInit(LUADebuggerWindow* debugger)
{
    debugger->watching = true;

    if(TableHasKey(initialConfig, INITLUASCRIPT)) {
        debugger->currentFile = (char*)LoadFileToMemory(TableGetString(&initialConfig, INITLUASCRIPT), FILE_MODE_READ_BINARY, &debugger->currentFileSize);
    }
}
#endif

static void EditorInit(HelpWindow* help)
{
    
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
                Log("No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
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
                Log("Possible matches:\n");
                for (i32 i = 0; i < candidates.Size; i++) {
                    Log("- %s\n", candidates[i]);
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

    LogString(console, command, ConsoleLogType_COMMAND, __LINE__);

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
        Log("Commands:");
        for (i32 i = 0; i < console->commands.Size; i++) {
            Log("- %s", console->commands[i]);
        }
    }
    else if (Stricmp(command, "HISTORY") == 0) {
        i32 first = console->history.Size - 10;
        for (i32 i = first > 0 ? first : 0; i < console->history.Size; i++) {
            Log("%3d: %s\n", i, console->history[i]);
        }
    }
#ifdef LUA_SCRIPTING_ENABLED
    else if (Stricmp(command, "LUA") == 0) {
        lua.script(command_line + argumentStart);
    }
#endif
    else {
        Log("Unknown command: '%s'\n", command_line);
    }

    // On commad input, we scroll to bottom even if autoScroll==false
    console->scrollToBottom = true;
}

static void EditorLogMenuButton(ConsoleWindow* console, const char* flagName, u32 flag) {
    bool value = (console->logFlags & flag) > 0;
    if (ImGui::Checkbox(flagName, &value)) { console->logFlags = (value ? console->logFlags | flag : console->logFlags & ~flag); }
}

static u32 GameInit();
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
            if (!gameState->game.updateRunning && ImGui::MenuItem("Play")) { gameState->game.updateRunning = true; }
            if (gameState->game.updateRunning && ImGui::MenuItem("Pause")) { gameState->game.updateRunning = false; }
            if (ImGui::MenuItem("Reset", "CTRL+R")) {
                GL_CleanCache();
                
                ResetArena(&sceneState->arena);

                gameState->time.gameTime = 0;
                gameState->time.gameFrames = 0;

                GameInit();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::Checkbox("Preview", &editorPreview.open)) { EditorInit(&editorPreview); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::Checkbox("Performance", &editorPerformanceDebugger.open)) { EditorInit(&editorPerformanceDebugger); }
            if (ImGui::Checkbox("Render", &editorRenderDebugger.open)) { EditorInit(&editorRenderDebugger); }
            if (ImGui::Checkbox("Memory", &editorMemoryDebugger.open)) { EditorInit(&editorMemoryDebugger); }
            if (ImGui::Checkbox("Textures", &editorTextureDebugger.open)) { EditorInit(&editorTextureDebugger); }
            if (ImGui::Checkbox("Input", &editorInputDebugger.open)) { EditorInit(&editorInputDebugger); }
            if (ImGui::Checkbox("Time", &editorTimeDebugger.open)) { EditorInit(&editorTimeDebugger); }
            if (ImGui::Checkbox("Sound", &editorSoundDebugger.open)) { EditorInit(&editorSoundDebugger); }
#ifdef LUA_SCRIPTING_ENABLED
            if (ImGui::Checkbox("LUA", &editorLUADebugger.open)) { EditorInit(&editorLUADebugger); }
#endif
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::Checkbox("About", &editorHelp.open)) { EditorInit(&editorHelp); }
            ImGui::Checkbox("Imgui demo window", &editorState->demoWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (ImGui::SmallButton("Clear")) {
        ClearLog(console);
    }
    
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    
    ImGui::SameLine();
    ImGuiIO imguiIO = ImGui::GetIO();
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / imguiIO.Framerate, imguiIO.Framerate);

    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &console->autoScroll);
        ImGui::EndPopup();
    }
    
    if (ImGui::BeginPopup("LogFlagPopup"))
    {
        EditorLogMenuButton(console, "Performance", EditorLogFlag_PERFORMANCE);
        EditorLogMenuButton(console, "Render", EditorLogFlag_RENDER);
        EditorLogMenuButton(console, "Memory", EditorLogFlag_MEMORY);
        EditorLogMenuButton(console, "Texture", EditorLogFlag_TEXTURE);
        EditorLogMenuButton(console, "Sound", EditorLogFlag_SOUND);
        EditorLogMenuButton(console, "Input", EditorLogFlag_INPUT);
        EditorLogMenuButton(console, "Time", EditorLogFlag_TIME);
        EditorLogMenuButton(console, "LUA", EditorLogFlag_LUA);
        
        EditorLogMenuButton(console, "System", EditorLogFlag_SYSTEM);
        EditorLogMenuButton(console, "Game", EditorLogFlag_GAME);
        EditorLogMenuButton(console, "Scripting", EditorLogFlag_SCRIPTING);

        ImGui::EndMenu();
    }

    // Options, filter
    console->filter.Draw("Filter", 180);

    ImGui::SameLine();
    if (ImGui::Button("Options")) {
        ImGui::OpenPopup("Options");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Flags"))
        ImGui::OpenPopup("LogFlagPopup");

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
    ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x * 0.87f);

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
        while(index < currentLog->logSize) {
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
        ImGui::TextUnformatted(currentLog->log + lastIndex, currentLog->log + currentLog->logSize - 1);
        
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
        ImGui::Text("%s:%d", inspectedLog->file, inspectedLog->line);
        ImGui::Spacing();
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
    if (!ImGui::Begin("Preview", &preview->open, ImGuiWindowFlags_MenuBar)) {
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }
    ImGui::PopStyleVar();

    preview->focused = ImGui::IsWindowFocused();

    ImVec2 size = ImGui::GetWindowSize();
    
    f32 height = ImGui::GetFrameHeight();

    ImVec2 contentStart = ImGui::GetWindowContentRegionMin();
    size.y -= contentStart.y;

    gameState->render.size.y = size.y;
    gameState->render.size.x = size.y * gameState->camera.ratio;
    
    PreviewMenuAction menuAction = PreviewMenuAction_NONE;
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            if(ImGui::Checkbox("Preview Data", &preview->showData)){
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Set Size")) {
                preview->changeSize = V2I((i32)size.x, (i32)size.y);
                menuAction = PreviewMenuAction_CHANGE_SIZE;
            }
            ImGui::EndMenu();
        }

        ImVec2 newSize = ImGui::GetWindowSize();
        height = newSize.y - size.y;

        ImGui::EndMenuBar();
    }
    
    f32 offsetX = (size.x - gameState->render.size.x) * 0.5f;

    ImGui::SetCursorPos(ImVec2(offsetX, contentStart.y));

    ImVec2 cursorPosition = ImGui::GetMousePos();
    ImVec2 previewMin = ImGui::GetWindowPos();
    previewMin.x += offsetX;
    previewMin.y += height;
    ImVec2 previewMax = previewMin + ImGui::GetWindowSize();
    previewMax.x -= offsetX * 2;
    previewMax.y -= height;

    preview->cursorInsideWindow = cursorPosition.x > previewMin.x && cursorPosition.x < previewMax.x && cursorPosition.y > previewMin.y && cursorPosition.y < previewMax.y;
    preview->cursorPosition = V2(Clamp(cursorPosition.x - previewMin.x, 0, gameState->render.size.x), Clamp(cursorPosition.y - previewMin.y, 0, gameState->render.size.y));

    if(menuAction == PreviewMenuAction_CHANGE_SIZE) { ImGui::OpenPopup("ChangeSize"); }
    
    if (ImGui::BeginPopup("ChangeSize")) {
        preview->cursorInsideWindow = false;
        
        ImGui::Text("Go to function");

        ImGui::InputInt("X", &preview->changeSize.x);
        ImGui::InputInt("Y", &preview->changeSize.y);
        if (ImGui::Button("Set")) {
            ImGui::SetWindowSize("Preview", ImVec2((f32)preview->changeSize.x, (f32)preview->changeSize.y + height), ImGuiCond_Always);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::Image((ImTextureID)gameState->render.frameBuffer, ImVec2((f32)gameState->render.size.x, (f32)gameState->render.size.y), ImVec2(0, 1), ImVec2(1, 0));

    if(preview->showData) {
        previewMin = ImGui::GetWindowPos();
        previewMin.y += height;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(previewMin, previewMin + ImVec2(200, 70), IM_COL32(0, 0, 0, 200), 0.0f, 1);

        ImGui::SetCursorPos(ImVec2(5, height + 3));
        ImGui::Text("Render Size: %d, %d", (i32)gameState->render.size.x, (i32)gameState->render.size.y);
        ImGui::SetCursorPos(ImVec2(5, height + 23));
        ImGui::Text("Buffer Size: %d, %d", (i32)gameState->render.bufferSize.x, (i32)gameState->render.bufferSize.y);
    }

    if(!preview->open) {        
        gameState->render.size.x = gameState->render.windowSize.x;
        gameState->render.size.y = gameState->render.windowSize.y;
    }

    ImGui::End();
}

static void EditorDraw(PerformanceDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Performance Debugger", &debugger->open)) {
        ImGui::End();
        return;
    }
    
    ImGui::Text("Update: %f ms %d cycles", debugger->updateTime / 10000.0f, debugger->updateCycles);
    ImGui::Text("LUA Update: %f ms %d cycles", debugger->luaUpdateTime / 10000.0f, debugger->luaUpdateCycles);
    
    ImGui::End();
}

static void EditorDraw(RenderDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Render Debugger", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Window size: %.0fx%.0f", gameState->render.windowSize.x, gameState->render.windowSize.y);
    ImGui::Text("Buffer size: %.0fx%.0f", gameState->render.bufferSize.x, gameState->render.bufferSize.y);
    
    ImGui::Separator();
    
    ImGui::Text("Render memory size: %d", debugger->renderMemory);
    ImGui::Text("Draw count: %d", debugger->drawCount);
    ImGui::Text("Program changes: %d", debugger->programChanges);
    
    ImGui::Separator();

    i32 total = GL_TotalGPUMemoryKB();
    ImGui::Text("GPU memory: %d / %d", total - GL_AvailableGPUMemoryKB(), total);
    
    ImGui::Separator();

    ImGui::Checkbox("Wireframe Mode", &debugger->wireframeMode);
    
    ImGui::Separator();

    ImGui::Checkbox("Record frames", &debugger->recording);

    ImGui::Separator();

    if(ImGui::SmallButton(editorState->editorFrameRunning ? "Stop" : "Run")) {
        editorState->editorFrameRunning = !editorState->editorFrameRunning;
    }

    if(!editorState->editorFrameRunning) {
        ImGui::SameLine();
        ImGui::Text("ID Target: %d", debugger->renderDebugTarget);
        ImGui::SameLine();
        if(ImGui::SmallButton("Clear")) {
            debugger->renderDebugTarget = -1;
        }
        ImGui::SameLine();
        if(ImGui::SmallButton("Previous")) {
            debugger->renderDebugTarget--;
        }
        ImGui::SameLine();
        if(ImGui::SmallButton("Next")) {
            debugger->renderDebugTarget++;
        }
    }

    b32 targetChanged = debugger->renderDebugTargetChanged;
    debugger->renderDebugTargetChanged = false;

    if(ImGui::CollapsingHeader("Render Queue")) {        
        RenderHeader *renderHeader = (RenderHeader *)renderTemporaryMemory.arena->base;
        if(!editorState->editorFrameRunning) {
            renderHeader = editorState->savedRenderHeader;
        }
        u32 lastHeaderID = 0;

        while(renderHeader->id > 0 && (void*)renderHeader < (void*)(renderTemporaryMemory.arena->base + renderTemporaryMemory.used)) {
            if(renderHeader->type == RenderType_RenderTransparent) {
                ImGui::Separator();
            }

            lastHeaderID = renderHeader->id;
            bool stylePushed = false;
            if(!renderHeader->enabled) {
                stylePushed = true;
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
            }
            
            ImGui::Text("%d   ", renderHeader->id); // #TODO (Juan): Fix padding number
            ImGui::SameLine();

            switch(renderHeader->type) {
                case RenderType_RenderTempData: {
                    RenderTempData *tempData = (RenderTempData *)renderHeader;
                    ImGui::Text("Temp Data");
                    break;
                }
                case RenderType_RenderClear: {
                    RenderClear *clear = (RenderClear *)renderHeader;
                    ImGui::Text("Clear -> %.3f, %.3f, %.3f, %.3f", clear->color.r, clear->color.g, clear->color.b, clear->color.a);
                    break;
                }
                case RenderType_RenderColor: {
                    RenderColor *color = (RenderColor *)renderHeader;
                    ImGui::Text("Color -> %.3f, %.3f, %.3f, %.3f", color->color.r, color->color.g, color->color.b, color->color.a);
                    break;
                }
                case RenderType_RenderLayer: {
                    RenderLayer *layer = (RenderLayer *)renderHeader;
                    ImGui::Text("Layer -> %d", layer->layer);
                    break;
                }
                case RenderType_RenderTransparent: {                    
                    RenderTransparent *transparent = (RenderTransparent *)renderHeader;
                    ImGui::Text("Transparent -> On: %u, Mode: %u %u, RGB: %u %u, Alpha: %u %u", transparent->enabled, transparent->modeRGB, transparent->modeAlpha, 
                        transparent->srcRGB, transparent->dstRGB, transparent->srcAlpha, transparent->dstAlpha);
                    break;
                }
                case RenderType_RenderLineWidth: {
                    RenderLineWidth *line = (RenderLineWidth *)renderHeader;
                    ImGui::Text("Line Width -> %.3f", line->width);
                    break;
                }
                case RenderType_RenderLine: {
                    RenderLine *line = (RenderLine *)renderHeader;
                    ImGui::Text("Line -> Start: %.3f %.3f, End: %.3f %.3f", line->start.x, line->start.y, line->end.x, line->end.y);
                    break;
                }
                case RenderType_RenderTriangle: {
                    RenderTriangle *triangle = (RenderTriangle *)renderHeader;
                    ImGui::Text("Triangle -> P1: %.3f %.3f, P2: %.3f %.3f, P3: %.3f %.3f", triangle->point1.x, triangle->point1.y,
                        triangle->point2.x, triangle->point2.y,
                        triangle->point3.x, triangle->point3.y);
                    break;
                }
                case RenderType_RenderRectangle: {
                    RenderRectangle *rectangle = (RenderRectangle *)renderHeader;
                    ImGui::Text("Rectangle -> Position: %.3f %.3f, Scale: %.3f %.3f", rectangle->position.x, rectangle->position.y, rectangle->scale.x, rectangle->scale.y);
                    break;
                }
                case RenderType_RenderCircle: {
                    RenderCircle *circle = (RenderCircle *)renderHeader;
                    ImGui::Text("Circle -> Position: %.3f %.3f, Radius: %.3f, Segments: %.3f", circle->position.x, circle->position.y, circle->radius, circle->segments);
                    break;\
                }
                case RenderType_RenderTextureParameters: {
                    RenderTextureParameters *textureParameters = (RenderTextureParameters *)renderHeader;
                    ImGui::Text("Texture Parameters -> %u, %u, %u, %u", textureParameters->wrapS, textureParameters->wrapT,
                        textureParameters->minFilter, textureParameters->magFilter);
                    break;
                }
                case RenderType_RenderTexture: {
                    RenderTexture *texture = (RenderTexture *)renderHeader;
                    ImGui::Text("Texture -> Position: %.3f %.3f, Scale: %.3f %.3f, ID: %u", texture->position.x, texture->position.y, texture->scale.x, texture->scale.y,
                        texture->textureID);
                    break;
                }
                case RenderType_RenderImage: {
                    RenderImage *image = (RenderImage *)renderHeader;
                    ImGui::Text("Image -> Position: %.3f %.3f, Scale: %.3f %.3f,\n\tFile: %s", image->position.x, image->position.y, image->scale.x, image->scale.y, 
                        image->filepath);
                    break;
                }
                case RenderType_RenderImageUV: {
                    RenderImageUV *imageUV = (RenderImageUV *)renderHeader;
                    ImGui::Text("Image UV -> Position: %.3f %.3f, Scale: %.3f %.3f, UVMin: %.3f %.3f, UVMax: %.3f %.3f,\n\tFile: %s", imageUV->position.x, imageUV->position.y,
                        imageUV->scale.x, imageUV->scale.y, 
                        imageUV->uvMin.x, imageUV->uvMin.y, imageUV->uvMax.x, imageUV->uvMax.y,
                        imageUV->filepath);
                    break;
                }
                case RenderType_RenderImage9Slice: {
                    RenderImage9Slice *image9Slice = (RenderImage9Slice *)renderHeader;
                    ImGui::Text("9 Slice -> Position: %.3f %.3f, End Position: %.3f %.3f, Slice: %.3f,\n\tFile: %s", image9Slice->position.x, image9Slice->position.y,
                        image9Slice->endPosition.x, image9Slice->endPosition.y,
                        image9Slice->slice, image9Slice->filepath);
                    break;
                }
                case RenderType_RenderAtlasSprite: {
                    RenderAtlasSprite *atlas = (RenderAtlasSprite *)renderHeader;
                    ImGui::Text("Atlas Sprite -> Position: %.3f %.3f, Scale: %.3f %.3f,\n\tFile: %s\n\tAtlas: %s Sprite: %s", atlas->position.x, atlas->position.y, 
                        atlas->scale.x, atlas->scale.y, atlas->filepath, atlas->atlasName, atlas->spriteKey);
                    break;
                }
                case RenderType_RenderFont: {
                    RenderFont *font = (RenderFont *)renderHeader;
                    ImGui::Text("Font ID: %d", font->fontID);
                    break;
                }
                case RenderType_RenderChar: {
                    RenderChar *renderChar = (RenderChar *)renderHeader;
                    ImGui::Text("Char -> Position: %.3f %.3f, Scale: %.3f %.3f, Char: %c", renderChar->position.x, renderChar->position.y, renderChar->scale.x, renderChar->scale.y,
                        renderChar->singleChar);
                    break;
                }
                case RenderType_RenderText: {
                    RenderText *text = (RenderText *)renderHeader;
                    ImGui::Text("Text -> Position: %.3f %.3f, String: %s,\n\tSize: %u", text->position.x, text->position.y,
                        text->string, text->stringSize);
                    break;
                }
                case RenderType_RenderStyledText: {
                    RenderStyledText *styledText = (RenderStyledText *)renderHeader;
                    ImGui::Text("Styled Text -> Position: %.3f %.3f, String: %s,\n\tSize: %u, Center: %u, LetterWrap: %u, WordWrap: %u", styledText->position.x, styledText->position.y,
                        styledText->string, styledText->stringSize, (styledText->header.renderFlags & TextRenderFlag_Center) > 0,
                        (styledText->header.renderFlags & TextRenderFlag_LetterWrap) > 0, (styledText->header.renderFlags & TextRenderFlag_WordWrap) > 0
                    );
                    break;
                }
                case RenderType_RenderSetUniform: {
                    RenderSetUniform *uniform = (RenderSetUniform *)renderHeader;
                    ImGui::Text("Set Uniform");
                    break;
                }
                case RenderType_RenderOverrideProgram: {
                    RenderOverrideProgram *program = (RenderOverrideProgram *)renderHeader;
                    ImGui::Text("Override Program");
                    break;
                }
                case RenderType_RenderOverrideVertices: {
                    RenderOverrideVertices *vertices = (RenderOverrideVertices *)renderHeader;
                    ImGui::Text("Override Vertices");
                    break;
                }
                case RenderType_RenderOverrideIndices: {
                    RenderOverrideIndices *indices = (RenderOverrideIndices *)renderHeader;
                    ImGui::Text("Override Indices");
                    break;
                }
                default: {
                    ImGui::Text("Unknown render command! Header Type %d, Size %d", renderHeader->type, renderHeader->size);
                    break;
                }
            }

            if(stylePushed) { ImGui::PopStyleColor(); }

            if(targetChanged && debugger->renderDebugTarget > 0) {
                renderHeader->enabled = renderHeader->id <= debugger->renderDebugTarget;
            }

            if(ImGui::IsItemClicked()) {
                if(ImGui::GetIO().KeyCtrl) {
                    debugger->renderDebugTarget = renderHeader->id;
                    debugger->renderDebugTargetChanged = true;
                }
                else {
                    renderHeader->enabled = !renderHeader->enabled;
                }
            }

            Assert(renderHeader->size > 0);
            renderHeader = (RenderHeader *)((u8 *)renderHeader + renderHeader->size);
        }
    }

    ImGui::End();
}

static void EditorDraw(MemoryDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Memory Debugger", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Permanent memory: %d / %d", permanentState->arena.used, permanentState->arena.size);
    ImGui::Text("Scene memory: %d / %d", sceneState->arena.used, sceneState->arena.size);
    ImGui::Text("Temporal memory: %d / %d", temporalState->arena.used, temporalState->arena.size);

    ImGui::Separator();

    ImGui::Text("Memory: %d", debugger->memoryCounters.WorkingSetSize);
    ImGui::Text("Peak memory: %d", debugger->memoryCounters.PeakWorkingSetSize);

    if(ImGui::CollapsingHeader("Page Data")) {
        ImGui::Text("Page faults: %d", debugger->memoryCounters.PageFaultCount);
        ImGui::Text("Page file usage: %d", debugger->memoryCounters.PagefileUsage);
        ImGui::Text("Peak page file usage: %d", debugger->memoryCounters.PeakPagefileUsage);
        ImGui::Text("Quota page pool usage: %d", debugger->memoryCounters.QuotaPagedPoolUsage);
        ImGui::Text("Quota non page pool usage: %d", debugger->memoryCounters.QuotaNonPagedPoolUsage);
        ImGui::Text("Quota peak page pool usage: %d", debugger->memoryCounters.QuotaPeakPagedPoolUsage);
        ImGui::Text("Quota peak non page pool usage: %d", debugger->memoryCounters.QuotaPeakNonPagedPoolUsage);
    }

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

    if (!ImGui::Begin("Texture Debugger", &debugger->open)) {
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
    if(debugger->inspectMode == TextureInspect_CACHE && textureCacheSize > 0)
    {
        ImGui::Text("Texture Cache ID: %d / %d", debugger->textureIndex + 1, textureCacheSize);
        
        ImGui::SameLine();
        if (ImGui::SmallButton("Prev")) {
            debugger->textureIndex--;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Next")) {
            debugger->textureIndex++;
        }

        if(debugger->textureIndex < 0) { debugger->textureIndex = textureCacheSize - 1; }
        if(debugger->textureIndex >= textureCacheSize) { debugger->textureIndex = 0; }

        GLTexture cachedTexture = textureCache[debugger->textureIndex].value;

        ImGui::Text("Texture ID: %d", cachedTexture.textureID);
        ImGui::Text("Texture Size: %d x %d", cachedTexture.width, cachedTexture.height);

        ImGui::Separator();

        debugger->textureWidth = cachedTexture.width;
        debugger->textureHeight = cachedTexture.height;

        textureID = cachedTexture.textureID;
    }
    else if(debugger->inspectMode == TextureInspect_ALL) {
        bool textureChanged = false;
        if(ImGui::InputInt("Texture ID", &debugger->textureIndex, 1, 1)) {
            debugger->textureLevel = 0;
            textureChanged = true;
        }
        if(ImGui::InputInt("Texture Level", &debugger->textureLevel, 1, 1)) {
            if(debugger->textureLevel < 0) {
                debugger->textureLevel = 0;
            }
            textureChanged = true;
        }

        if(textureChanged) {
            glBindTexture(GL_TEXTURE_2D, debugger->textureIndex);
            f32 width, height;
            glGetTexLevelParameterfv(GL_TEXTURE_2D, debugger->textureLevel, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameterfv(GL_TEXTURE_2D, debugger->textureLevel, GL_TEXTURE_HEIGHT, &height);
            debugger->textureWidth = (i32)width;
            debugger->textureHeight = (i32)height;
        }
        
        ImGui::InputInt("Texture Width", &debugger->textureWidth, 1, 1);
        ImGui::InputInt("Texture Height", &debugger->textureHeight, 1, 1);

        textureID = debugger->textureIndex;
    }

    ImGui::BeginChild("Texture", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
    ImGui::Image((ImTextureID)textureID, ImVec2((f32)debugger->textureWidth, (f32)debugger->textureHeight));
    ImGui::EndChild();

    ImGui::End();
}

static void EditorDraw(SoundDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Sound debugger", &debugger->open)) {
        ImGui::End();
        return;
    }

    if (ImGui::SmallButton(soundMuted ? "Unmute" : "Mute")) {
        soundMuted = !soundMuted;
    }

    ImGui::Text("Sound mix pool: %d / %d", soundMixIndex + 1, SOUND_MIX_SIZE);

    ImGui::Separator();

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    float width = contentMax.x - contentMin.x;

    char title[32];
    for(i32 channelIndex = 0; channelIndex < SOUND_CHANNELS; ++channelIndex) {
        sprintf(title, "Master Channel %d", channelIndex);

        ImGui::Text("Master Min: %f Max: %f (%f - %f)", debugger->bufferToShowMin[channelIndex], debugger->bufferToShowMax[channelIndex], soundRangeMin, soundRangeMax);
        ImGui::PlotLines("", debugger->bufferToShow + channelIndex * BUFFER_CHANNEL_TO_SHOW_SIZE, BUFFER_CHANNEL_TO_SHOW_SIZE, editorSoundDebugger.bufferOffset, title, soundRangeMin, soundRangeMax, ImVec2(width, 80));
    }
    
    if(ImGui::CollapsingHeader("Sound cache")) {
        // i32 soundCacheSize = shlen(soundCache);
        // ImGui::Text("Sound Cache ID: %d / %d", debugger->cacheIndex + 1, soundCacheSize);

        // if(debugger->cacheIndex < soundCacheSize) {
        //     ImGui::SameLine();
        //     if (ImGui::SmallButton("Prev")) {
        //         debugger->cacheIndex--;
        //     }
        //     ImGui::SameLine();
        //     if (ImGui::SmallButton("Next")) {
        //         debugger->cacheIndex++;
        //     }

        //     if(debugger->cacheIndex < 0) { debugger->cacheIndex = soundCacheSize - 1; }
        //     if(debugger->cacheIndex >= soundCacheSize) { debugger->cacheIndex = 0; }

        //     MASoundCache cacheItem = soundCache[debugger->cacheIndex];
            
        //     ImGui::Separator();

        //     ImGui::Text("Name: %s", cacheItem.key);
        //     ImGui::Text("Format: %d", cacheItem.value->internalFormat);
        //     ImGui::Text("Channels: %d", cacheItem.value->internalChannels);
        //     ImGui::Text("ChannelMap: %d", cacheItem.value->internalChannelMap);
        //     ImGui::Text("SampleRate: %d", cacheItem.value->internalSampleRate);
        // }
    }

    ImGui::End();
}

static void EditorDraw(InputDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Input debugger", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Mouse texture ID: %d", gameState->input.mouseTextureID);
    ImGui::Text("Mouse position\tX: %f\tY: %f", gameState->input.mousePosition.x, gameState->input.mousePosition.y);
    ImGui::Text("Mouse screen  \tX: %f\tY: %f", gameState->input.mouseScreenPosition.x, gameState->input.mouseScreenPosition.y);
    ImGui::Text("Mouse wheel:  \t%d", gameState->input.mouseWheel);

    ImGui::Text("Text input buffer: %s", gameState->input.textInputBuffer);

    float elementSize = 50;

    ImGui::TextUnformatted("Mouse");
    ImGui::PushItemWidth(elementSize);
    for(int i = 0; i < MOUSE_COUNT; ++i) {
        if(i > 0 && i < MOUSE_COUNT && i % 10 != 0) {
            ImGui::SameLine();
        }
        else {
            ImGui::Text("%03d ->", (i32)(Floor(i / 10.0f) * 10));
            ImGui::SameLine();
        }
        ImGui::Text("%d", gameState->input.mouseState[i]);
    }

    ImGui::TextUnformatted("Keyboard");
    for(int i = 0; i < KEY_COUNT; ++i) {
        if(i > 0 && i < KEY_COUNT &&  i % 10 != 0) {
            ImGui::SameLine();
        }
        else {
            ImGui::Text("%03d ->", (i32)(Floor(i / 10.0f) * 10));
            ImGui::SameLine();
        }
        ImGui::Text("%d", gameState->input.keyState[i]);
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

static void EditorDraw(TimeDebuggerWindow* debugger)
{
    if(!debugger->open) { return; };
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Time debugger", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    float width = contentMax.x - contentMin.x;

    ImGui::Text("Start time: %f", gameState->time.startTime);
    ImGui::Text("Delta time: %f", gameState->time.deltaTime);
    ImGui::Text("Game time: %f", gameState->time.gameTime);
    ImGui::Text("Last frame game time: %f", gameState->time.lastFrameGameTime);

    ImGui::Separator();
    
    ImGui::Text("Frames: %d", gameState->time.frames);
    ImGui::Text("Game frames: %d", gameState->time.gameFrames);

    ImGui::Separator();

    ImGui::Text("Frame Time Min: %f Max: %f", debugger->frameTimeMin, debugger->frameTimeMax);
    ImGui::PlotLines("", debugger->frameTimeBuffer, TIME_BUFFER_SIZE, editorTimeDebugger.debuggerOffset, "Frame time", debugger->frameTimeMin, debugger->frameTimeMax, ImVec2(width, 80));

    ImGui::Text("FPS Min: %f Max: %f", debugger->fpsMin, debugger->fpsMax);
    ImGui::PlotLines("", debugger->fpsBuffer, TIME_BUFFER_SIZE, editorTimeDebugger.debuggerOffset, "FPS", debugger->fpsMin, debugger->fpsMax, ImVec2(width, 80));

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
        // if (ImGui::BeginMenu("File"))
        // {
        //     ImGui::EndMenu();
        // }
        
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
                    Log("Debugging stopped");
                }
                if (ImGui::MenuItem("Step", "F10")) {
                    
                }
            }
            else {
                if (ImGui::MenuItem("Start", "F5")) {
                    debugger->debugging = true;
                    Log("Debugging started");

                    ScriptingDebugStart();
                    
                    LoadLUALibrary(sol::lib::debug);
                }
            }

            ImGui::Checkbox("Watch variables", &debugger->watching);

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
                Log("Function found %s:%d", debugInfo.source, debugInfo.linedefined);

                if(debugger->currentFile) {
                    UnloadFileFromMemory(debugger->currentFile);
                }

                debugger->currentFile = (char*)LoadFileToMemory(debugInfo.source + 1, FILE_MODE_READ_BINARY, &debugger->currentFileSize);
            }
            else {
                Log("Function not found %s", debugger->inputBuffer);
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
                Log("Function found %s:%d", debugInfo.source, debugInfo.linedefined);

                // lua_sethook(lua, 0, 0, 0);
                // lua_sethook(lua, function, LUA_MASKCALL, 0);
            }
            else {
                Log("Function not found %s", debugger->inputBuffer);
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    f32 height = contentMax.y - contentMin.y;

    ImGui::BeginChild("LUA Code", ImVec2(contentMax.x, height * (debugger->watching ? 0.5f : 1)), false);
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
	            //     LogCommand("%d", line);
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
    ImGui::EndChild();

    if(debugger->watching) {
        ImGui::Separator();
        ImGui::BeginChild("Watch Window", ImVec2(contentMax.x, height * (debugger->watching ? 0.5f : 1) - 5), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
        ImGui::Text("Watching");
        char valueBuffer[64];
        for(i32 i = 0; i < WATCH_BUFFER_COUNT; ++i) {
            valueBuffer[0] = '\0';

            ImGui::PushID(i);
            ImGui::PushItemWidth(contentMax.x * 0.3f);
            char* test = debugger->watchBuffer + i * WATCH_BUFFER_SIZE_EXT;
            ImGui::InputText("##Input", test, WATCH_BUFFER_SIZE);
            ImGui::SameLine();
            ImGui::PushItemWidth(contentMax.x * 0.3f);
            if (ImGui::BeginCombo("##Combo", watchTypeNames[debugger->watchType[i]]))
            {
                for (int n = 0; n < ArrayCount(watchTypeNames); n++)
                {
                    const bool is_selected = (debugger->watchType[i] == n);
                    if (ImGui::Selectable(watchTypeNames[n], is_selected))
                        debugger->watchType[i] = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            GetWatchValue(debugger->watchType[i], debugger->watchBuffer + i * WATCH_BUFFER_SIZE_EXT, valueBuffer);
            ImGui::TextUnformatted(valueBuffer);
            ImGui::PopID();
        }
        ImGui::EndChild();
    }

    ImGui::End();
}
#endif

static void EditorDraw(HelpWindow* help)
{    
    if(!help->open) { return; };
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowSize(ImVec2(500,300), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("Help", &help->open, ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::Separator();
    
    ImGui::Text("Envari help");

    ImGui::Separator();

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    ImGui::Text("Version: %d.%d.%d (%s)", ENVARI_MAYOR_VERSION, ENVARI_MINOR_VERSION, ENVARI_MICRO_VERSION, ENVARI_PLATFORM);
#ifdef LUA_SCRIPTING_ENABLED
    ImGui::Text("LUA Version: %s", LUA_RELEASE);
    ImGui::Text("SOL Version: %s", SOL_VERSION_STRING);
#endif
    ImGui::Text("Dear Imgui Version: %s", IMGUI_VERSION);
    ImGui::Text("SDL Compile Version: %d.%d.%d", compiled.major, compiled.minor, compiled.patch);
    ImGui::Text("SDL Link Version: %d.%d.%d", linked.major, linked.minor, linked.patch);
    ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
    ImGui::Text("Miniaudio Version: %s", MA_VERSION_STRING);

    ImGui::End();
}

static void EditorInit()
{
    SerializableTable* editorSave = 0;
    DeserializeTable(&permanentState->arena, &editorSave, "editor.save");
    
    char loadNameBuffer[128];

    // editorConsole.open = TableGetBool(&editorSave, "editorConsoleOpen");
    editorConsole.open = true;
    EditorInit(&editorConsole);
    editorConsole.logFlags = TableGetI32(&editorSave, "editorLogFlags");

    editorPreview.open = TableGetBool(&editorSave, "editorPreviewOpen");
    EditorInit(&editorPreview);

    editorPerformanceDebugger.open = TableGetBool(&editorSave, "editorPerformanceDebuggerOpen");
    EditorInit(&editorPerformanceDebugger);

    editorRenderDebugger.open = TableGetBool(&editorSave, "editorRenderDebuggerOpen");
    EditorInit(&editorRenderDebugger);

    editorMemoryDebugger.open = TableGetBool(&editorSave, "editorMemoryDebuggerOpen");
    EditorInit(&editorMemoryDebugger);

    editorTextureDebugger.open = TableGetBool(&editorSave, "editorTextureDebuggerOpen");
    EditorInit(&editorTextureDebugger);

    editorInputDebugger.open = TableGetBool(&editorSave, "editorInputDebuggerOpen");
    EditorInit(&editorInputDebugger);

    editorTimeDebugger.open = TableGetBool(&editorSave, "editorTimeDebuggerOpen");
    EditorInit(&editorTimeDebugger);

    editorSoundDebugger.open = TableGetBool(&editorSave, "editorSoundDebuggerOpen");
    EditorInit(&editorSoundDebugger);
    soundMuted = TableGetBool(&editorSave, "editorSoundDebuggersoundMuted");

#ifdef LUA_SCRIPTING_ENABLED
    editorLUADebugger.open = TableGetBool(&editorSave, "editorLUADebuggerOpen");
    EditorInit(&editorLUADebugger);
    
    for(i32 i = 0; i < WATCH_BUFFER_COUNT; ++i) {
        sprintf(loadNameBuffer, "editorLUADebuggerWatching%d", i);
        strcpy(editorLUADebugger.watchBuffer + i * WATCH_BUFFER_SIZE_EXT, TableGetString(&editorSave, loadNameBuffer, ""));
    }
#endif
}

static void EditorDrawAllOpen()
{
    EditorDraw(&editorConsole);
    EditorDraw(&editorPreview);
    EditorDraw(&editorPerformanceDebugger);
    EditorDraw(&editorRenderDebugger);
    EditorDraw(&editorMemoryDebugger);
    EditorDraw(&editorTextureDebugger);
    EditorDraw(&editorInputDebugger);
    EditorDraw(&editorTimeDebugger);
    EditorDraw(&editorSoundDebugger);
#ifdef LUA_SCRIPTING_ENABLED
    EditorDraw(&editorLUADebugger);
#endif
    EditorDraw(&editorHelp);

    if(editorState->demoWindow) {
        ImGui::ShowDemoWindow(&editorState->demoWindow);
    }
}

static void EditorEnd()
{
    char saveNameBuffer[128];
    
    SerializableTable* editorSave = 0;
    TableSetBool(&permanentState->arena, &editorSave, "editorConsoleOpen", editorConsole.open);
    TableSetI32(&permanentState->arena, &editorSave, "editorLogFlags", editorConsole.logFlags);
    TableSetBool(&permanentState->arena, &editorSave, "editorPreviewOpen", editorPreview.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorPerformanceDebuggerOpen", editorPerformanceDebugger.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorRenderDebuggerOpen", editorRenderDebugger.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorMemoryDebuggerOpen", editorMemoryDebugger.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorTextureDebuggerOpen", editorTextureDebugger.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorInputDebuggerOpen", editorInputDebugger.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorTimeDebuggerOpen", editorTimeDebugger.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorSoundDebuggerOpen", editorSoundDebugger.open);
    TableSetBool(&permanentState->arena, &editorSave, "editorSoundDebuggersoundMuted", soundMuted);
#ifdef LUA_SCRIPTING_ENABLED
    TableSetBool(&permanentState->arena, &editorSave, "editorLUADebuggerOpen", editorLUADebugger.open);
    for(i32 i = 0; i < WATCH_BUFFER_COUNT; ++i) {
        sprintf(saveNameBuffer, "editorLUADebuggerWatching%d", i);
        TableSetString(&permanentState->arena, &editorSave, saveNameBuffer, editorLUADebugger.watchBuffer + i * WATCH_BUFFER_SIZE_EXT);
    }
#endif
    SerializeTable(&editorSave, "editor.save");
}