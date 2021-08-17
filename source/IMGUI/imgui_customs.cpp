#ifndef IMGUI_CUSTOMS_CPP
#define IMGUI_CUSTOMS_CPP

bool ImGuiInputDynamicText(const char* label, DynamicString* string, ImGuiInputTextFlags flags)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    
    return ImGui::InputTextEx(label, NULL, string->value, string->allocSize, ImVec2(0, 0), flags | ImGuiInputTextFlags_CallbackResize, [](ImGuiInputTextCallbackData* callbackData) {
        if(callbackData->EventFlag == ImGuiInputTextFlags_CallbackResize) {
            DynamicString* stringReference = (DynamicString*)callbackData->UserData;
            ResizeDynamicString(stringReference, callbackData->BufSize);
            callbackData->Buf = stringReference->value;
            return 1;
        }
        return 0;
    }, (void*)string);
}

#endif