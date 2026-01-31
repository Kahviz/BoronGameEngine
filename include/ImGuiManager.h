#pragma once
#include "imgui.h"

class ImGuiManager {
public:
    static void Initialize();
    static void Shutdown();

private:
    static bool isInitialized;
};