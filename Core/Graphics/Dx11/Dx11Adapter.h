#pragma once
#include "IRenderer.h"
#include <vector>
#include <memory>
#include "Instance.h"

class Dx11Renderer;

class Dx11Adapter : public IRenderer
{
public:
    bool Init(GLFWwindow* window) override;
    void DrawFrame(float dt,
        std::vector<std::unique_ptr<Instance>>& drawables) override;
    void EndFrame() override;
    void ClearBuffer(float r, float g, float b) override;
    void CreateSceneResources(int width, int height) override;
    void ClearSceneBuffer(float r, float g, float b) override;

    Camera& GetCamera() override;

    void ReSizeWindow(int width, int height, Window* wnd) override;

    void SetRenderTargetToScene() override;
    void SetRenderTargetToBackBuffer() override;

    ID3D11Device* GetDevice() noexcept;
    ID3D11DeviceContext* GetContext() noexcept;
    ID3D11ShaderResourceView* GetSceneSRV() noexcept;

    ID3D11RenderTargetView* GetBackBufferRTV() noexcept;

    ID3D11RenderTargetView* GetMainTarget() noexcept; 
    ID3D11DepthStencilView* GetDepthStencil() noexcept;
    Texture* GetViewport() override;

    void CleanUp() override;

    Dx11Renderer* GetRenderer()
    {
        return renderer.get();
    }
private:
    std::unique_ptr<Dx11Renderer> renderer;
};