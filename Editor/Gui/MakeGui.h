#pragma once
#include <vector>
#include <functional>
#include "Object.h"
#include <DirectXMath.h>
#include "GLOBALS.h"
#include <memory>
#include "Instances/Instance.h"
#include "GLFW/glfw3.h"
#include "Window/Window.h"
#include <filesystem>
#include "Style.h"

class Engine;

namespace fs = std::filesystem;

class MakeGui
{
public:
    void MakeStyle();
    void MakeIMGui(Window& wnd, std::vector<std::unique_ptr<Instance>>& Drawables, std::function<Instance* (const std::string&, const std::string&, BML::Vector3, BML::Vector3, bool)> AddAMesh, float* Color3, bool Selec, Engine* engine, Instance& world);
    void MakeIMViewPort(Window& wnd);
    bool MakeDashBoard();
private:
    Style style;
};
