#include "ImGuiManager.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_dx11.h"

bool ImGuiManager::isInitialized = false;

void ImGuiManager::Initialize()
{
    if (isInitialized) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui::SetCurrentContext(ImGui::GetCurrentContext());

    isInitialized = true;
}

void ImGuiManager::Shutdown()
{
    if (!isInitialized) return;

    isInitialized = false;
}