#ifndef EDITOR_COMMON_H
#define EDITOR_COMMON_H

static void InitImGui() {
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
#endif

    IMGUI_CHECKVERSION(); 
    ImGuiContext* context = ImGui::CreateContext();
    ImGuiIO& imguiIO = ImGui::GetIO();
    (void)imguiIO;
    imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imguiIO.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_DockingEmptyBg].w = 0.0f;

    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

#endif