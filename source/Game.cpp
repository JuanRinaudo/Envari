#define SOL_ALL_SAFETIES_ON 1

#include "IMGUI/imgui.h"

#include "File.h"
#include "Data.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Render.h"
#include "GLRender.h"
#include "Input.h"
#include "Scripting.h"

static u32 GameInit() {

    console.InitConsole();

    LoadScriptFile(TableGetString(&initialConfig, "initLuaScript"));

    sol::protected_function Init(lua["Init"]);
    if(Init.valid()) {
        Init();
    }
    else {
        console.AddLog("Error on script 'Init', not valid");
    }
    
    return 0;

}

static u32 GameLoop() {

    f32 fps = (f32)(1 / gameState->time.deltaTime);

    // ss << "TIME: " << global.time.gameTime << " | DELTA TIME: " << global.time.deltaTime << " | FPS: " << fps << endl;

    console.Draw("Console", &consoleOpen);
    
    sol::protected_function Update(lua["Update"]);
    if(Update.valid()) {
        Update();
    }
    else {
        console.AddLog("Error on script 'Update', not valid");
    }

    // ImGui::ShowDemoWindow();
    // NOTE(Juan): IMGUI
    // ImGui::Begin("OpenGL Test");
    
    // ImGui::LabelText("", "Width: %d | Height: %d", gameState->screen.width, gameState->screen.height);
    // ImGui::LabelText("", "Background color looping");
    // ImGui::Checkbox("Color R", (bool *)(&gameState->demo.backgroundR));
    // ImGui::Checkbox("Color G", (bool *)(&gameState->demo.backgroundG));
    // ImGui::Checkbox("Color B", (bool *)(&gameState->demo.backgroundB));
    
    // if (ImGui::Button("Reset Top X")) {
    //     gameState->demo.triangleTopX = 0.0f;
    // }
    // ImGui::SliderFloat("Top X", &gameState->demo.triangleTopX, -0.5f, 0.5f);
    // ImGui::ColorPicker4("Render Color", (f32 *)&gameState->demo.renderColor.E, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);

    // if (ImGui::Button("Reset Speed")) {
    //     gameState->demo.colorSpeed = 1.0f;
    // }
    // ImGui::SliderFloat("Speed", &gameState->demo.colorSpeed, 0.0f, 10.0f);
    
    // ImGui::LabelText("", "Memory: %d / %d", GL_AvailableGPUMemoryKB(), GL_TotalGPUMemoryKB());
    
    // ImGui::End();

    return 0;

}

static u32 GameEnd()
{
    return 0;
}