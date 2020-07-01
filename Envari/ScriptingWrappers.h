#if !defined(SCRIPTING_WRAPPERS_H)
#define SCRIPTING_WRAPPERS_H

// #NOTE(Juan): IMGUI
static void ImguiBegin(const char* name) {
    ImGui::Begin(name);
}

static bool ImguiCheckbox(const char* label, bool value) {
	ImGui::Checkbox(label, &value);
	return value;
}

static bool ImguiButton(const char* label, v2 size) {
	return ImGui::Button(label, ImVec2(size.x, size.y));
}

#endif