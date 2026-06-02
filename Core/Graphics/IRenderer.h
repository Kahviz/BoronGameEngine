#pragma once

#include "Camera/Camera.h"
#include <GLFW/glfw3.h>
#include <Window/Window.h>
#include "GraphicsBackends.h"

class Instance;

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual bool Init(GLFWwindow* window) = 0;
    virtual void CleanUp() = 0;
    virtual void DrawFrame(float dt,
        std::vector<std::unique_ptr<Instance>>& drawables) = 0;
    virtual void EndFrame() = 0;
    virtual void ClearBuffer(float r, float g, float b) = 0;
    virtual Camera& GetCamera() = 0;
    virtual void CreateSceneResources(int width, int height) = 0;
    virtual void ClearSceneBuffer(float r, float g, float b) = 0;

    virtual void ReSizeWindow(int width, int height, Window* wnd) = 0;

    virtual void SetRenderTargetToScene() = 0;
    virtual void SetRenderTargetToBackBuffer() = 0;
};
