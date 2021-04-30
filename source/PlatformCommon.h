#ifndef PLATFORM_COMMON_H
#define PLATFORM_COMMON_H

SDL_Window* sdlWindow;
SDL_DisplayMode displayMode;
SDL_GLContext glContext;

i32 fpsLimit;
i32 fpsFixed;
f32 fpsDelta;
i32 vsync;

std::chrono::steady_clock::time_point timeStart;
std::chrono::steady_clock::time_point timeEnd;

bool mouseOverWindow = true;
bool mouseEnabled = true;
bool keyboardEnabled = true;

static void CheckInput() {
    for(i32 key = 0; key < KEY_COUNT; ++key) {
        u8 keyState = gameState->input.keyState[key];
        if(keyState == KEY_PRESSED) { gameState->input.keyState[key] = KEY_DOWN; }
        else if(keyState == KEY_RELEASED) { gameState->input.keyState[key] = KEY_UP; }
    }

    for(i32 key = 0; key < MOUSE_COUNT; ++key) {
        u8 mouseState = gameState->input.mouseState[key];
        if(mouseState == KEY_PRESSED) { gameState->input.mouseState[key] = KEY_DOWN; }
        else if(mouseState == KEY_RELEASED) { gameState->input.mouseState[key] = KEY_UP; }
    }

    gameState->input.mouseWheel = 0;
}

static i32 InitSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    
    #ifdef PLATFORM_WASM
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    #else
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    #endif

    SDL_GetCurrentDisplayMode(0, &displayMode);
    
    return 1;
}

static i32 SetupWindow()
{
    DeserializeTable(&permanentState->arena, &configSave, "config.save");
    gameState->render.windowPosition = TableGetV2(&configSave, "windowPosition", V2(-1, -1));
    gameState->render.windowSize = TableGetV2(&configSave, "windowSize", TableGetV2(&initialConfig, "windowSize"));

    if(gameState->render.windowSize.x <= 32 && gameState->render.windowSize.y <= 32) {
        gameState->render.size.x = displayMode.w * gameState->render.windowSize.x;
        gameState->render.size.y = displayMode.h * gameState->render.windowSize.y;
    }
    else {
        gameState->render.size.x = gameState->render.windowSize.x;
        gameState->render.size.y = gameState->render.windowSize.y;
    }
    gameState->render.windowSize.x = gameState->render.size.x;
    gameState->render.windowSize.y = gameState->render.size.y;

    gameState->render.framebufferEnabled = TableHasKey(initialConfig, "bufferSize");
    if(gameState->render.framebufferEnabled) {
        v2 bufferSize = TableGetV2(&initialConfig, "bufferSize");
        if(bufferSize.x <= 32 && bufferSize.y <= 32) {
            gameState->render.bufferSize.x = gameState->render.windowSize.x * bufferSize.x;
            gameState->render.bufferSize.y = gameState->render.windowSize.y * bufferSize.y;
        }
        else {
            gameState->render.bufferSize.x = bufferSize.x;
            gameState->render.bufferSize.y = bufferSize.y;
        }
    }
    else {
        gameState->render.bufferSize.x = -1;
        gameState->render.bufferSize.y = -1;
    }

    gameState->render.refreshRate = displayMode.refresh_rate;

    char* windowTitle = TableGetString(&initialConfig, "windowTitle");
    sdlWindow = SDL_CreateWindow(windowTitle, gameState->render.windowPosition.x > 0 ? (i32)gameState->render.windowPosition.x : SDL_WINDOWPOS_UNDEFINED,
        gameState->render.windowPosition.y > 0 ? (i32)gameState->render.windowPosition.y : SDL_WINDOWPOS_UNDEFINED, 
        (i32)gameState->render.windowSize.x, (i32)gameState->render.windowSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!sdlWindow) {
        return -1;
    }

    glContext = SDL_GL_CreateContext(sdlWindow);
    if (!glContext) {
        return -1;
    }

    return 1;
}

static i32 CreateFramebuffer()
{
    if(gameState->render.framebufferEnabled) {
        GL_InitFramebuffer((i32)gameState->render.bufferSize.x, (i32)gameState->render.bufferSize.y);
    }
    else {
        gameState->render.frameBuffer = 0;
    }

    return 1;
}

static i32 SetupTime()
{
    fpsLimit = TableGetInt(&initialConfig, "fpsLimit");
    fpsFixed = TableGetInt(&initialConfig, "fpsFixed");
    fpsDelta = 1000.0f / fpsLimit;
    vsync = TableGetInt(&initialConfig, "vsync");
#ifndef __EMSCRIPTEN__
    // #TODO (Juan): Check why this brings problems with emcripten even if it is disabled
    SDL_GL_SetSwapInterval(vsync);
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
    if(fpsFixed > 0)
    {
        gameState->time.deltaTime = 1.0f / fpsFixed;
        gameState->time.startTime += gameState->time.deltaTime;
        startTime = gameState->time.startTime;
    }
    else
    {
        startTime = SDL_GetTicks() / 1000.0f;
        gameState->time.startTime = startTime;
        gameState->time.deltaTime = startTime - gameState->time.lastFrameGameTime;
    }

    gameState->time.frames++;
    gameState->time.lastFrameGameTime = startTime;

    // #NOTE(Juan): Do a fps limit if enabled
    if(fpsLimit > 0) {
        auto timeNow = std::chrono::steady_clock::now();
        i64 epochTime = timeNow.time_since_epoch().count();
        i64 deltaFPSNanoseconds = (u32)(fpsDelta * 1000000);
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

    return 1;
}

static i32 ProcessEvent(const SDL_Event* event)
{
    switch (event->type) {
        case SDL_QUIT: {
            gameState->game.running = false;
            break;
        }
        case SDL_WINDOWEVENT: // #NOTE (Juan): Window resize/orientation change
        {
            if(event->window.event == SDL_WINDOWEVENT_MOVED) {
                gameState->render.windowPosition.x = (f32)event->window.data1;
                gameState->render.windowPosition.y = (f32)event->window.data2;
            }
            else if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                gameState->render.windowSize.x = (f32)event->window.data1;
                gameState->render.windowSize.y = (f32)event->window.data2;
                gameState->render.size.x = gameState->render.windowSize.x;
                gameState->render.size.y = gameState->render.windowSize.y;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if((mouseEnabled || mouseOverWindow) && gameState->input.mouseState[event->button.button] <= KEY_RELEASED) {
                gameState->input.mouseState[event->button.button] = KEY_PRESSED;
            }
           break;
        }
        case SDL_MOUSEBUTTONUP: {
            if((mouseEnabled || mouseOverWindow) && gameState->input.mouseState[event->button.button] >= KEY_PRESSED) {
                gameState->input.mouseState[event->button.button] = KEY_RELEASED;
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            if(mouseEnabled) {
                gameState->input.mouseScreenPosition.x = (f32)event->button.x;
                gameState->input.mouseScreenPosition.y = (f32)event->button.y;

                gameState->input.mousePosition = RenderToViewport(gameState->input.mouseScreenPosition.x, gameState->input.mouseScreenPosition.y, gameState->camera.size, gameState->camera.ratio);
            }
            break;
        }
        case SDL_MOUSEWHEEL:
            if(mouseEnabled || mouseOverWindow) {
                gameState->input.mouseWheel += event->wheel.y;
            }
            break;
        case SDL_KEYDOWN: {
            if(keyboardEnabled && gameState->input.keyState[event->key.keysym.scancode] <= KEY_RELEASED) {
                gameState->input.keyState[event->key.keysym.scancode] = KEY_PRESSED;
            }
            break;
        }
        case SDL_KEYUP: {
            if(keyboardEnabled && gameState->input.keyState[event->key.keysym.scancode] >= KEY_PRESSED) {
                gameState->input.keyState[event->key.keysym.scancode] = KEY_RELEASED;
            }
            break;
        }
        default:
            return 0;       
    }

    return 1;
}

static i32 RenderFramebuffer()
{
    // #NOTE (Juan): Render framebuffer to actual screen buffer, save data and then restore it
    f32 tempSize = gameState->camera.size;
    f32 tempRatio = gameState->camera.ratio;
    m44 tempView = gameState->camera.view;
    m44 tempProjection = gameState->camera.projection;

    gameState->camera.size = 1;
    gameState->camera.ratio = (f32)gameState->render.size.x / (f32)gameState->render.size.y;
    gameState->camera.view = IdM44();
    gameState->camera.projection = OrtographicProjection(gameState->camera.size, gameState->camera.ratio, gameState->camera.nearPlane, gameState->camera.farPlane);

    Begin2D(0, (u32)gameState->render.windowSize.x, (u32)gameState->render.windowSize.y);
    DrawOverrideVertices(0, 0);
    DrawClear(0, 0, 0, 1);
    DrawTextureParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, FRAMEBUFFER_DEFAULT_FILTER, FRAMEBUFFER_DEFAULT_FILTER);
    f32 sizeX = tempRatio;
    f32 xOffset = -(gameState->render.windowSize.x / (gameState->render.bufferSize.x * (gameState->render.windowSize.y / gameState->render.bufferSize.y))) * 0.5f;
    // #TODO (Juan): Fix this texture offset
    DrawTexture(0, gameState->camera.size, sizeX, -gameState->camera.size, gameState->render.renderBuffer);
    GL_Render();
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
        Begin2D(gameState->render.frameBuffer, (u32)gameState->render.bufferSize.x, (u32)gameState->render.bufferSize.y);
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
    if(fpsLimit > 0) {
        std::this_thread::sleep_until(timeEnd);
    }

    return 1;
}

static i32 SaveConfig()
{
    TableSetV2(&permanentState->arena, &configSave, "windowPosition", gameState->render.windowPosition);
    TableSetV2(&permanentState->arena, &configSave, "windowSize", gameState->render.windowSize);
    SerializeTable(&configSave, "config.save");

    return 1;
}

#endif