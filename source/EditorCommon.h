#ifndef EDITOR_COMMON_H
#define EDITOR_COMMON_H

static void InitImGui() {
    const char* glsl_version = 0;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imguiIO = ImGui::GetIO();
    imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imguiIO.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_DockingEmptyBg].w = 0.0f;

    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

#endif