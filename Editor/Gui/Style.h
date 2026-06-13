#pragma once
#include "Enums.h"

class Style {
public:
	void CreateImGuiStyle(Boron::Editor::ThemeType theme = Boron::Editor::ThemeType::Classic);
    void CreateImGuiCustomization();
private:
    //Rounding
    float FrameRounding = 5.0f;
    float Default_FrameRounding = 5.0f;

    float WindowRounding = 5.0f;
    float Default_WindowRounding = 5.0f;

    float ChildRounding = 5.0f;
    float Default_ChildRounding = 5.0f;

    float PopupRounding = 5.0f;
    float Default_PopupRounding = 5.0f;

    float GrabRounding = 3.0f;
    float Default_GrabRounding = 3.0f;
};