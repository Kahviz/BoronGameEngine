#pragma once

#include "GraphicsBackends.h"
#include <memory>
#include <string>
#include "Camera/Camera.h"
#include "GLFW/glfw3.h"
#include "GLOBALS.h"
#include "BoronMathLibrary.h"
class Window;

#include "GraphicsBackends.h"
#include "ErrorHandling/ErrorMessage.h"

#include <vector>
#include "Instances/Instance.h"
#include <IRenderer.h>
#include <VulkanAdapter.h>

class Graphics
{
public:
    Graphics() = default;
    ~Graphics() = default;

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    bool InitGraphics(GLFWwindow* window);
    void SetRenderTargetToScene();
    void RenderAMesh(float Deltatime,
        const Instance* drawable
    );
    void SetRenderTargetToBackBuffer();

    Camera& GetCamera();

    void EndFrame();

    void DrawAFrame(float DELTATIME, std::vector<std::unique_ptr<Instance>>& Drawables);

    void ClearBuffer(float r, float g, float b);
    void ClearSceneBuffer(float r, float g, float b);

    void ReSizeWindow(int width, int height, Window* wnd);

    void CreateSceneResources(int width, int height);
#if DIRECTX11 == 1
    ID3D11DepthStencilView* GetDepthStencil();

    ID3D11Device* GetDevice() noexcept;
    ID3D11DeviceContext* GetpContext() noexcept;
    
    ID3D11ShaderResourceView* GetSceneSRV();

    ID3D11RenderTargetView* GetBackBufferRTV();

    ID3D11RenderTargetView* GetMainTarget();
#endif
    
    IRenderer& GetRenderer()
    {
        return *Renderer;
    }
private:
    std::unique_ptr<IRenderer> Renderer;
};