#pragma once
#include <vector>
#include <functional>
#include <DirectXMath.h>
#include "GLOBALS.h"
#include <memory>
#include "GLFW/glfw3.h"
#include "Window/Window.h"
#include <filesystem>
#include "Style.h"
#include "IRenderer.h"
#include "Image2d.h"
#include "ECS.h"

class Engine;

namespace fs = std::filesystem;

class MakeGui
{
public:
    void MakeStyle();
    void MakeIMGui(ECS& ecs, Window& wnd, float* Color3, bool Selec, Engine* engine, EntityECS world, IRenderer* renderer);
    void MakeIMViewPort(Window& wnd);
    void CreateErrorPopUp(IRenderer* renderer, Image2d& image2d, const std::string& errormsg, const float duration);
    bool MakeDashBoard(IRenderer* renderer);
    void RenderPopUps(float deltatime);
private:
    struct Image {
        Image2d image2d;
        float duration;
        std::string reason;
    };
    std::vector<Image> images;
    Style style;
};
