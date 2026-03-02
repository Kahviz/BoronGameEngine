#pragma once
#include <vector>
#include <functional>
#include "Object.h"
#include <DirectXMath.h>
#include "GLOBALS.h"
#include <memory>
#include "Instance.h"
#include "GLFW/glfw3.h"
#include "Window/Window.h"

class MakeGui
{
public:
    void MakeIMGui(
        Window& wnd,
        std::vector<std::unique_ptr<Instance>>& Drawables,
        std::function<Instance* (const std::string&,
            const std::string&,
            FLOAT3, FLOAT3, 
            bool)> AddAMesh,
        float* Color3,
        bool Selec
    );
    void MakeIMViewPort(Window& wnd);
    bool MakeDashBoard();
private:
    Instance world;
};
