#define SOL_ALL_SAFETIES_ON 1

#include "IMGUI/imgui.h"
#include "LUA/sol.hpp"

#include "Defines.h"
#include "GameMath.h"
#include "Intrinsics.h"
#include "Render.h"
#include "Scripting.h"

static u32 GameInit() {

    InitializeArena(&temporalState->temporalArena, (memoryIndex)(gameState->memory.temporalStorageSize - sizeof(TemporalData)), (u8 *)gameState->memory.temporalStorageSize + sizeof(TemporalData));

    console.InitConsole();

    gameState->demo.backgroundR = true;
    gameState->demo.backgroundG = true;
    gameState->demo.backgroundB = true;
    gameState->demo.colorSpeed = 1.0f;
    gameState->demo.triangleTopX = 0.0f;
    gameState->demo.renderColor = V4(1, 1, 1, 1);
    
    return 0;

}

static u32 GameLoop() {

    f32 fps = (f32)(1 / gameState->time.deltaTime);

    // ss << "TIME: " << global.time.gameTime << " | DELTA TIME: " << global.time.deltaTime << " | FPS: " << fps << endl;

    f32 clearR = Abs(Sin(gameState->time.gameTime * gameState->demo.colorSpeed) * gameState->demo.backgroundR);
    f32 clearG = Abs(Cos(gameState->time.gameTime * gameState->demo.colorSpeed) * gameState->demo.backgroundG);
    f32 clearB = Abs((Sin(gameState->time.gameTime * gameState->demo.colorSpeed) + Cos(gameState->time.gameTime * gameState->demo.colorSpeed)) * 0.5f * gameState->demo.backgroundB);

    console.Draw("Example: Console", &consoleOpen);

    PushClear(clearR, clearG, clearB);
    PushColor(gameState->demo.renderColor.r, gameState->demo.renderColor.g, gameState->demo.renderColor.b, gameState->demo.renderColor.a);
    //PushTriangle(V2(0, 0), V2(-0.5f, -0.5f), V2(0.5f, -0.5f), V2(gameState->demo.triangleTopX,  0.5f));
    // PushRectangle(V2(0, 0), V2(Sin(gameState->time.gameTime), Cos(gameState->time.gameTime)));
    //PushImage(V2(0, 0), V2(1, 1), "images/sprite_female_mage_angry01.png");

    sol::protected_function Update(lua["Update"]);
    if(Update.valid()) {
        Update();
    }
    else {
        
    }

    // ImGui::ShowDemoWindow();
    // NOTE(Juan): IMGUI
    ImGui::Begin("OpenGL Test");
    ImGui::LabelText("", "Width: %d | Height: %d", gameState->screen.width, gameState->screen.height);
    ImGui::LabelText("", "Background color looping");
    ImGui::Checkbox("Color R", (bool *)(&gameState->demo.backgroundR));
    ImGui::Checkbox("Color G", (bool *)(&gameState->demo.backgroundG));
    ImGui::Checkbox("Color B", (bool *)(&gameState->demo.backgroundB));
    
    if (ImGui::Button("Reset Top X")) {
        gameState->demo.triangleTopX = 0.0f;
    }
    ImGui::SliderFloat("Top X", &gameState->demo.triangleTopX, -0.5f, 0.5f);
    ImGui::ColorPicker4("Render Color", (f32 *)&gameState->demo.renderColor.E, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);

    if (ImGui::Button("Reset Speed")) {
        gameState->demo.colorSpeed = 1.0f;
    }
    ImGui::SliderFloat("Speed", &gameState->demo.colorSpeed, 0.0f, 10.0f);
    ImGui::End();

    return 0;

}

static u32 GameEnd()
{
    return 0;
}