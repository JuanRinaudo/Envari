model = IdM44();
view = IdM44();
projection = IdM44();

characterFrames = {
    "images/sprite_female_mage_angry01.png",
    "images/sprite_female_mage_neutral01.png",
    "images/sprite_female_mage_relieved01.png",
    "images/sprite_female_mage_sad01.png",
    "images/sprite_female_mage_smile01.png",
    "images/sprite_female_mage_smirk01.png",
    "images/sprite_female_mage_suspicious01.png",
};

function Update()
    camera.size = 1;
    camera.ratio = screen.width / screen.height;
    camera.nearPlane = 0.01;
    camera.farPlane = 100.0;
    ReloadCameraData();
    projection = OrtographicProjection(camera.size, camera.ratio, camera.nearPlane, camera.farPlane);

    -- PushRenderRectangle(V2(-0, -1), V2(1, 1));

    currentImage = (Floor(time.gameTime) % #characterFrames) + 1;
    
    PushRenderTransparent(GL_FUNC_ADD, GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    PushRenderColor(1, 1, 1, 1);
    PushRenderImage(V2(-1, -1.5), V2(2, 3), "images/Background-Day.png", IMAGE_KEEP_RATIO_Y);
    PushRenderColor(1, 1, 1, .5);
    PushRenderCircle(V2(time.gameTime % 2 - 1, time.gameTime % 2 - 1), 0.5, 20);
    PushRenderColor(1, 1, 1, 1);
    PushRenderImage(V2(-1, -1), V2(2, 2), characterFrames[currentImage], IMAGE_KEEP_RATIO_Y);
    PushRenderImage(V2(-1, 0.5), V2(2, .5), "images/Panel_Dialogue.png", IMAGE_KEEP_RATIO_Y);
    
    PushRenderFont("fonts/VarelaRound-Regular.ttf", 128.0, 1024, 1024);
    PushRenderRectangle(V2(-1, 0.5), V2(.1, .1));
    PushRenderText(V2(-1, 0.5), V2(.1, .1), "Hola mi nombre es juan\nY esto es\nUna prueba");
    -- PushRenderImage(V2(-1, 0.5), V2(2, .5), "fonts/VarelaRound-Regular.ttf", IMAGE_KEEP_RATIO_Y);
    
    PushRenderColor(1, 1, 1, .3);
    PushRenderRectangle(V2(Sin(time.gameTime), -.5), V2(.5, .5));
    PushRenderRectangle(V2(Cos(time.gameTime), .5), V2(.5, .5));

    -- PushRenderTransparentDisable();
    
    -- ConsoleAddLog("Test" + time.gameTime);

    -- ImguiBegin("Lua IMGUI");
    -- ImguiLabelText("Label", "Test");
    -- ImguiButton("Test", V2());
    -- ImguiEnd();
end