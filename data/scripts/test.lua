model = IdM44();
view = IdM44();

ratio = screen.width / screen.height;

projection = OrtographicProyection(1, ratio, 0.01, 100.0);

boolValue = false;
function Update()
    ratio = screen.width / screen.height;
    projection = OrtographicProyection(1, ratio, 0.01, 100.0);

    PushColor(1, 0, 0, 1);
    --PushRectangle(V2(-0, -1), V2(1, 1));
    PushCircle(V2(0, 0), 0.5, 20);

    ImguiBegin("Lua IMGUI");
    ImguiLabelText("Label", "Test");
    ImguiLabelText("Label", "Test");
    ImguiLabelText("Label", "Test");
    ImguiLabelText("Label", "Test");
    ImguiLabelText("Label", "Test");
    boolValue = ImguiCheckbox("Label", boolValue);
    ImguiButton("Test", V2());
    ImguiEnd();
end