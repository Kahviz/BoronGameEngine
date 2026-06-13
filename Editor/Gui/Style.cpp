#include "Style.h"
#include "ImGui.h"
#include <string>

namespace ImGui
{
    inline void SliderFloatAndInputFloat(const char* name, float& value)
    {
        std::string sliderId = std::string(name) + "##Slider";
        std::string inputId = std::string(name) + "##Input";

        ImGui::SliderFloat(sliderId.c_str(), &value, 0.0f, 100.0f, "%.2f");
        ImGui::InputFloat(inputId.c_str(), &value);
    }
}
void Style::CreateImGuiStyle(Boron::Editor::ThemeType theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    colors[ImGuiCol_ButtonHovered] = ImVec4(0.065f, 0.065f, 0.065f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.06f, 0.05f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.07f, 0.07f, 0.07f, 1.0f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.015f, 0.015f, 0.015f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);

    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);

    style.FrameRounding = FrameRounding;
    style.WindowRounding = WindowRounding;
    style.ChildRounding = ChildRounding;
    style.PopupRounding = PopupRounding;
    style.GrabRounding = GrabRounding;
}

void Style::CreateImGuiCustomization()
{
    ImGui::SliderFloatAndInputFloat("FrameRounding", FrameRounding);

    if (ImGui::Button("Reset ##1")) {
        FrameRounding = Default_FrameRounding;
    }

    ImGui::SliderFloatAndInputFloat("WindowRounding", WindowRounding);

    if (ImGui::Button("Reset ##2")) {
        WindowRounding = Default_WindowRounding;
    }

    ImGui::SliderFloatAndInputFloat("ChildRounding", ChildRounding);

    if (ImGui::Button("Reset ##3")) {
        ChildRounding = Default_ChildRounding;
    }

    ImGui::SliderFloatAndInputFloat("GrabRounding", GrabRounding);

    if (ImGui::Button("Reset ##4")) {
        GrabRounding = Default_GrabRounding;
    }

    ImGui::PopStyleColor();
    ImGui::End();
}
