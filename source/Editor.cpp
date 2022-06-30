#ifndef EDITOR_CPP
#define EDITOR_CPP

LogFlag currentLogFlag = LogFlag_NONE;

static i32 BuildPlatform(RuntimePlatform platform);

AssetType GetAssetType(filesystem::path filepath)
{
    std::string string = filepath.extension().string();
    const char* extension = string.c_str();
    if(strcmp(extension, ".txt") == 0 || strcmp(extension, ".ini") == 0 || strcmp(extension, ".envt") == 0 || strcmp(extension, ".lua") == 0) {
        return AssetType_TEXT;
    }
    else if(strcmp(extension, ".png") == 0 || strcmp(extension, ".jpg") == 0 || strcmp(extension, ".tga") == 0  || strcmp(extension, ".psd") == 0 || 
        strcmp(extension, ".gif") == 0 || strcmp(extension, ".bmp") == 0 || strcmp(extension, ".hdr") == 0 || strcmp(extension, ".pic") == 0) {
        return AssetType_IMAGE;
    }
    else if(strcmp(extension, ".wav") == 0 || strcmp(extension, ".ogg") == 0 || strcmp(extension, ".flac") == 0 || strcmp(extension, ".mp3") == 0) {
        return AssetType_SOUND;
    }
    else {
        return AssetType_UNKNOWN;
    }
}

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
    if(currentLogFlag == LogFlag_NONE || (currentLogFlag & console->logFlags) > 0) {
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
    #if PLATFORM_EDITOR && GAME_SLOW
    currentLogFlag = (LogFlag)newFlag;
    #endif
}

static void EditorInit(ConsoleWindow* console)
{
    ClearLog(console);
    memset(console->inputBuffer, 0, sizeof(console->inputBuffer));
    console->historyPos = -1;
    console->commands.push_back("HELP");
    console->commands.push_back("HISTORY");
    console->commands.push_back("CLEAR");
#ifdef LUA_ENABLED
    console->commands.push_back("LUA");
#endif
    console->autoScroll = true;
    console->scrollToBottom = false;
    console->logFlags = LogFlag_NONE;

    console->open = true;
    
    Log("Envari Console Start");
}

static void SetupTexture(u32 textureID, u32 textureFiltering)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFiltering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFiltering);
}

static void CloseAllWindows() 
{
    editorConsole.open = false;
    editorPreview.open = false;
    assetsWindow.open = false;
    editorPerformanceDebugger.open = false;
    editorRenderDebugger.open = false;
    editorMemoryDebugger.open = false;
    editorTextureDebugger.open = false;
    editorSoundDebugger.open = false;
    editorInputDebugger.open = false;
    editorTimeDebugger.open = false;
    editorShaderDebugger.open = false;
#ifdef LUA_ENABLED
    editorLUADebugger.open = false;
    editorLUADebugger.codeOpen = false;
    editorLUADebugger.watchOpen = false;
    editorLUADebugger.stackOpen = false;
#endif
    editorConfig.open = false;
    editorHelp.open = false;
}

static void EditorCodeLayout()
{
    CloseAllWindows();
    
    if(ImGui::DockBuilderGetNode(editorState->dockspaceID)) {
        editorConsole.open = true;
        editorPreview.open = true;
        
        ImGui::DockBuilderRemoveNode(editorState->dockspaceID);
        ImGui::DockBuilderAddNode(editorState->dockspaceID);

        ImGuiID consoleID = ImGui::GetID("Console");
        ImGui::DockBuilderRemoveNode(consoleID);
        ImGui::DockBuilderAddNode(consoleID, ImGuiDockNodeFlags_None);

        ImGuiID previewID = ImGui::GetID("Preview");
        ImGui::DockBuilderRemoveNode(previewID);
        ImGui::DockBuilderAddNode(previewID, ImGuiDockNodeFlags_None);

        ImGuiID luaID = ImGui::GetID("LUA");
        ImGui::DockBuilderRemoveNode(luaID);
        ImGui::DockBuilderAddNode(luaID, ImGuiDockNodeFlags_None);

        ImGuiID dockMain = editorState->dockspaceID;
        ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.55f, NULL, &dockMain);
        ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Up, 0.35f, NULL, &dockMain);

        ImGui::DockBuilderDockWindow("Preview", dockLeft);
        ImGui::DockBuilderDockWindow("Console", dockTop);

#if LUA_ENABLED
        ImGui::DockBuilderDockWindow("LUA Code", dockMain);
        editorLUADebugger.codeOpen = true;
#endif

        ImGui::DockBuilderFinish(editorState->dockspaceID);
    }
}

static void EditorShadersLayout()
{
    CloseAllWindows();
    
    if(ImGui::DockBuilderGetNode(editorState->dockspaceID)) {
        editorConsole.open = true;
        editorPreview.open = true;
        editorRenderDebugger.open = true;
        
        ImGui::DockBuilderRemoveNode(editorState->dockspaceID);
        ImGui::DockBuilderAddNode(editorState->dockspaceID);

        ImGuiID consoleID = ImGui::GetID("Console");
        ImGui::DockBuilderRemoveNode(consoleID);
        ImGui::DockBuilderAddNode(consoleID, ImGuiDockNodeFlags_None);

        ImGuiID renderID = ImGui::GetID("Render");
        ImGui::DockBuilderRemoveNode(renderID);
        ImGui::DockBuilderAddNode(renderID, ImGuiDockNodeFlags_None);

        ImGuiID previewID = ImGui::GetID("Preview");
        ImGui::DockBuilderRemoveNode(previewID);
        ImGui::DockBuilderAddNode(previewID, ImGuiDockNodeFlags_None);

        ImGuiID luaID = ImGui::GetID("LUA");
        ImGui::DockBuilderRemoveNode(luaID);
        ImGui::DockBuilderAddNode(luaID, ImGuiDockNodeFlags_None);

        ImGuiID dockMain = editorState->dockspaceID;
        ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.55f, NULL, &dockMain);
        ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Up, 0.35f, NULL, &dockMain);
        ImGuiID dockTopRight;
        ImGuiID dockTopLeft = ImGui::DockBuilderSplitNode(dockTop, ImGuiDir_Left, 0.4f, NULL, &dockTopRight);

        ImGui::DockBuilderDockWindow("Preview", dockLeft);
        ImGui::DockBuilderDockWindow("Render", dockTopRight);
        ImGui::DockBuilderDockWindow("Console", dockTopLeft);

#if LUA_ENABLED
        ImGui::DockBuilderDockWindow("LUA Code", dockMain);
        editorLUADebugger.codeOpen = true;
#endif

        ImGui::DockBuilderDockWindow("Shaders", dockMain);
        editorShaderDebugger.open = true;

        ImGui::DockBuilderFinish(editorState->dockspaceID);
        
        ImGui::SetWindowFocus("Console");
        ImGui::SetWindowFocus("Shaders");
    }
}

static void EditorDefaultLayout()
{
    EditorShadersLayout();
}

static void EditorInit(PreviewWindow* preview)
{
    preview->linearFiltering = true;

    SetupTexture(gameState->render.renderBuffer, GL_LINEAR);
    SetupTexture(gameState->render.frameBuffer, GL_LINEAR);
}

static void EditorInit(AssetsWindow* debugger)
{
    debugger->pathLevel = 0;
    debugger->currentPath = filesystem::current_path();
}

static void EditorInit(AssetsViewer* debugger, filesystem::path filepath)
{
    debugger->lastAssetType = AssetType_NONE;
    debugger->assetType = GetAssetType(filepath);
    debugger->targetAssetPath = filepath;
}

static void EditorInit(PerformanceDebuggerWindow* debugger)
{    
    debugger->updateMinTime = FLT_MAX;
    debugger->updateMaxTime = FLT_MIN;
    debugger->luaUpdateMinTime = FLT_MAX;
    debugger->luaUpdateMaxTime = FLT_MIN;
}

static void EditorInit(RenderDebuggerWindow* debugger)
{
    debugger->recording = false;
    debugger->recordingFormat = RecordingFormat_PNG;
    debugger->jpgQuality = 100;
}

static void EditorInit(MemoryDebuggerWindow* debugger)
{
    
}

static void EditorInit(TextureDebuggerWindow* debugger)
{
    debugger->textureIndex = 0;
    debugger->inspectMode = TextureInspect_CACHE;
    debugger->textureChanged = true;
}

static void EditorInit(SoundDebuggerWindow* debugger)
{
    
}

static void EditorInit(InputDebuggerWindow* debugger)
{
    
}

static void EditorInit(TimeDebuggerWindow* debugger)
{
    debugger->frameTimeBuffer = (f32*)malloc(sizeof(f32) * TIME_BUFFER_SIZE);
    ZeroSize(TIME_BUFFER_SIZE, debugger->frameTimeBuffer);
    debugger->frameTimeMax = 1;
    debugger->fpsBuffer = (f32*)malloc(sizeof(f32) * TIME_BUFFER_SIZE);
    ZeroSize(TIME_BUFFER_SIZE, debugger->fpsBuffer);
    debugger->fpsMax = 1;

    debugger->framesMultiplier = 1;
    debugger->timeScale = 1;

    debugger->loopFormat = gameState->time.fpsFixed != -1 ? TimeFormat_FRAMES : TimeFormat_TIME;
}

static void EditorInit(ShaderDebuggerWindow* debugger)
{
    debugger->programIndex = -1;
}

#ifdef LUA_ENABLED
static void EditorInit(LUADebuggerWindow* debugger)
{
    debugger->codeOpen = true;
    debugger->watchOpen = false;
    debugger->stackOpen = false;
}
#endif

static void EditorInit(EditorConfigWindow* config)
{
    if(config->runtimesPath == 0) {
        config->runtimesPath = AllocateDynamicString(stringAllocator, "", 32);
        config->dataPath = AllocateDynamicString(stringAllocator, "", 32);

        windows86OutputConfig.outputPath = AllocateDynamicString(stringAllocator, "", 32);
        windows64OutputConfig.outputPath = AllocateDynamicString(stringAllocator, "", 32);
        androidOutputConfig.outputPath = AllocateDynamicString(stringAllocator, "", 32);
        wasmOutputConfig.outputPath = AllocateDynamicString(stringAllocator, "", 32);
    }
}

static void EditorInit(HelpWindow* help)
{
    
}

static i32 ConsoleInputEditCallback(ImGuiInputTextCallbackData* data)
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
            for (i32 i = 0; i < editorConsole.commands.Size; i++) {
                if (Strnicmp(editorConsole.commands[i], word_start, (i32)(word_end-word_start)) == 0) {
                    candidates.push_back(editorConsole.commands[i]);
                }
            }

            if (candidates.Size == 0) {
                // No match
                Log("No match for \"%.*s\"!\n", (i32)(word_end-word_start), word_start);
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
            const i32 prev_history_pos = editorConsole.historyPos;
            if (data->EventKey == ImGuiKey_UpArrow) {
                if (editorConsole.historyPos == -1) {
                    editorConsole.historyPos = editorConsole.history.Size - 1;
                }
                else if (editorConsole.historyPos > 0) {
                    editorConsole.historyPos--;
                }
            }
            else if (data->EventKey == ImGuiKey_DownArrow) {
                if (editorConsole.historyPos != -1) {
                    if (++editorConsole.historyPos >= editorConsole.history.Size) {
                        editorConsole.historyPos = -1;
                    }
                }
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != editorConsole.historyPos) {
                const char* history_str = (editorConsole.historyPos >= 0) ? editorConsole.history[editorConsole.historyPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
    }
    return 0;
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

    // #PERFORMANCE(Juan): Optimize this 
    // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
    console->historyPos = -1;
    for (i32 i = console->history.Size-1; i >= 0; i--) {
        if (Stricmp(console->history[i], command_line) == 0) {
            free(console->history[i]);
            console->history.erase(console->history.begin() + i);
            break;
        }
    }

    if(CONSOLE_HISTORY_SIZE > 0 && console->history.Size >= CONSOLE_HISTORY_SIZE) {
        free(console->history[0]);
        console->history.erase(console->history.begin());
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
#ifdef LUA_ENABLED
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

static void EditorLogMenuButton(ConsoleWindow* console, const char* flagName, u32 flag)
{
    bool value = (console->logFlags & flag) > 0;
    if (ImGui::Checkbox(flagName, &value)) { console->logFlags = (value ? console->logFlags | flag : console->logFlags & ~flag); }
}

static void PushConsoleStyleColor(ConsoleLogType type)
{
    switch(type) {
        case ConsoleLogType_NORMAL: {
            
        } break;
        case ConsoleLogType_ERROR: {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        } break;
        case ConsoleLogType_WARNING: {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.95f, 0.3f, 1.0f));
        } break;
        case ConsoleLogType_COMMAND: {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
        } break;
    }
}

static void PopConsoleStyleColor(ConsoleLogType type)
{
    if (type != ConsoleLogType_NORMAL) {
        ImGui::PopStyleColor();
    }
}

static u32 GameInit();
ConsoleLog *inspectedLog = 0;
static void EditorDraw(ConsoleWindow* console)
{
    if(!console->open) { return; }
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowSize(ImVec2(600,600), ImGuiCond_FirstUseEver);

    if (console->open && !ImGui::Begin("Console", &console->open, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Main"))
        {
            if (!gameState->game.updateRunning && ImGui::MenuItem("Play")) { gameState->game.updateRunning = true; }
            if (gameState->game.updateRunning && ImGui::MenuItem("Pause")) { gameState->game.updateRunning = false; }
            if (ImGui::MenuItem("Reset", "CTRL+R")) {
                CleanCache();
                
                ResetArena(&sceneState->arena);

                gameState->time.gameTime = 0;
                gameState->time.gameFrames = 0;

                GameInit();

#if LUA_ENABLED
                LoadScriptString(editorLUADebugger.currentFileBuffer);
                // #TODO (Juan): This is not the best solution because we are running Load twice, but we can fix it later
                RunLUAProtectedFunction(Load);
#endif
            }
            if (ImGui::BeginMenu("Build")) {
                for(i32 platformIndex = 0; platformIndex < ArraySize(platformNames); platformIndex++) {
                    if (ImGui::MenuItem(platformNames[platformIndex])) { BuildPlatform((RuntimePlatform)platformIndex); }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::Checkbox("Assets", &assetsWindow.open)) { EditorInit(&assetsWindow); }
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
            if (ImGui::Checkbox("Shaders", &editorShaderDebugger.open)) { EditorInit(&editorShaderDebugger); }
#ifdef LUA_ENABLED
            if (ImGui::Checkbox("LUA", &editorLUADebugger.open)) { EditorInit(&editorLUADebugger); }
            RunLUAProtectedFunction(EditorConsoleDebugBar)
#endif
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Config"))
        {
            if (ImGui::Checkbox("Editor", &editorConfig.open)) { EditorInit(&editorConfig); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Layout"))
        {
            if (ImGui::MenuItem("Code")) { EditorCodeLayout(); }
            if (ImGui::MenuItem("Shaders")) { EditorShadersLayout(); }
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
        EditorLogMenuButton(console, "Performance", LogFlag_PERFORMANCE);
        EditorLogMenuButton(console, "Render", LogFlag_RENDER);
        EditorLogMenuButton(console, "Memory", LogFlag_MEMORY);
        EditorLogMenuButton(console, "Texture", LogFlag_TEXTURE);
        EditorLogMenuButton(console, "Sound", LogFlag_SOUND);
        EditorLogMenuButton(console, "Input", LogFlag_INPUT);
        EditorLogMenuButton(console, "Time", LogFlag_TIME);
        EditorLogMenuButton(console, "LUA", LogFlag_LUA);
        
        EditorLogMenuButton(console, "System", LogFlag_SYSTEM);
        EditorLogMenuButton(console, "Game", LogFlag_GAME);
        EditorLogMenuButton(console, "Scripting", LogFlag_SCRIPTING);
        EditorLogMenuButton(console, "Scripting Functions", LogFlag_SCRIPTING_FUNCTIONS);

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

        PushConsoleStyleColor(currentLog->type);

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
        
        PopConsoleStyleColor(currentLog->type);
    }

    ImGui::NextColumn();

    for (i32 i = 0; i < console->items.Size; i++) {
        ConsoleLog* currentLog = &console->items[i];
        if (!console->filter.PassFilter(currentLog->log)) {
            continue;
        }
        
        PushConsoleStyleColor(currentLog->type);
        
        ImGui::Text("%d", currentLog->count);

        if(ImGui::IsItemClicked()) {
            ImGui::OpenPopup("LogDetails");
            inspectedLog = currentLog;
        }
        
        PopConsoleStyleColor(currentLog->type);
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
    if (ImGui::InputText("Input", console->inputBuffer, IM_ARRAYSIZE(console->inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &ConsoleInputEditCallback)) {
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
    if(!preview->open) { return; }
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
            if(ImGui::Checkbox("Linear Filtering", &preview->linearFiltering)) {
                u32 filterMethod = 0;
                if(preview->linearFiltering) { filterMethod = GL_LINEAR; }
                else { filterMethod = GL_NEAREST; }
                SetupTexture(gameState->render.renderBuffer, filterMethod);
                SetupTexture(gameState->render.frameBuffer, filterMethod);
            }
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

    bool lastCursorInsideWindow = preview->cursorInsideWindow;
    preview->cursorInsideWindow = cursorPosition.x > previewMin.x && cursorPosition.x < previewMax.x && cursorPosition.y > previewMin.y && cursorPosition.y < previewMax.y;

    if(lastCursorInsideWindow != preview->cursorInsideWindow) {
        gameState->game.hasFocus = preview->cursorInsideWindow;
#if LUA_ENABLED
        RunLUAProtectedFunction(FocusChange);
#endif
    }

    preview->lastCursorPosition = preview->cursorPosition;
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

#if PLATFORM_WINDOWS
    ImGui::Image((ImTextureID)gameState->render.frameBuffer, ImVec2(gameState->render.size.x, gameState->render.size.y), ImVec2(0, 1), ImVec2(1, 0));
#else
    ImGui::Image((ImTextureID)(gameState->render.frameBuffer - 1), ImVec2(gameState->render.size.x, gameState->render.size.y), ImVec2(0, 1), ImVec2(1, 0));
#endif

    if(preview->showData) {
        previewMin = ImGui::GetWindowPos();
        previewMin.y += height;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(previewMin, previewMin + ImVec2(200, 70), IM_COL32(0, 0, 0, 200), 0.0f, 1);

        ImGui::SetCursorPos(ImVec2(5, height + 3));
        ImGui::Text("Render Size: %d, %d", (i32)gameState->render.size.x, (i32)gameState->render.size.y);
        ImGui::SetCursorPos(ImVec2(5, height + 23));
        ImGui::Text("Buffer Size: %d, %d", (i32)gameState->render.bufferSize.x, (i32)gameState->render.bufferSize.y);
        ImGui::SetCursorPos(ImVec2(5, height + 43));
        ImGui::Text("Scaled Buffer Size: %d, %d", (i32)gameState->render.scaledBufferSize.x, (i32)gameState->render.scaledBufferSize.y);
    }

    if(!preview->open) {        
        gameState->render.size.x = gameState->render.windowSize.x;
        gameState->render.size.y = gameState->render.windowSize.y;
    }

    ImGui::End();
}

static void EditorDraw(AssetsWindow* debugger)
{
    if(!debugger->open) { return; }

    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Assets", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("This menu is a WIP");
    
    ImGui::Separator();

    ImGui::Text("%s", debugger->currentPath.string().c_str());

    if(debugger->pathLevel > 0 && debugger->currentPath.has_relative_path() && ImGui::Button("<")) {
        debugger->currentPath = debugger->currentPath.parent_path();
        debugger->pathLevel--;
    }

    for(auto entry : filesystem::directory_iterator(debugger->currentPath)) {
        std::string string = entry.path().filename().string();
        if(ImGui::Button(string.c_str())) {
            if(entry.is_directory()) {
                debugger->currentPath = entry.path();
                debugger->pathLevel++;
            }
            else {
                assetsViewer.open = true;
                EditorInit(&assetsViewer, entry.path());
            }
        }
    }

    ImGui::End();
}

static void EditorDraw(AssetsViewer* debugger)
{
    if(!debugger->open) { return; }

    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Asset Viewer", &debugger->open)) {
        ImGui::End();
        return;
    }

    if(debugger->assetType != debugger->lastAssetType) {
        if(debugger->targetAsset != 0) {
            free(debugger->targetAsset);
        }

        debugger->lastAssetType = debugger->assetType;
        debugger->targetAsset = 0;
    }

    ImGui::Text("Asset Type %s", assetTypeLabels[(i32)debugger->assetType]);

    ImGui::Separator();

    switch(debugger->assetType) {
        case AssetType_NONE:
        case AssetType_UNKNOWN: {
            ImGui::Text("No asset viewer available");
            break;
        }
        case AssetType_IMAGE: {
            TextureAsset* texture;
            if(debugger->targetAsset == 0) {
                texture = (TextureAsset*)malloc(sizeof(TextureAsset));
                *texture = LoadTextureFile(debugger->targetAssetPath.string().c_str());
                debugger->targetAsset = (void*)texture;
            }
            else {
                texture = (TextureAsset*)debugger->targetAsset;
            }

            ImGui::Image((ImTextureID)texture->textureID, ImVec2((f32)texture->width, (f32)texture->height));
            break;
        }
        case AssetType_TEXT: {  
            TextAsset* text;
            if(debugger->targetAsset == 0) {
                text = (TextAsset*)malloc(sizeof(TextAsset));
                *text = LoadFileToMemory(debugger->targetAssetPath.string().c_str(), FILE_MODE_READ_BINARY);
                debugger->targetAsset = (void*)text;
            }
            else {
                text = (TextAsset*)debugger->targetAsset;
            }

            ImGui::Text("%s", text->data);
            
            break;
        }
        case AssetType_SOUND: {
            ImGui::SliderFloat("Volume", &previewInstance.volumeModifier, 0.0f, 1.0f);
            if(ImGui::Button("Play")) {
                EditorPlayPreviewSound(debugger->targetAssetPath.string().c_str(), 1.0f);
            }
            ImGui::BeginDisabled(previewInstance.decoder == 0);
            ImGui::SameLine();
            if(ImGui::Button("Stop")) {
                EditorStopPreviewSound();
            }
            ImGui::EndDisabled();
            break;
        }
    }

    ImGui::End();
}

static void EditorDraw(PerformanceDebuggerWindow* debugger)
{
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Performance", &debugger->open)) {
        ImGui::End();
        return;
    }
    
    if(debugger->updateTime != 0) {
        debugger->updateMinTime = MIN(debugger->updateTime, debugger->updateMinTime);
        debugger->updateMaxTime = MAX(debugger->updateTime, debugger->updateMaxTime);
        ImGui::Text("Update: %f (%f / %f) ms %" PRIu64 " cycles", debugger->updateTime, debugger->updateMinTime, debugger->updateMaxTime, debugger->updateCycles);

        debugger->luaUpdateMinTime = MIN(debugger->luaUpdateTime, debugger->luaUpdateMinTime);
        debugger->luaUpdateMaxTime = MAX(debugger->luaUpdateTime, debugger->luaUpdateMaxTime);
        ImGui::Text("LUA Update: %f (%f / %f) ms %" PRIu64 " cycles", debugger->luaUpdateTime, debugger->luaUpdateMinTime, debugger->luaUpdateMaxTime, debugger->luaUpdateCycles);
    }
    
    ImGui::End();
}

static void EditorDraw(RenderDebuggerWindow* debugger)
{
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Render", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Window size: %.0fx%.0f", gameState->render.windowSize.x, gameState->render.windowSize.y);
    ImGui::Text("Buffer size: %.0fx%.0f", gameState->render.bufferSize.x, gameState->render.bufferSize.y);
    v2 lastSize = gameState->render.scaledBufferSize;
    if(ImGui::InputFloat2("Scaled Buffer Size", gameState->render.scaledBufferSize.e)) {
        if(gameState->render.scaledBufferSize.x != lastSize.x) {
            gameState->render.renderScale = gameState->render.scaledBufferSize.x / gameState->render.bufferSize.x;
        }
        else {
            gameState->render.renderScale = gameState->render.scaledBufferSize.y / gameState->render.bufferSize.y;
        }
        gameState->render.scaledBufferSize.x = gameState->render.bufferSize.x * gameState->render.renderScale;
        gameState->render.scaledBufferSize.y = gameState->render.bufferSize.y * gameState->render.renderScale;
        ResizeFramebufferGL((i32)gameState->render.scaledBufferSize.x, (i32)gameState->render.scaledBufferSize.y);
    }

    if(ImGui::SliderFloat("Render scale", &gameState->render.renderScale, 0.01f, 8.0f)) {
        gameState->render.scaledBufferSize.x = gameState->render.bufferSize.x * gameState->render.renderScale;
        gameState->render.scaledBufferSize.y = gameState->render.bufferSize.y * gameState->render.renderScale;
        ResizeFramebufferGL((i32)gameState->render.scaledBufferSize.x, (i32)gameState->render.scaledBufferSize.y);
    }
    
    ImGui::Separator();
    
    ImGui::Text("Render memory size: %zu", debugger->renderMemory);
    ImGui::Text("Draw count: %d", debugger->drawCount);
    ImGui::Text("Program changes: %d", debugger->programChanges);
    
    ImGui::Separator();

    i32 total = TotalGPUMemoryKB();
    ImGui::Text("GPU memory: %d / %d", total - AvailableGPUMemoryKB(), total);
    
    ImGui::Separator();

    ImGui::Checkbox("Wireframe Mode", &debugger->wireframeMode);
#if PLATFORM_WINDOWS
    ImGui::Separator();

    ImGui::Checkbox("Record frames", &debugger->recording);

    i32 formatIndex = (i32)debugger->recordingFormat;
    ImGui::Combo("Format", &formatIndex, recordingFormatExtensions, ArrayCount(recordingFormatExtensions));
    debugger->recordingFormat = (RecordingFormat)formatIndex;
    ImGui::SliderInt("JPG Quality", &debugger->jpgQuality, 1, 100);
#endif
    ImGui::Separator();

    if(ImGui::SmallButton(editorState->editorFrameRunning ? "Stop" : "Run")) {
        editorState->editorFrameRunning = !editorState->editorFrameRunning;
    }

    if(!editorState->editorFrameRunning) {
        ImGui::SameLine();
        editorState->playNextFrame = false;
        if(ImGui::SmallButton("Next Frame")) {
            editorState->playNextFrame = true;
        }
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

    if(ImGui::TreeNode("Render Queue")) {        
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
#if PLATFORM_EDITOR
                    ImGui::Text("Temp Data (%s)", tempData->header.debugData);
#else
                    ImGui::Text("Temp Data");
#endif
                } break;
                case RenderType_RenderClear: {
                    RenderClear *clear = (RenderClear *)renderHeader;
                    ImGui::Text("Clear -> %.3f, %.3f, %.3f, %.3f", clear->color.r, clear->color.g, clear->color.b, clear->color.a);
                } break;
                case RenderType_RenderSetStyle: {
                    RenderSetStyle *setStyle = (RenderSetStyle *)renderHeader;
                    ImGui::Text("Set Style -> Normal: %s,\nHovered: %s,\nDown: %s", setStyle->style.slicedFilepath, setStyle->style.slicedHoveredFilepath, setStyle->style.slicedDownFilepath);
                } break;
                case RenderType_RenderSetTransform: {
                    RenderSetTransform *setTransform = (RenderSetTransform *)renderHeader;
                    ImGui::Text("Set Transform");
                } break;
                case RenderType_RenderPushTransform: {
                    RenderPushTransform *pushTransform = (RenderPushTransform *)renderHeader;
                    ImGui::Text("Push Transform");
                } break;
                case RenderType_RenderPopTransform: {
                    RenderPopTransform *popTransform = (RenderPopTransform *)renderHeader;
                    ImGui::Text("Pop Transform");
                } break;
                case RenderType_RenderColor: {
                    RenderColor *color = (RenderColor *)renderHeader;
                    ImGui::Text("Color -> %.3f, %.3f, %.3f, %.3f", color->color.r, color->color.g, color->color.b, color->color.a);
                } break;
                case RenderType_RenderLayer: {
                    RenderLayer *layer = (RenderLayer *)renderHeader;
                    ImGui::Text("Layer -> %d", layer->layer);
                } break;
                case RenderType_RenderTransparent: {                    
                    RenderTransparent *transparent = (RenderTransparent *)renderHeader;
                    ImGui::Text("Transparent -> On: %u, Mode: %u %u, RGB: %u %u, Alpha: %u %u", transparent->enabled, transparent->modeRGB, transparent->modeAlpha, 
                        transparent->srcRGB, transparent->dstRGB, transparent->srcAlpha, transparent->dstAlpha);
                } break;
                case RenderType_RenderLineWidth: {
                    RenderLineWidth *line = (RenderLineWidth *)renderHeader;
                    ImGui::Text("Line Width -> %.3f", line->width);
                } break;
                case RenderType_RenderLine: {
                    RenderLine *line = (RenderLine *)renderHeader;
                    ImGui::Text("Line -> Start: %.3f %.3f, End: %.3f %.3f", line->start.x, line->start.y, line->end.x, line->end.y);
                } break;
                case RenderType_RenderTriangle: {
                    RenderTriangle *triangle = (RenderTriangle *)renderHeader;
                    ImGui::Text("Triangle -> P1: %.3f %.3f, P2: %.3f %.3f, P3: %.3f %.3f", triangle->point1.x, triangle->point1.y,
                        triangle->point2.x, triangle->point2.y,
                        triangle->point3.x, triangle->point3.y);
                } break;
                case RenderType_RenderRectangle: {
                    RenderRectangle *rectangle = (RenderRectangle *)renderHeader;
                    ImGui::Text("Rectangle -> Position: %.3f %.3f, Scale: %.3f %.3f", rectangle->origin.x, rectangle->origin.y, rectangle->size.x, rectangle->size.y);
                } break;
                case RenderType_RenderCircle: {
                    RenderCircle *circle = (RenderCircle *)renderHeader;
                    ImGui::Text("Circle -> Position: %.3f %.3f, Radius: %.3f, Segments: %d", circle->origin.x, circle->origin.y, circle->radius, circle->segments);
                } break;
                case RenderType_RenderTextureParameters: {
                    RenderTextureParameters *textureParameters = (RenderTextureParameters *)renderHeader;
                    ImGui::Text("Texture Parameters -> %s, %s, %s, %s", GetWrapTypeName(textureParameters->wrapS), GetWrapTypeName(textureParameters->wrapT),
                        GetFilteringTypeName(textureParameters->minFilter), GetFilteringTypeName(textureParameters->magFilter));
                } break;
                case RenderType_RenderTexture: {
                    RenderTexture *texture = (RenderTexture *)renderHeader;
                    ImGui::Text("Texture -> Position: %.3f %.3f, Scale: %.3f %.3f, ID: %u", texture->origin.x, texture->origin.y, texture->size.x, texture->size.y,
                        texture->textureID);
                } break;
                case RenderType_RenderImage: {
                    RenderImage *image = (RenderImage *)renderHeader;
                    ImGui::Text("Image -> Position: %.3f %.3f,\n\tFile: %s", image->origin.x, image->origin.y, image->filepath);
                } break;
                case RenderType_RenderImageUV: {
                    RenderImageUV *imageUV = (RenderImageUV *)renderHeader;
                    ImGui::Text("Image UV -> Position: %.3f %.3f, UVMin: %.3f %.3f, UVMax: %.3f %.3f,\n\tFile: %s", imageUV->origin.x, imageUV->origin.y,
                        imageUV->uvMin.x, imageUV->uvMin.y, imageUV->uvMax.x, imageUV->uvMax.y,
                        imageUV->filepath);
                } break;
                case RenderType_RenderImage9Slice: {
                    RenderImage9Slice *image9Slice = (RenderImage9Slice *)renderHeader;
                    ImGui::Text("9 Slice -> Position: %.3f %.3f, End Position: %.3f %.3f, Slice: %.3f,\n\tFile: %s", image9Slice->origin.x, image9Slice->origin.y,
                        image9Slice->endOrigin.x, image9Slice->endOrigin.y,
                        image9Slice->slice, image9Slice->filepath);
                } break;
                case RenderType_RenderAtlasSprite: {
                    RenderAtlasSprite *atlas = (RenderAtlasSprite *)renderHeader;
                    ImGui::Text("Atlas Sprite -> Position: %.3f %.3f,\n\tFile: %s\n\tAtlas: %s\n\tSprite: %s", atlas->origin.x, atlas->origin.y, 
                        atlas->filepath, atlas->atlasName, atlas->spriteKey);
                } break;
                case RenderType_RenderFont: {
                    RenderFont *font = (RenderFont *)renderHeader;
                    ImGui::Text("Font ID: %d", font->fontID);
                } break;
                case RenderType_RenderChar: {
                    RenderChar *renderChar = (RenderChar *)renderHeader;
                    ImGui::Text("Char -> Position: %.3f %.3f, Char: %c", renderChar->origin.x, renderChar->origin.y,
                        renderChar->singleChar);
                } break;
                case RenderType_RenderText: {
                    RenderText *text = (RenderText *)renderHeader;
                    ImGui::Text("Text -> Position: %.3f %.3f, String: %s,\n\tSize: %zu", text->origin.x, text->origin.y,
                        text->string, text->stringSize);
                } break;
                case RenderType_RenderStyledText: {
                    RenderStyledText *styledText = (RenderStyledText *)renderHeader;
                    ImGui::Text("Styled Text -> Position: %.3f %.3f, String: %s,\n\tSize: %zu, Center: %u, LetterWrap: %u, WordWrap: %u", styledText->origin.x, styledText->origin.y,
                        styledText->string, styledText->stringSize, (styledText->header.renderFlags & TextRenderFlag_Center) > 0,
                        (styledText->header.renderFlags & TextRenderFlag_LetterWrap) > 0, (styledText->header.renderFlags & TextRenderFlag_WordWrap) > 0
                    );
                } break;
                case RenderType_RenderButton: {
                    RenderButton *button = (RenderButton *)renderHeader;
                    ImGui::Text("Button -> Start: %.3f %.3f, End: %.3f %.3f\n\tLabel: %s", button->origin.x, button->origin.y, button->endOrigin.x, button->endOrigin.y, button->label);
                } break;
                case RenderType_RenderSetUniform: {
                    RenderSetUniform *uniform = (RenderSetUniform *)renderHeader;
                    ImGui::Text("Set Uniform");
                } break;
                case RenderType_RenderOverrideProgram: {
                    RenderOverrideProgram *program = (RenderOverrideProgram *)renderHeader;
                    ImGui::Text("Override Program");
                } break;
                case RenderType_RenderOverrideVertices: {
                    RenderOverrideVertices *vertices = (RenderOverrideVertices *)renderHeader;
                    ImGui::Text("Override Vertices");
                } break;
                case RenderType_RenderOverrideIndices: {
                    RenderOverrideIndices *indices = (RenderOverrideIndices *)renderHeader;
                    ImGui::Text("Override Indices");
                } break;
                default: {
#if PLATFORM_EDITOR
                    ImGui::Text("Unknown render command! Header Type %d, Size %zu, Debug: %s", renderHeader->type, renderHeader->size, renderHeader->debugData);
#else
                    ImGui::Text("Unknown render command! Header Type %d, Size %d", renderHeader->type, renderHeader->size);
#endif
                } break;
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

            AssertMessage(renderHeader->size > 0, "Editor render loop error: header has no size");
            renderHeader = (RenderHeader *)((u8 *)renderHeader + renderHeader->size);
        }
        
        ImGui::TreePop();
    }
    
    ImGui::Separator();

    if(ImGui::TreeNode("Render State")) {
        if(ImGui::TreeNode("Render Style")) {
            ImGui::Text("Sliced Path: %s", renderState->style.slicedFilepath);
            ImGui::Text("Sliced Hovered Path: %s", renderState->style.slicedHoveredFilepath);
            ImGui::Text("Sliced Down Path: %s", renderState->style.slicedDownFilepath);
            ImGui::Text("Slice: %.3fpx", renderState->style.slice);

            ImGui::TreePop();
        }

        ImGui::Text("Last Render ID: %d", renderState->lastRenderID);
        ImGui::Text("Render Color: R %.3f, G %.3f, B %.3f, A %.3f", renderState->renderColor.r, renderState->renderColor.g, renderState->renderColor.b, renderState->renderColor.a);
        ImGui::Text("Used Layers: %u", renderState->usedLayers);
        ImGui::Text("Transparent Layers: %u", renderState->transparentLayers);
        ImGui::Text("Texture Wrap S: %s", GetWrapTypeName(renderState->wrapS));
        ImGui::Text("Texture Wrap T: %s", GetWrapTypeName(renderState->wrapT));
        ImGui::Text("Texture Min Filter: %s", GetFilteringTypeName(renderState->minFilter));
        ImGui::Text("Texture Mag Filter: %s", GetFilteringTypeName(renderState->magFilter));
        ImGui::Text("Current Program: %u", renderState->currentProgram);
        ImGui::Text("Override Program: %u", renderState->overrideProgram);
        ImGui::Text("Overriding Vertices: %u", renderState->overridingVertices);
        ImGui::Text("Overriding Indices: %u", renderState->overridingIndices);
        ImGui::Text("Generate MipMaps: %u", renderState->generateMipMaps);

        ImGui::TreePop();
    }

    ImGui::End();
}

static void EditorDraw(MemoryDebuggerWindow* debugger)
{
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Memory", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Permanent memory: %zu / %zu", permanentState->arena.used, permanentState->arena.size);
    ImGui::Text("Scene memory: %zu / %zu", sceneState->arena.used, sceneState->arena.size);
    ImGui::Text("Temporal memory: %zu / %zu", temporalState->arena.used, temporalState->arena.size);

    ImGui::Separator();

#if PLATFORM_WINDOWS
    ImGui::Text("Memory: %d", debugger->memoryCounters.WorkingSetSize);
    ImGui::Text("Peak memory: %d", debugger->memoryCounters.PeakWorkingSetSize);

    if(ImGui::TreeNode("Page Data")) {
        ImGui::Text("Page faults: %d", debugger->memoryCounters.PageFaultCount);
        ImGui::Text("Page file usage: %d", debugger->memoryCounters.PagefileUsage);
        ImGui::Text("Peak page file usage: %d", debugger->memoryCounters.PeakPagefileUsage);
        ImGui::Text("Quota page pool usage: %d", debugger->memoryCounters.QuotaPagedPoolUsage);
        ImGui::Text("Quota non page pool usage: %d", debugger->memoryCounters.QuotaNonPagedPoolUsage);
        ImGui::Text("Quota peak page pool usage: %d", debugger->memoryCounters.QuotaPeakPagedPoolUsage);
        ImGui::Text("Quota peak non page pool usage: %d", debugger->memoryCounters.QuotaPeakNonPagedPoolUsage);
    
        ImGui::TreePop();
    }
#endif

#if LUA_ENABLED
    ImGui::Separator();
    ImGui::Text("LUA Memory: %d", (i32)lua.memory_used());
    ImGui::Text("Supports CG Incremental: %s", lua.supports_gc_mode(sol::gc_mode::incremental) ? "true" : "false");
    ImGui::Text("Supports CG Generational: %s", lua.supports_gc_mode(sol::gc_mode::generational) ? "true" : "false");
    ImGui::Text("CG On: %s", lua.is_gc_on() ? "true" : "false");
    if (ImGui::SmallButton("Force GC")) {
        lua.collect_garbage();
    }
#endif

    ImGui::Separator();
    ImGui::Text("Dynamic Strings");
    ImGui::Text("String reallocs on asign last frame: %zu", stringAllocator->stringReallocOnAsignLastFrame);
    ImGui::Text("Strings allocated last frame: %zu", stringAllocator->stringsAllocatedLastFrame);
    ImGui::Text("Total strings rellocated: %zu", stringAllocator->totalStringsReallocated);
    ImGui::Text("Total strings allocated: %zu", stringAllocator->totalStringsAllocated);
    
    ImGui::End();
}

static void EditorDraw(TextureDebuggerWindow* debugger)
{
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Texture", &debugger->open)) {
        ImGui::End();
        return;
    }

    if (ImGui::SmallButton("Cache")) {
        debugger->inspectMode = TextureInspect_CACHE;
        debugger->textureChanged = true;
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("All")) {
        debugger->inspectMode = TextureInspect_ALL;
        debugger->textureChanged = true;
    }

    ImGui::Separator();

    i32 textureID = 0;

    i32 textureCacheSize = (i32)shlen(textureCache);
    if(debugger->inspectMode == TextureInspect_CACHE && textureCacheSize > 0)
    {
        ImGui::Text("Texture Cache ID: %d / %d", debugger->textureIndex + 1, textureCacheSize);
        
        ImGui::SameLine();
        if (ImGui::SmallButton("Prev")) {
            debugger->textureIndex--;
            debugger->textureChanged = true;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Next")) {
            debugger->textureIndex++;
            debugger->textureChanged = true;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear Cache")) {
            CleanCache();
        }

        if(debugger->textureIndex < 0) { debugger->textureIndex = textureCacheSize - 1; }
        if(debugger->textureIndex >= textureCacheSize) { debugger->textureIndex = 0; }

        TextureAsset cachedTexture = textureCache[debugger->textureIndex].value;

        ImGui::Text("Texture ID: %d", cachedTexture.textureID);
        
#if PLATFORM_WINDOWS
        if(ImGui::InputInt("Texture Level", &debugger->textureLevel, 1, 1)) {
            if(debugger->textureLevel < 0) {
                debugger->textureLevel = 0;
            }
            debugger->textureChanged = true;
        }

        if(debugger->textureChanged) {
            glBindTexture(GL_TEXTURE_2D, cachedTexture.textureID);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, debugger->textureLevel, GL_TEXTURE_WIDTH, &debugger->textureSize.x);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, debugger->textureLevel, GL_TEXTURE_HEIGHT, &debugger->textureSize.y);
        }
#else
        if(debugger->textureChanged) {
            debugger->textureSize.x = 512;
            debugger->textureSize.y = 512;
        }
        ImGui::InputInt2("Width/Height", debugger->textureSize.e);
#endif

        textureID = cachedTexture.textureID;
    }
    else if(debugger->inspectMode == TextureInspect_ALL) {
        if(ImGui::InputInt("Texture ID", &debugger->textureIndex, 1, 1)) {
            if(debugger->textureIndex < 0) {
                debugger->textureIndex = 0;
            }

            debugger->textureLevel = 0;
            debugger->textureChanged = true;
        }
#if PLATFORM_WINDOWS
        if(ImGui::InputInt("Texture Level", &debugger->textureLevel, 1, 1)) {
            if(debugger->textureLevel < 0) {
                debugger->textureLevel = 0;
            }
            debugger->textureChanged = true;
        }

        if(debugger->textureChanged) {
            glBindTexture(GL_TEXTURE_2D, debugger->textureIndex);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, debugger->textureLevel, GL_TEXTURE_WIDTH, &debugger->textureSize.x);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, debugger->textureLevel, GL_TEXTURE_HEIGHT, &debugger->textureSize.y);
        }
#else
        ImGui::InputInt2("Width/Height", debugger->textureSize.e);
#endif
        
        textureID = debugger->textureIndex;
    }

    ImGui::Text("Texture Size: %d x %d", debugger->textureSize.x, debugger->textureSize.y);
    
    ImGui::Separator();

    ImGui::BeginChild("Texture", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
    ImGui::Image((ImTextureID)textureID, ImVec2((f32)debugger->textureSize.x, (f32)debugger->textureSize.y));
    ImGui::EndChild();

    ImGui::End();
}

static void EditorDraw(SoundDebuggerWindow* debugger)
{
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Sound", &debugger->open)) {
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
    
    if(ImGui::TreeNode("Sound cache")) {
        ImGui::Text("Sound cache needs to be reimplemented");
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

        ImGui::TreePop();
    }

    ImGui::End();
}

static void EditorDraw(InputDebuggerWindow* debugger)
{
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Input", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Mouse texture ID: %d", gameState->input.mouseTextureID);
    ImGui::Text("Mouse position\tX: %f\tY: %f", gameState->input.mousePosition.x, gameState->input.mousePosition.y);
    ImGui::Text("Mouse screen  \tX: %f\tY: %f", gameState->input.mouseScreenPosition.x, gameState->input.mouseScreenPosition.y);
    ImGui::Text("Mouse wheel:  \t%d", gameState->input.mouseWheel);

    ImGui::Text("Text input buffer: %s", gameState->input.textInputBuffer->value);

    float elementSize = 50;

    ImGui::TextUnformatted("Mouse");
    ImGui::Text("Any mouse state: %d", gameState->input.anyMouseState);
    ImGui::PushItemWidth(elementSize);
    for(i32 i = 0; i < MOUSE_COUNT; ++i) {
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
    ImGui::Text("Any key state: %d", gameState->input.anyKeyState);
    for(i32 i = 0; i < KEY_COUNT; ++i) {
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
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Time", &debugger->open)) {
        ImGui::End();
        return;
    }

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    float width = contentMax.x - contentMin.x;

    ImGui::Text("Delta time: %f", gameState->time.deltaTime);
    ImGui::Text("Game time: %f", gameState->time.gameTime);
    ImGui::Text("Last frame game time: %f", gameState->time.lastFrameGameTime);

    ImGui::Separator();
    
    ImGui::Text("Frames: %" PRIi64, gameState->time.frames);
    ImGui::Text("Game frames: %" PRIi64, gameState->time.gameFrames);

    ImGui::Separator();

    if(gameState->time.fpsFixed != -1) {
        ImGui::Checkbox("Time loop", &debugger->timeloop);
        
        i32 formatIndex = (i32)debugger->loopFormat;
        ImGui::Combo("Loop type", &formatIndex, timeFormatsLabels, ArrayCount(timeFormatsLabels));
        debugger->loopFormat = (TimeFormat)formatIndex;

        if(debugger->loopFormat == TimeFormat_FRAMES) {
            ImGui::InputInt("Loop start frame", &debugger->loopStartFrame, 1, 1);
            ImGui::InputInt("Loop end frame", &debugger->loopEndFrame, 1, 1);
        }
        else if(debugger->loopFormat == TimeFormat_TIME) {
            ImGui::InputFloat("Loop start time", &debugger->loopStartTime, 1, 1);
            ImGui::InputFloat("Loop end time", &debugger->loopEndTime, 1, 1);
        }

        ImGui::InputInt("Frame multiplier", &debugger->framesMultiplier, 1, 1);
        if(debugger->framesMultiplier < 1) { debugger->framesMultiplier = 1; }

        ImGui::Separator();
    }
    else {
        ImGui::Checkbox("Time loop", &debugger->timeloop);

        ImGui::InputFloat("Loop start time", &debugger->loopStartTime, 1, 1);
        ImGui::InputFloat("Loop end time", &debugger->loopEndTime, 1, 1);

        ImGui::InputFloat("Time scale", &debugger->timeScale, .1f, 1);
    }

    ImGui::Text("Frame Time Min: %f Max: %f", debugger->frameTimeMin, debugger->frameTimeMax);
    ImGui::PlotLines("", debugger->frameTimeBuffer, TIME_BUFFER_SIZE, editorTimeDebugger.debuggerOffset, "Frame time", debugger->frameTimeMin, debugger->frameTimeMax, ImVec2(width, 80));

    ImGui::Text("FPS Min: %f Max: %f", debugger->fpsMin, debugger->fpsMax);
    ImGui::PlotLines("", debugger->fpsBuffer, TIME_BUFFER_SIZE, editorTimeDebugger.debuggerOffset, "FPS", debugger->fpsMin, debugger->fpsMax, ImVec2(width, 80));

    ImGui::End();
}

static i32 ShaderSourceEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackResize: {
            if(data->BufTextLen >= editorShaderDebugger.currentFileBufferSize) {
                u32 newBufferSize = data->BufTextLen * 2;
                void* oldMemory = (void*)editorShaderDebugger.currentFileBuffer;
                size_t oldSize = editorShaderDebugger.currentFileBufferSize;
                editorShaderDebugger.currentFileBufferSize = newBufferSize;
                editorShaderDebugger.currentFileBuffer = (char*)malloc(editorShaderDebugger.currentFileBufferSize);
                data->Buf = editorShaderDebugger.currentFileBuffer;
                ZeroSize(editorShaderDebugger.currentFileBufferSize, editorShaderDebugger.currentFileBuffer);
                free(oldMemory);
            }

            break;
        }
    }
    return 0;
}

static void LoadTargetIDShader() {
    if(editorShaderDebugger.currentFileBuffer) {
        free(editorShaderDebugger.currentFileBuffer);
    }
    editorShaderDebugger.currentFileBuffer = 0;

    if(editorShaderDebugger.targetID >= 0) {
        i32 shaderLength = 0;
        glGetShaderiv(editorShaderDebugger.targetID, GL_SHADER_SOURCE_LENGTH, &shaderLength);
        editorShaderDebugger.currentFileBufferSize = shaderLength * 2;
        editorShaderDebugger.currentFileBuffer = (char*)malloc(editorShaderDebugger.currentFileBufferSize);

        i32 shaderSourceLength;
        u32 bufferSize = (u32)editorShaderDebugger.currentFileBufferSize;
        glGetShaderSource(editorShaderDebugger.targetID, bufferSize, &shaderSourceLength, editorShaderDebugger.currentFileBuffer);
    }
}

static void EditorDraw(ShaderDebuggerWindow* debugger) 
{
    if(!debugger->open) { return; }
    ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Shaders", &debugger->open)) {
        ImGui::End();
        return;
    }

    bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);

    if(debugger->programIndex == -1) {
        debugger->programIDChanged = true;
        debugger->programIndex = 0;
    }

    if(ImGui::InputInt("Program Index", &debugger->programIndex, 1, 1)) {
        if(debugger->programIndex < 0) {
            debugger->programIndex = 0;
        }

        if(debugger->currentFileBuffer) {
            free(debugger->currentFileBuffer);
        }
        debugger->currentFileBuffer = 0;

        debugger->programIDChanged = true;
    }

    WatchedProgram watched = watchedPrograms[debugger->programIndex];
    u32 programID = watched.shaderProgram;
    
    bool validProgram = GetProgramValid(programID);

    if(validProgram) {
        if(debugger->programIDChanged) {
            debugger->targetID = -1;
            debugger->vertexShaderID = -1;
            debugger->fragmentShaderID = -1;

            i32 shaderCount = 0;
            u32 shaders[2];
            glGetAttachedShaders(programID, 2, &shaderCount, shaders);

        ImGui::Text("Shaders:");
        for(i32 i = 0; i < shaderCount; ++i) {
            ImGui::Separator();

                i32 shaderType = 0;
                glGetShaderiv(shaders[i], GL_SHADER_TYPE, &shaderType);
                
                if(shaderType == GL_VERTEX_SHADER) {
                    debugger->vertexShaderID = shaders[i];
                    debugger->targetID = debugger->vertexShaderID;
                }
                else {
                    debugger->fragmentShaderID = shaders[i];
                    debugger->targetID = debugger->fragmentShaderID;
                }
            }
            
            LoadTargetIDShader();
        }

        if(debugger->currentFileBuffer != 0) {
            ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
            ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
            float contentHeight = contentMax.y - contentMin.y - 76;

            ImGui::Text("Shaders:");
            if(debugger->vertexShaderID >= 0) {
                ImGui::SameLine();
                if(ImGui::SmallButton("Vertex")) {
                    debugger->targetID = debugger->vertexShaderID;
                    LoadTargetIDShader();
                }
            }
            if(debugger->fragmentShaderID >= 0) {
                ImGui::SameLine();
                if(ImGui::SmallButton("Fragment")) {
                    debugger->targetID = debugger->fragmentShaderID;
                    LoadTargetIDShader();
                }
            }

            if(debugger->targetID == debugger->vertexShaderID) {
                ImGui::Text("File: %s", watched.vertexFilepath);
            }
            if(debugger->targetID == debugger->fragmentShaderID) {
                ImGui::Text("File: %s", watched.fragmentFilepath);
            }

            ImGui::InputTextMultiline("##source", debugger->currentFileBuffer, debugger->currentFileBufferSize, ImVec2(-FLT_MIN, contentHeight), ImGuiInputTextFlags_AllowTabInput|ImGuiInputTextFlags_CallbackResize, &ShaderSourceEditCallback);    
            
            ImGuiIO imguiIO = ImGui::GetIO();
            if(ImGui::Button("Reload") || (focused && imguiIO.KeyCtrl && ImGui::IsKeyPressed(SDL_SCANCODE_S))) {
                SOURCE_TYPE source = static_cast<SOURCE_TYPE>(debugger->currentFileBuffer);

                i32 size = (i32)debugger->currentFileBufferSize;
                glShaderSource(debugger->targetID, 1, &source, &size);
                glCompileShader(debugger->targetID);
                
                i32 success;
                char infoLog[INFO_LOG_BUFFER_SIZE];
                glGetShaderiv(debugger->targetID, GL_COMPILE_STATUS, &success);

                if (!success)
                {
                    glGetShaderInfoLog(debugger->targetID, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
                    if(debugger->targetID == debugger->vertexShaderID) {
                        LogError("ERROR::VERTEX::COMPILATION_FAILED %s\n", watched.vertexFilepath);
                    }
                    else if(debugger->targetID == debugger->fragmentShaderID) {
                        LogError("ERROR::FRAGMENT::COMPILATION_FAILED %s\n", watched.fragmentFilepath);
                    }
                    LogError("ERROR::COMPILATION_FAILED");
                    LogError(infoLog);
                }

                glLinkProgram(programID);

                glGetProgramiv(programID, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(programID, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
                    LogError("ERROR::PROGRAM::LINK_FAILED");
                    LogError(infoLog);
                }

#ifdef LUA_ENABLED
                RunLUAProtectedFunction(EditorShaderReload);
#endif
            }
        }
    } else {
        ImGui::Text("Program ID is not valid");
    }

    debugger->programIDChanged = false;

    ImGui::End();
}

#ifdef LUA_ENABLED
static i32 LuaSourceEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackEdit: {
            watchListEdited[editorLUADebugger.currentFileIndex] = true;

            break;
        }
        case ImGuiInputTextFlags_CallbackResize: {
            if(data->BufTextLen >= editorLUADebugger.currentFileBufferSize) {
                u32 newBufferSize = data->BufTextLen * 2;
                void* oldMemory = (void*)editorLUADebugger.currentFileBuffer;
                size_t oldSize = editorLUADebugger.currentFileBufferSize;
                editorLUADebugger.currentFileBufferSize = newBufferSize;
                editorLUADebugger.currentFileBuffer = (char*)malloc(editorLUADebugger.currentFileBufferSize);
                data->Buf = editorLUADebugger.currentFileBuffer;
                ZeroSize(editorLUADebugger.currentFileBufferSize, editorLUADebugger.currentFileBuffer);
                free(oldMemory);
            }

            break;
        }
    }
    return 0;
}

static void LoadCurrentLUAFile() {
    editorLUADebugger.currentFileBufferSize = GetFileSize(editorLUADebugger.currentFileName) * 2;
    if(editorLUADebugger.currentFileBuffer) { UnloadFileFromMemory(editorLUADebugger.currentFileBuffer); }
    editorLUADebugger.currentFileBuffer = (char*)LoadFileToMemory(editorLUADebugger.currentFileName, FILE_MODE_READ_BINARY, editorLUADebugger.currentFileBufferSize);
}

static void EditorDraw(LUADebuggerWindow* debugger)
{
    debugger->dockspaceID = ImGui::GetID("DebuggerDockspace");

    if(debugger->open) {
        ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("LUA", &debugger->open, ImGuiWindowFlags_MenuBar))
        {
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

                if(ImGui::BeginMenu("Window")) {
                    ImGui::Checkbox("Code editor", &debugger->codeOpen);
                    ImGui::Checkbox("Watch variables", &debugger->watchOpen);
                    ImGui::Checkbox("Stack", &debugger->stackOpen);

                    ImGui::EndMenu();
                }

#if PLATFORM_WINDOWS
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

                    ImGui::Separator();
                    if (ImGui::MenuItem("Break on function")) {
                        menuAction = DebugMenuAction_BREAK_ON_FUNCTION;
                    }
                    
                    ImGui::EndMenu();
                }
#endif

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

                        if(debugger->currentFileBuffer) {
                            UnloadFileFromMemory(debugger->currentFileBuffer);
                        }

                        strcpy(debugger->currentFileName, debugInfo.source + 1);

                        if(debugger->currentFileBuffer) { UnloadFileFromMemory(debugger->currentFileBuffer); }
                        debugger->currentFileBuffer = (char*)LoadFileToMemory(debugInfo.source + 1, FILE_MODE_READ_BINARY, &debugger->currentFileBufferSize);
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

            ImGui::DockSpace(debugger->dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode );
        }
        else {
            ImGui::PopStyleVar();
        }

        ImGui::End();
    }

    const char* luaCodeWindowName = "LUA Code";
    if(debugger->codeOpen) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowDockID(debugger->dockspaceID, ImGuiCond_FirstUseEver);
        if(ImGui::Begin(luaCodeWindowName, &debugger->codeOpen), ImGuiWindowFlags_DockNodeHost) {
            bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);

            char selectedFilename[LUA_FILENAME_MAX];
            i32 selectedFileIndex = debugger->currentFileIndex;
            if (ImGui::BeginTabBar("LoadedFiles", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll)) {
                i32 nameIndex = 0;
                i32 watchIndex = 0;
                i32 fileIndex = 0;
                char name[LUA_FILENAME_MAX];
                while(watchIndex < watchListSize) {
                    if(watchList[watchIndex] == '@') {
                        i32 nameSize = nameIndex;

                        name[nameIndex] = 0;

                        bool tabOpen = true;
                        ImGui::PushID(fileIndex);
                        if (ImGui::BeginTabItem(name, &tabOpen, ImGuiTabItemFlags_NoCloseButton))
                        {
                            if(tabOpen) {
                                selectedFileIndex = fileIndex;
                                strcpy(selectedFilename, name);
                                selectedFilename[nameSize] = 0;
                            }
                            ImGui::EndTabItem();
                        }
                        ImGui::PopID();

                        nameIndex = -1;
                        fileIndex++;
                    }
                    else {
                        name[nameIndex] = watchList[watchIndex];
                    }
                    nameIndex++;
                    watchIndex++;
                }
                ImGui::EndTabBar();
            }

            if(strcmp(selectedFilename, debugger->currentFileName)) {
                strcpy(debugger->currentFileName, selectedFilename);
                debugger->currentFileIndex = selectedFileIndex;
                
                LoadCurrentLUAFile();
            }
            ImGui::PopStyleVar();

            if(debugger->currentFileBuffer) {
                ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
                ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
                float contentHeight = contentMax.y - contentMin.y - 50;
                
                ImGui::InputTextMultiline("##source", debugger->currentFileBuffer, debugger->currentFileBufferSize, ImVec2(-FLT_MIN, contentHeight), ImGuiInputTextFlags_AllowTabInput|ImGuiInputTextFlags_CallbackResize|ImGuiInputTextFlags_CallbackEdit, &LuaSourceEditCallback);
                
                ImGuiIO imguiIO = ImGui::GetIO();
                if(ImGui::Button("Reload source")) {
                    LoadCurrentLUAFile();
                    LoadScriptString(debugger->currentFileBuffer);
                    ImGui::SetItemDefaultFocus();
                }
                ImGui::SameLine();
                if(ImGui::Button("Load") || (focused && imguiIO.KeyCtrl && ImGui::IsKeyPressed(SDL_SCANCODE_S))) {
                    LoadScriptString(debugger->currentFileBuffer);
                }

                if(watchListEdited[debugger->currentFileIndex]) {
                    ImGui::SameLine();
                    ImGui::Text("Edited");
                }
                // ImVec2 size = ImGui::GetWindowSize();
                // i32 line = 1;
                // i32 index = 0;
                // ImGui::BeginColumns("Text Editor", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
                // f32 lineSize = size.x * 0.1f;
                // ImGui::SetColumnWidth(0, lineSize);
                // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                // ImGui::Dummy(ImVec2(0, 3));
                // while(index <= debugger->currentFileBufferSize) {
                //     if(debugger->currentFileBuffer[index] == '\r') { ++index; }
                //     if(debugger->currentFileBuffer[index] == '\n' || debugger->currentFileBuffer[index] == 0) {
                //         ImGui::Text("%d         ", line);
                //         ++line;
                //     }
                //     ++index;
                // }

                // ImGui::NextColumn();

                // index = 0;
                // i32 lastIndex = 0;
                // ImGui::Dummy(ImVec2(0, 3));
                // while(index <= debugger->currentFileBufferSize) {
                //     if(debugger->currentFileBuffer[index] == '\r') { ++index; }
                //     if(debugger->currentFileBuffer[index] == '\n' || debugger->currentFileBuffer[index] == 0) {
                //         ImGui::TextUnformatted(debugger->currentFileBuffer + lastIndex, debugger->currentFileBuffer + index);
                //         lastIndex = index + 1;
                //         ++line;
                //     }
                //     ++index;
                // }
                // size.x -= lineSize;

                // ImGui::PopStyleVar();
                // ImGui::EndColumns();
            }
            else {
                ImGui::Text("No file loaded");
            }
        }

        ImGui::End();
    }

    const char* watchWindowName = "LUA Watch";
    if(debugger->watchOpen) {
        ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowDockID(debugger->dockspaceID, ImGuiCond_FirstUseEver);
        if(ImGui::Begin(watchWindowName, &debugger->watchOpen)) {
            char valueBuffer[64];
            for(i32 i = 0; i < WATCH_BUFFER_COUNT; ++i) {
                valueBuffer[0] = '\0';

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

                ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
                ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
                float contentWidth = contentMax.x - contentMin.x;

                ImGui::PushID(i);
                ImGui::PushItemWidth(contentWidth * 0.3f);
                char* watchSubBuffer = debugger->watchBuffer + i * WATCH_BUFFER_SIZE_EXT;
                ImGui::InputText("##Input", watchSubBuffer, WATCH_BUFFER_SIZE);
                ImGui::SameLine();
                ImGui::PushItemWidth(contentWidth * 0.2f - 2);
                if (ImGui::BeginCombo("##Combo", watchTypeNames[debugger->watchType[i]]))
                {
                    for (i32 n = 0; n < ArraySize(watchTypeNames); n++)
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
                char* watchName = debugger->watchBuffer + i * WATCH_BUFFER_SIZE_EXT;
                if(*watchName != 0) {
                    GetWatchValue(debugger->watchType[i], watchName, valueBuffer);
                }
                ImGui::PushItemWidth(contentWidth * 0.5f - 4);
                ImGui::TextUnformatted(valueBuffer);
                ImGui::PopID();

                ImGui::PopStyleVar();
            }
        }

        ImGui::End();
    }

    const char* stackWindowName = "LUA Stack";
    if(debugger->stackOpen) {
        ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowDockID(debugger->dockspaceID, ImGuiCond_FirstUseEver);
        if(ImGui::Begin(stackWindowName, &debugger->stackOpen)) {
            i32 top=lua_gettop(lua);
            for (i32 i = 1; i <= top; i++) {
                ImGui::Text("%d\t%s\t", i, luaL_typename(lua,i));
                switch (lua_type(lua, i)) {
                    case LUA_TNUMBER:
                    {
                        ImGui::Text("%g\n",lua_tonumber(lua,i));
                    } break;
                    case LUA_TSTRING:
                    {
                        ImGui::Text("%s\n",lua_tostring(lua,i));
                    } break;
                    case LUA_TBOOLEAN:
                    {
                        ImGui::Text("%s\n", (lua_toboolean(lua, i) ? "true" : "false"));
                    } break;
                    case LUA_TNIL:
                    {
                        ImGui::Text("%s\n", "nil");
                    } break;
                    default:
                    {
                        ImGui::Text("%p\n",lua_topointer(lua,i));
                    } break;
                }
            }
        }

        ImGui::End();
    }
}
#endif

static void EditorDraw(EditorConfigWindow* config)
{
    if(!config->open) { return; }
    
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowSize(ImVec2(500,300), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("Editor Config", &config->open, ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    i32 selectedPlatform = 0;
    
    ImGui::Text("General configuration");

    ImGuiInputDynamicText("Runtimes Path", config->runtimesPath);
    ImGuiInputDynamicText("Data Path", config->dataPath);

    ImGui::Separator();

    ImGui::Text("Platform configuration");

    if (ImGui::BeginTabBar("Platforms", ImGuiTabBarFlags_FittingPolicyScroll)) {
        i32 nameIndex = 0;
        while(nameIndex < ArraySize(platformNames)) {
            bool tabOpen = true;
            if (ImGui::BeginTabItem(platformNames[nameIndex], &tabOpen, ImGuiTabItemFlags_NoCloseButton))
            {
                if(tabOpen) {
                    selectedPlatform = nameIndex;
                }
                ImGui::EndTabItem();
            }
            nameIndex++;
        }
        ImGui::EndTabBar();
    }

    ImGui::PushID(selectedPlatform);

    if(selectedPlatform == RuntimePlatform_WINDOWS_86) {
        ImGuiInputDynamicText("Output Path", windows86OutputConfig.outputPath);
    }
    if(selectedPlatform == RuntimePlatform_WINDOWS_64) {
        ImGuiInputDynamicText("Output Path", windows64OutputConfig.outputPath);
    }
    if(selectedPlatform == RuntimePlatform_ANDROID) {
        ImGui::Text("Android platform is still WIP");
    }
    if(selectedPlatform == RuntimePlatform_WASM) {
        ImGuiInputDynamicText("Output Path", wasmOutputConfig.outputPath);
    }

    ImGui::PopID();

    ImGui::End();
}

static void EditorDraw(HelpWindow* help)
{
    if(!help->open) { return; }
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowSize(ImVec2(500,300), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("Help", &help->open, ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    ImGui::Text("Version: %d.%d.%d (%s)", ENVARI_MAYOR_VERSION, ENVARI_MINOR_VERSION, ENVARI_MICRO_VERSION, ENVARI_PLATFORM_NAME);
#ifdef LUA_ENABLED
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
    char loadNameBuffer[128];

    editorState->editorFrameRunning = TableGetBool(&editorSave, "editorFrameRunning", true);
    editorState->layoutInited = TableGetBool(&editorSave, "layoutInited", false);

    // editorConsole.open = TableGetBool(&editorSave, "editorConsoleOpen");
    editorConsole.open = true;
    EditorInit(&editorConsole);
    editorConsole.logFlags = TableGetI32(&editorSave, "editorLogFlags");
    
    for(i32 i = 0; i < CONSOLE_HISTORY_SAVE_SIZE; ++i) {
        sprintf(loadNameBuffer, "editorConsoleHistory%d", i);
        char* history = TableGetString(&editorSave, loadNameBuffer, 0);
        if(history != 0) {
            size_t commandLineSize = 0;
            editorConsole.history.push_back(Strdup(history, &commandLineSize));
        }
        else {
            break;
        }
    }

    editorPreview.open = TableGetBool(&editorSave, "editorPreviewOpen");
    EditorInit(&editorPreview);

    assetsWindow.open = TableGetBool(&editorSave, "assetsWindowOpen");
    EditorInit(&assetsWindow);

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

    editorShaderDebugger.open = TableGetBool(&editorSave, "editorShaderDebuggerOpen");
    EditorInit(&editorShaderDebugger);

#ifdef LUA_ENABLED
    editorLUADebugger.open = TableGetBool(&editorSave, "editorLUADebuggerOpen");
    EditorInit(&editorLUADebugger);
    editorLUADebugger.codeOpen = TableGetBool(&editorSave, "editorLUADebuggerCodeOpen");
    editorLUADebugger.watchOpen = TableGetBool(&editorSave, "editorLUADebuggerWatchOpen");
    editorLUADebugger.stackOpen = TableGetBool(&editorSave, "editorLUADebuggerStackOpen");
    
    for(i32 i = 0; i < WATCH_BUFFER_COUNT; ++i) {
        sprintf(loadNameBuffer, "editorLUADebuggerWatching%d", i);
        strcpy(editorLUADebugger.watchBuffer + i * WATCH_BUFFER_SIZE_EXT, TableGetString(&editorSave, loadNameBuffer));
    }
#endif

    editorHelp.open = TableGetBool(&editorSave, "editorHelpOpen");
    EditorInit(&editorHelp);

    editorConfig.open = TableGetBool(&editorSave, "editorConfigOpen");
    EditorInit(&editorConfig);

    *editorConfig.runtimesPath = TableGetString(&editorSave, "editorConfigRuntimesPath");
    *editorConfig.dataPath = TableGetString(&editorSave, "editorConfigDataPath");

    *windows86OutputConfig.outputPath = TableGetString(&editorSave, "windows86OutputConfigOutputPath");
    *windows64OutputConfig.outputPath = TableGetString(&editorSave, "windows64OutputConfigOutputPath");
    *androidOutputConfig.outputPath = TableGetString(&editorSave, "androidOutputConfigOutputPath");
    *wasmOutputConfig.outputPath = TableGetString(&editorSave, "wasmOutputOutputConfigOutputPath");

    editorConsole.logFlags = 0x7FFFFFFF;
}

static void EditorDrawAll()
{
    bool previewClosed = !editorPreview.open;
    if(previewClosed) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(.0f, .0f, .0f, .0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.06f, 0.06f, 0.94f));
    }

    editorState->dockspaceID = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    EditorDraw(&editorConsole);
    EditorDraw(&editorPreview);
    EditorDraw(&assetsWindow);
    EditorDraw(&assetsViewer);
    EditorDraw(&editorPerformanceDebugger);
    EditorDraw(&editorRenderDebugger);
    EditorDraw(&editorMemoryDebugger);
    EditorDraw(&editorTextureDebugger);
    EditorDraw(&editorInputDebugger);
    EditorDraw(&editorTimeDebugger);
    EditorDraw(&editorSoundDebugger);
    EditorDraw(&editorShaderDebugger);
#ifdef LUA_ENABLED
    EditorDraw(&editorLUADebugger);
#endif
    EditorDraw(&editorConfig);
    EditorDraw(&editorHelp);

    if(cppDemoVersion) {
        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowSize(ImVec2(500,300), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Demo", &cppDemoVersion)) {
            ImGui::End();
            return;
        }
        
        ImGui::SliderFloat("Radius", &radius, .1f, 50);
        ImGui::SliderFloat("Color offset", &colorOffset, 0, 1);
        ImGui::SliderFloat("Y speed", &ySpeed, 0, 10);
        ImGui::SliderFloat("Y offset", &yOffset, 0, 10);
        ImGui::SliderFloat("Per Line Offset", &perLineOffset, 0, 1);
        ImGui::InputFloat2("Distance", distance.e);
        ImGui::InputInt2("Count", count.e);
        ImGui::End();
    }

    if(editorState->demoWindow) {
        ImGui::ShowDemoWindow(&editorState->demoWindow);
    }

    if(previewClosed) {
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

static void EditorEnd()
{
    char saveKeyBuffer[128];
    
    TableSetBool(&temporalState->arena, &editorSave, "editorFrameRunning", editorState->editorFrameRunning);
    TableSetBool(&temporalState->arena, &editorSave, "layoutInited", editorState->layoutInited);

    TableSetBool(&temporalState->arena, &editorSave, "editorConsoleOpen", editorConsole.open);
    for(i32 i = 0; i < CONSOLE_HISTORY_SAVE_SIZE && i < editorConsole.history.Size; ++i) {
        sprintf(saveKeyBuffer, "editorConsoleHistory%d", i);
        TableSetString(&temporalState->arena, &editorSave, saveKeyBuffer, editorConsole.history[i]);
    }
    TableSetI32(&temporalState->arena, &editorSave, "editorLogFlags", editorConsole.logFlags);
    TableSetBool(&temporalState->arena, &editorSave, "editorPreviewOpen", editorPreview.open);
    TableSetBool(&temporalState->arena, &editorSave, "assetsWindowOpen", assetsWindow.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorPerformanceDebuggerOpen", editorPerformanceDebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorRenderDebuggerOpen", editorRenderDebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorMemoryDebuggerOpen", editorMemoryDebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorTextureDebuggerOpen", editorTextureDebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorInputDebuggerOpen", editorInputDebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorTimeDebuggerOpen", editorTimeDebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorSoundDebuggerOpen", editorSoundDebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorSoundDebuggersoundMuted", soundMuted);
    TableSetBool(&temporalState->arena, &editorSave, "editorShaderDebuggerOpen", editorShaderDebugger.open);
#ifdef LUA_ENABLED
    TableSetBool(&temporalState->arena, &editorSave, "editorLUADebuggerOpen", editorLUADebugger.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorLUADebuggerCodeOpen", editorLUADebugger.codeOpen);
    TableSetBool(&temporalState->arena, &editorSave, "editorLUADebuggerWatchOpen", editorLUADebugger.watchOpen);
    TableSetBool(&temporalState->arena, &editorSave, "editorLUADebuggerStackOpen", editorLUADebugger.stackOpen);
    for(i32 i = 0; i < WATCH_BUFFER_COUNT; ++i) {
        sprintf(saveKeyBuffer, "editorLUADebuggerWatching%d", i);
        TableSetString(&temporalState->arena, &editorSave, saveKeyBuffer, editorLUADebugger.watchBuffer + i * WATCH_BUFFER_SIZE_EXT);
    }
#endif
    TableSetBool(&temporalState->arena, &editorSave, "editorHelpOpen", editorHelp.open);
    TableSetBool(&temporalState->arena, &editorSave, "editorConfigOpen", editorConfig.open);
    TableSetString(&temporalState->arena, &editorSave, "editorConfigRuntimesPath", editorConfig.runtimesPath->value);
    TableSetString(&temporalState->arena, &editorSave, "editorConfigDataPath", editorConfig.dataPath->value);

    TableSetString(&temporalState->arena, &editorSave, "windows86OutputConfigOutputPath", windows86OutputConfig.outputPath->value);
    TableSetString(&temporalState->arena, &editorSave, "windows64OutputConfigOutputPath", windows64OutputConfig.outputPath->value);
    TableSetString(&temporalState->arena, &editorSave, "androidOutputConfigOutputPath", androidOutputConfig.outputPath->value);
    TableSetString(&temporalState->arena, &editorSave, "wasmOutputOutputConfigOutputPath", wasmOutputConfig.outputPath->value);

    SerializeTable(&editorSave, EDITOR_SAVE_PATH);

#ifdef PLATFORM_WASM
    main_save();
#endif
}

#endif