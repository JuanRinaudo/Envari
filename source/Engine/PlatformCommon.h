#ifndef PLATFORM_COMMON_H
#define PLATFORM_COMMON_H

#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#endif

SDL_Window* sdlWindow;
SDL_DisplayMode displayMode;
SDL_GLContext glContext;

std::chrono::steady_clock::time_point timeStart;
std::chrono::steady_clock::time_point timeEnd;

bool mouseOverWindow = true;
bool mouseEnabled = true;
bool keyboardEnabled = true;

char saveNameBuffer[ArrayCount(DATA_SAVE_PATH) + 4] = "";

static void CheckInput() {
    ZeroSize(TEXT_INPUT_EVENT_SIZE, gameState->input.textInputEvent);

    for(i32 key = 0; key < KEY_COUNT; ++key) {
        u8 keyState = gameState->input.keyState[key];
        if(keyState == KEY_PRESSED) { gameState->input.keyState[key] = KEY_DOWN; }
        else if(keyState == KEY_RELEASED) { gameState->input.keyState[key] = KEY_UP; }
    }

    if(gameState->input.anyReasonableKeyState == KEY_PRESSED) { gameState->input.anyReasonableKeyState = KEY_DOWN; }
    else if(gameState->input.anyReasonableKeyState == KEY_RELEASED) { gameState->input.anyReasonableKeyState = KEY_UP; }
    
    if(gameState->input.anyKeyState == KEY_PRESSED) { gameState->input.anyKeyState = KEY_DOWN; }
    else if(gameState->input.anyKeyState == KEY_RELEASED) { gameState->input.anyKeyState = KEY_UP; }

    for(i32 key = 0; key < MOUSE_COUNT; ++key) {
        u8 mouseState = gameState->input.mouseState[key];
        if(mouseState == KEY_PRESSED) { gameState->input.mouseState[key] = KEY_DOWN; }
        else if(mouseState == KEY_RELEASED) { gameState->input.mouseState[key] = KEY_UP; }
    }

    if(gameState->input.anyMouseState == KEY_PRESSED) { gameState->input.anyMouseState = KEY_DOWN; }
    else if(gameState->input.anyMouseState == KEY_RELEASED) { gameState->input.anyMouseState = KEY_UP; }

    gameState->input.mouseWheel = 0;
    gameState->input.mouseScreenDeltaPosition.x = 0;
    gameState->input.mouseScreenDeltaPosition.y = 0;
    gameState->input.mouseDeltaPosition.x = 0;
    gameState->input.mouseDeltaPosition.y = 0;
}

static void TryCreateDataFolderStructure(std::string workingDirectoryPath)
{
    std::string configPath = workingDirectoryPath + "/config";
    std::string fontsPath = workingDirectoryPath + "/fonts";
    std::string imagesPath = workingDirectoryPath + "/images";
    std::string atlasPath = workingDirectoryPath + "/atlas";
    std::string scriptsPath = workingDirectoryPath + "/scripts";
    std::string shadersPath = workingDirectoryPath + "/shaders";
    std::string soundPath = workingDirectoryPath + "/sound";
    std::string videoPath = workingDirectoryPath + "/video";

    CreateDirectoryIfNotExists(workingDirectoryPath.c_str());
    CreateDirectoryIfNotExists(configPath.c_str());
    CreateDirectoryIfNotExists(fontsPath.c_str());
    CreateDirectoryIfNotExists(imagesPath.c_str());
    CreateDirectoryIfNotExists(atlasPath.c_str());
    CreateDirectoryIfNotExists(scriptsPath.c_str());
    CreateDirectoryIfNotExists(shadersPath.c_str());
    CreateDirectoryIfNotExists(soundPath.c_str());
    CreateDirectoryIfNotExists(videoPath.c_str());
}

static i32 SetupEnviroment()
{
    filesystem::path dataPath = filesystem::path(filesystem::current_path().string() + "/data");
	if(!filesystem::exists(dataPath)) {
		filesystem::create_directories(dataPath);
    }
    filesystem::current_path(dataPath);
    TryCreateDataFolderStructure(dataPath);

    CreateDirectoryIfNotExists("temp");
    CreateDirectoryIfNotExists("save");

    return 1;
}

static i32 InitEngine()
{
    gameState->sound.bindingsEnabled = TableGetBool(&initialConfig, "soundBindingsEnabled", true);

    for(int i = 0; i < MOUSE_COUNT; ++i) {
        gameState->input.mouseState[i] = KEY_UP;
    }
    for(int i = 0; i < KEY_COUNT; ++i) {
        gameState->input.keyState[i] = KEY_UP;
    }

    return 1;
}

static i32 InitSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        return -1;
    }
    
    #if defined(PLATFORM_WASM) | defined(PLATFORM_ANDROID)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #endif

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GetCurrentDisplayMode(0, &displayMode);
    
    return 1;
}

static void SetupRenderSize()
{
    switch(gameState->render.framebufferAdjustStyle) {
        case TextureAdjustStyle_Stretch: {
            gameState->render.size.x = gameState->render.windowSize.x;
            gameState->render.size.y = gameState->render.windowSize.y;
            break;
        }
        case TextureAdjustStyle_FitRatio: {
            
            break;
        }
        case TextureAdjustStyle_KeepRatioX: {

            break;
        }
        case TextureAdjustStyle_KeepRatioY: {
            f32 bufferRatio = gameState->render.bufferSize.x / gameState->render.bufferSize.y;
            gameState->render.size.x = gameState->render.windowSize.y * bufferRatio;
            gameState->render.size.y = gameState->render.windowSize.y;
            break;
        }
    }
}

static i32 SetupWindow()
{
    DeserializeTable(&permanentState->arena, &configSave, CONFIG_SAVE_PATH);
    gameState->render.windowPosition = TableGetV2(&configSave, "windowPosition", V2(-1, -1));
    gameState->render.windowSize = TableGetV2(&configSave, "windowSize", TableGetV2(&initialConfig, "windowSize", V2(320, 320)));

    gameState->render.framebufferEnabled = TableHasKey(initialConfig, "bufferSize");
    if(gameState->render.framebufferEnabled) {
        v2 bufferSize = TableGetV2(&initialConfig, "bufferSize");
        gameState->render.renderScale = TableGetFloat(&initialConfig, "renderScale");
        if(bufferSize.x <= 32 && bufferSize.y <= 32) {
            gameState->render.bufferSize.x = gameState->render.windowSize.x * bufferSize.x;
            gameState->render.bufferSize.y = gameState->render.windowSize.y * bufferSize.y;
        }
        else {
            gameState->render.bufferSize.x = bufferSize.x;
            gameState->render.bufferSize.y = bufferSize.y;
        }
        gameState->render.scaledBufferSize.x = gameState->render.bufferSize.x * gameState->render.renderScale;
        gameState->render.scaledBufferSize.y = gameState->render.bufferSize.y * gameState->render.renderScale;
    }
    else {
        gameState->render.bufferSize.x = -1;
        gameState->render.bufferSize.y = -1;
        gameState->render.scaledBufferSize.x = -1;
        gameState->render.scaledBufferSize.y = -1;
    }

    gameState->render.refreshRate = displayMode.refresh_rate;
    gameState->render.framebufferAdjustStyle = TextureAdjustStyle_KeepRatioY;

    SetupRenderSize();

    char* windowTitle = TableGetString(&initialConfig, "windowTitle");
    sdlWindow = SDL_CreateWindow(windowTitle, gameState->render.windowPosition.x > 0 ? (i32)gameState->render.windowPosition.x : SDL_WINDOWPOS_UNDEFINED,
        gameState->render.windowPosition.y > 0 ? (i32)gameState->render.windowPosition.y : SDL_WINDOWPOS_UNDEFINED, 
        (i32)gameState->render.windowSize.x, (i32)gameState->render.windowSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!sdlWindow) {
        return -1;
    }

    glContext = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_MakeCurrent(sdlWindow, glContext);
    if (!glContext) {
        return -1;
    }

    return 1;
}

static i32 CreateFramebuffer()
{
    if(gameState->render.framebufferEnabled) {
        CreateFramebufferGL((i32)gameState->render.scaledBufferSize.x, (i32)gameState->render.scaledBufferSize.y);
    }
    else {
        gameState->render.frameBuffer = 0;
    }

    return 1;
}

static i32 SetupTime()
{
    gameState->time.fpsLimit = TableGetInt(&initialConfig, "fpsLimit", -1);
    gameState->time.fpsFixed = TableGetInt(&initialConfig, "fpsFixed", -1);
    gameState->time.fpsDelta = 1000.0f / gameState->time.fpsLimit;
    gameState->render.vsync = TableGetBool(&initialConfig, "vsync", false);
#ifndef __EMSCRIPTEN__
    // #TODO (Juan): Check why this brings problems with emcripten even if it is disabled
    SDL_GL_SetSwapInterval(gameState->render.vsync);
#endif

    gameState->game.updateRunning = true;
    gameState->time.gameFrames = -1;
    gameState->time.frames = -1;

    timeStart = std::chrono::steady_clock::now();

    return 1;
}

static i32 TimeTick()
{
    f32 startTime = 0;
    if(gameState->time.fpsFixed > 0)
    {
        gameState->time.deltaTime = 1.0f / gameState->time.fpsFixed;
        gameState->time.lastFrameGameTime += gameState->time.deltaTime;
    }
    else
    {
        startTime = SDL_GetTicks() / 1000.0f;
        gameState->time.deltaTime = startTime - gameState->time.realLastFrameGameTime;
        gameState->time.realLastFrameGameTime = startTime;
#if UNITY_EDITOR
        gameState->time.deltaTime *= editorTimeDebugger.timeScale;
#endif
        gameState->time.lastFrameGameTime = gameState->time.lastFrameGameTime + gameState->time.deltaTime;
    }

    gameState->time.frames++;

    // #NOTE(Juan): Do a fps limit if enabled
    if(gameState->time.fpsLimit > 0) {
        auto timeNow = std::chrono::steady_clock::now();
        i64 epochTime = timeNow.time_since_epoch().count();
        i64 deltaFPSNanoseconds = (u32)(gameState->time.fpsDelta * 1000000);
        timeEnd = timeNow + std::chrono::nanoseconds(deltaFPSNanoseconds - epochTime % deltaFPSNanoseconds);

        auto diff = timeNow - timeStart;
        if(diff >= std::chrono::seconds(1))
        {
            timeStart = timeNow;
        }
    }

    if(gameState->game.updateRunning) {
        gameState->time.gameTime += gameState->time.deltaTime;
        gameState->time.gameFrames++;
    }

#if PLATFORM_EDITOR
    if(editorTimeDebugger.timeloop) {
        if(editorTimeDebugger.loopFormat == TimeFormat_FRAMES) {
            if(gameState->time.gameFrames > editorTimeDebugger.loopEndFrame) {
                gameState->time.gameFrames = editorTimeDebugger.loopStartFrame;
                gameState->time.lastFrameGameTime = (gameState->time.fpsDelta / 1000) * (gameState->time.gameFrames - 1);
                gameState->time.gameTime = gameState->time.fpsDelta * gameState->time.gameFrames;
            }
        }
        else if(editorTimeDebugger.loopFormat == TimeFormat_TIME) {
            if(gameState->time.gameTime > editorTimeDebugger.loopEndTime) {
                gameState->time.gameTime = editorTimeDebugger.loopStartTime;
                gameState->time.lastFrameGameTime = gameState->time.gameTime - (gameState->time.fpsDelta / 1000);
                gameState->time.gameFrames = FloorToInt(gameState->time.gameTime / (gameState->time.fpsDelta / 1000));
            }
        }
    }
#endif

    return 1;
}

void EngineUpdate()
{
    if(gameState->sound.bindingsEnabled) {
        if(gameState->input.keyState[SDL_SCANCODE_KP_MINUS] == KEY_PRESSED) {
            masterVolumeModifier = Clamp(masterVolumeModifier - 0.1f, 0.0f, 1.0f);
        }
        if(gameState->input.keyState[SDL_SCANCODE_KP_PLUS] == KEY_PRESSED) {
            masterVolumeModifier = Clamp(masterVolumeModifier + 0.1f, 0.0f, 1.0f);
        }
    }

    GameUpdate();
}

void RuntimeQuit()
{
    gameState->game.running = false;
}

static i32 ProcessEvent(const SDL_Event* event)
{
    switch (event->type) {
        case SDL_QUIT: {
            RuntimeQuit();
            break;
        }
        case SDL_WINDOWEVENT: { // #NOTE (Juan): Window resize/orientation change
            switch(event->window.event) {
                case SDL_WINDOWEVENT_MOVED: {
                    gameState->render.windowPosition.x = (f32)event->window.data1;
                    gameState->render.windowPosition.y = (f32)event->window.data2;
                    break;
                }
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    gameState->render.windowSize.x = (f32)event->window.data1;
                    gameState->render.windowSize.y = (f32)event->window.data2;

                    SetupRenderSize();
                    break;
                }
                case SDL_WINDOWEVENT_FOCUS_GAINED: {
                    gameState->game.hasFocus = true;
#if LUA_ENABLED
                    RunLUAProtectedFunction(FocusChange);
#endif
                    break;
                }
                case SDL_WINDOWEVENT_FOCUS_LOST: {
                    gameState->game.hasFocus = false;
#if LUA_ENABLED
                    RunLUAProtectedFunction(FocusChange);
#endif
                    break;
                }
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if((mouseEnabled || mouseOverWindow) && gameState->input.mouseState[event->button.button] <= KEY_RELEASED) {
                gameState->input.mouseState[event->button.button] = KEY_PRESSED;
                gameState->input.anyMouseState = KEY_PRESSED;
            }
           break;
        }
        case SDL_MOUSEBUTTONUP: {
            if((mouseEnabled || mouseOverWindow) && gameState->input.mouseState[event->button.button] >= KEY_PRESSED) {
                gameState->input.mouseState[event->button.button] = KEY_RELEASED;
                gameState->input.anyMouseState = KEY_RELEASED;
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            if(mouseEnabled) {
                v2 oldMousePosition = ScreenToBuffer(gameState->input.mouseScreenPosition.x, gameState->input.mouseScreenPosition.y);

                gameState->input.mouseScreenDeltaPosition.x = (f32)event->button.x - gameState->input.mouseScreenPosition.x;
                gameState->input.mouseScreenDeltaPosition.y = (f32)event->button.y - gameState->input.mouseScreenPosition.y;
                gameState->input.mouseScreenPosition.x = (f32)event->button.x;
                gameState->input.mouseScreenPosition.y = (f32)event->button.y;

                gameState->input.mousePosition = ScreenToBuffer(gameState->input.mouseScreenPosition.x, gameState->input.mouseScreenPosition.y);
                gameState->input.mouseDeltaPosition.x = gameState->input.mousePosition.x - oldMousePosition.x;
                gameState->input.mouseDeltaPosition.y = gameState->input.mousePosition.y - oldMousePosition.y;
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            if(mouseEnabled || mouseOverWindow) {
                gameState->input.mouseWheel += event->wheel.y;
            }
            break;
        }
        case SDL_KEYDOWN: {
            if(keyboardEnabled && gameState->input.keyState[event->key.keysym.scancode] <= KEY_RELEASED) {
                gameState->input.keyState[event->key.keysym.scancode] = KEY_PRESSED;
                if((event->key.keysym.scancode >= SDL_SCANCODE_A && event->key.keysym.scancode <= SDL_SCANCODE_F12) ||
                   (event->key.keysym.scancode >= SDL_SCANCODE_LCTRL && event->key.keysym.scancode <= SDL_SCANCODE_RGUI)) {
                    gameState->input.anyReasonableKeyState = KEY_PRESSED;
                }
                gameState->input.anyKeyState = KEY_PRESSED;
            }
            break;
        }
        case SDL_KEYUP: {
            if(keyboardEnabled && gameState->input.keyState[event->key.keysym.scancode] >= KEY_PRESSED) {
                gameState->input.keyState[event->key.keysym.scancode] = KEY_RELEASED;
                if((event->key.keysym.scancode >= SDL_SCANCODE_A && event->key.keysym.scancode <= SDL_SCANCODE_SLASH) ||
                   (event->key.keysym.scancode >= SDL_SCANCODE_LCTRL && event->key.keysym.scancode <= SDL_SCANCODE_RGUI)) {
                    gameState->input.anyReasonableKeyState = KEY_RELEASED;
                }
                gameState->input.anyKeyState = KEY_RELEASED;
            }
            break;
        }
        case SDL_TEXTINPUT: {
            if(keyboardEnabled) {
                strcpy((char*)gameState->input.textInputEvent, event->text.text);
            }
        }
        default: {
            return 0;       
        }
    }

    return 1;
}

static rectangle2 GetTextureAdjustValues(TextureAdjustStyle style, v2 from, v2 to)
{
    f32 ratioX = to.x / from.x;
    f32 ratioY = to.y / from.y;

    switch(gameState->render.framebufferAdjustStyle) {
        case TextureAdjustStyle_Stretch: {
            f32 toRatio = to.x / to.y;
            return Rectangle2(-(toRatio - 1.0f) / 2.0f, 0, toRatio, 1);
        }
        case TextureAdjustStyle_FitRatio: {
            f32 toRatio = to.x / to.y;
            if(toRatio < 1) {
                return Rectangle2(0, -(toRatio - 1.0f) / 2.0f, toRatio, toRatio);
            }
            else {
                f32 fromRatio = from.x / from.y;
                return Rectangle2((toRatio - fromRatio) / 2.0f, 0, fromRatio, 1);
            }
        }
        case TextureAdjustStyle_KeepRatioX: {
            f32 toRatio = to.x / to.y;
            return Rectangle2(0, -(toRatio - 1.0f) / 2.0f, toRatio, toRatio);
        }
        case TextureAdjustStyle_KeepRatioY: {
            f32 fromRatio = from.x / from.y;
            return Rectangle2(-(fromRatio - 1.0f) / 2.0f, 0, fromRatio, 1);
        }
    }

    return Rectangle2(0, 0, 1, 1);
}

static i32 RenderFramebuffer()
{
    rectangle2 adjustment = GetTextureAdjustValues(gameState->render.framebufferAdjustStyle, gameState->render.bufferSize, gameState->render.windowSize);

    // #NOTE (Juan): Render framebuffer to actual screen buffer, save data and then restore it
    f32 tempSize = gameState->camera.size;
    f32 tempRatio = gameState->camera.ratio;
    m44 tempView = gameState->camera.view;
    m44 tempProjection = gameState->camera.projection;

    gameState->camera.size = 1;
    gameState->camera.ratio = (f32)gameState->render.windowSize.x / (f32)gameState->render.windowSize.y;
    gameState->camera.view = IdM44();
    gameState->camera.projection = OrtographicProjection(gameState->camera.size, gameState->camera.ratio, gameState->camera.nearPlane, gameState->camera.farPlane);

    Begin2D(0, (u32)gameState->render.windowSize.x, (u32)gameState->render.windowSize.y);
    DrawOverrideVertices(0, 0);
    DrawClear(0, 0, 0, 1);
    DrawTextureParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, FRAMEBUFFER_DEFAULT_FILTER, FRAMEBUFFER_DEFAULT_FILTER);
    DrawTexture(adjustment.x, adjustment.y + adjustment.height, adjustment.width, -adjustment.height, gameState->render.renderBuffer);
    RenderPass();
    End2D();

    gameState->camera.size = tempSize;
    gameState->camera.ratio = tempRatio;
    gameState->camera.view = tempView;
    gameState->camera.projection = tempProjection;

    return 1;
}

// #TODO (Juan): Maybe change this and refactor it for a real render size variable instead
static v2 GetRenderSize()
{
    if(gameState->render.framebufferEnabled) {
        return gameState->render.bufferSize;
    }
    else {
        return gameState->render.size;
    }
}

static void CommonBegin2D()
{
    if(gameState->render.framebufferEnabled) {
        Begin2D(gameState->render.frameBuffer, (u32)gameState->render.scaledBufferSize.x, (u32)gameState->render.scaledBufferSize.y);
    }
    else {
        Begin2D(0, (u32)gameState->render.size.x, (u32)gameState->render.size.y);
    }
}

static void CommonShowCursor()
{
    v2 renderSize = GetRenderSize();

    SDL_ShowCursor(gameState->input.mouseTextureID == 0 ||
        gameState->input.mousePosition.x < 0 || gameState->input.mousePosition.x > renderSize.x ||
        gameState->input.mousePosition.y < 0 || gameState->input.mousePosition.y > renderSize.y);
}

static i32 WaitFPSLimit()
{
    if(gameState->time.fpsLimit > 0) {
        std::this_thread::sleep_until(timeEnd);
    }

    return 1;
}

char* GetSavePath()
{
    sprintf(saveNameBuffer, DATA_SAVE_PATH, gameState->game.saveSlotID);
    return saveNameBuffer;
}

void SaveData()
{
    SerializeTable(&saveData, GetSavePath());
#ifdef PLATFORM_EDITOR
    SerializeTable(&editorSave, EDITOR_SAVE_PATH);
#endif
#ifdef PLATFORM_WASM
    main_save();
#endif
}

static i32 SaveConfig()
{
    TableSetV2(&permanentState->arena, &configSave, "windowPosition", gameState->render.windowPosition);
    TableSetV2(&permanentState->arena, &configSave, "windowSize", gameState->render.windowSize);
    SerializeTable(&configSave, CONFIG_SAVE_PATH);

    return 1;
}

#endif