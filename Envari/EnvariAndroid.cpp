#include "SDL.h"
#include "IMGUI/imgui.h"

#define GL_PROFILE_GLES3

#include "Defines.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Game.h"

#include <GLES3/gl3.h>
#include "IMGUI/imgui_impl_sdl_es3.h"

int main(int argc, char *argv[])
{

    gameMemory = malloc(Megabytes(128));

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    SDL_DisplayMode DisplayMode;
    SDL_GetCurrentDisplayMode(0, &DisplayMode);
    global.screen.width = DisplayMode.w;
    global.screen.height = DisplayMode.h;

    SDL_Window *Window = SDL_CreateWindow("Demo App", 0, 0, global.screen.width, global.screen.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext Context = SDL_GL_CreateContext(Window);
    SDL_GL_SetSwapInterval(1); // NOTE(Juan): 0 immediate update, 1 vsync, -1 adaptative sync

    // NOTE(Juan): Dear IMGUI
    IMGUI_CHECKVERSION();
    ImGuiContext *imguiContext = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(2);

    ImGui_ImplSdlGLES3_Init(Window);
    
    GameInit();

    GLCompileProgram(vertexShaderSource, fragmentShaderSource);

    while (!Running) {

        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            ImGui_ImplSdlGLES3_ProcessEvent(&Event);
            switch (Event.type) {
                case SDL_QUIT:
                    Running = 1;
                    break;
                case SDL_WINDOWEVENT:
                {
                    //Window resize/orientation change
                    if(Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        //Get screen dimensions
                        global.screen.width = Event.window.data1;
                        global.screen.height = Event.window.data2;
                    }
                }
                // case SDL_MOUSEBUTTONDOWN:
                //     prevX = e.button.x;
                //     prevY = e.button.y;
                //     break;
                // case SDL_MOUSEMOTION:
                //     if (e.motion.state & SDL_BUTTON_LMASK) {
                //         deltaX += prevX - e.motion.x;
                //         deltaY += prevY - e.motion.y;
                //         prevX = e.motion.x;
                //         prevY = e.motion.y;
                //     }
                //     break;
                // case SDL_MULTIGESTURE:
                //     if (e.mgesture.numFingers > 1) {
                //         deltaZoom += e.mgesture.dDist * 10.0f;
                //     }
                //     break;
                // case SDL_MOUSEWHEEL:
                //     deltaZoom += e.wheel.y / 100.0f;
                //     break;
                default:
                    break;
            }
        }

        u32 gameTime = SDL_GetTicks();
        global.time.gameTime = (f32)(gameTime / 1000.0f);
        global.time.deltaTime = (f32)(gameTime - global.time.lastFrameGameTime);

        global.time.lastFrameGameTime = global.time.gameTime;

        if (io.WantTextInput) {
            SDL_StartTextInput();
        } else {
            SDL_StopTextInput();
        }

        ImGui_ImplSdlGLES3_NewFrame(Window);

        GameLoop();
        
        ImGui::Render();
        SDL_GL_SwapWindow(Window);

    }

    ImGui_ImplSdlGLES3_Shutdown(imguiContext);

    SDL_GL_DeleteContext(Context);
    SDL_Quit();
    
    GameEnd();

    return 0;

}