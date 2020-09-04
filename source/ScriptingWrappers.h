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

// #NOTE(Juan): Console
static void ConsoleAddLog(const char* log)
{
	AddLog(&editorConsole, log);
}

// #NOTE(Juan): Engine
static void ReloadCameraData()
{	
    gameState->camera.size = lua["camera"]["size"];
    gameState->camera.ratio = lua["camera"]["ratio"];
    gameState->camera.nearPlane = lua["camera"]["nearPlane"];
    gameState->camera.farPlane = lua["camera"]["farPlane"];
    gameState->camera.view = lua["camera"]["view"];
    gameState->camera.projection = lua["camera"]["projection"];
}

// #NOTE(Juan): Cast
static i32 CharToInt(const char* singleChar)
{
    return (i32)singleChar[0];
}

char castChar[1];
static char* IntToChar(i32 value)
{
    castChar[0] = (char)value;
    return castChar;
}

// #NOTE(Juan): Render

static void PushRenderDisableOverrideVertices()
{
    PushRenderOverrideVertices(0, 0);
}

static void PushRenderDisableOverrideIndices()
{    
    PushRenderOverrideIndices(0, 0);
}

#endif