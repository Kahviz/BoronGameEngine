#pragma once
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include <string>
#include "Camera.h"
#include <wrl/client.h>
#include "GLFW/glfw3.h"

#include "Dx11/Dx11Renderer.h"
#include "Vulkan/VulkanRender.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Graphics
{
public:
    Graphics() = default;
    ~Graphics() = default;  // Vain yksi destruktori!

    // Poista kopiointi
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    // Muut metodit
    void InitGraphics(GLFWwindow* window);
    void SetRenderTargetToScene();
    void SetRenderTargetToBackBuffer();
    void CreateDeviceAndSwapChain(int width, int height, HWND hWnd);
    void CreateViewport(int width, int height);
    void CreateDepthStencil(int width, int height);
    void CreateRenderTarget();
    void CreateConstantBuffers();
    void CompileShaders();

    Camera& GetCamera();
    ID3D11Device* GetDevice() noexcept;
    ID3D11DeviceContext* GetpContext() noexcept;

    void EndFrame();

    void DrawMesh(float deltaTime, Mesh& mesh, FLOAT3 Orientation, FLOAT3& pos,
        FLOAT3& size, INT3 color, FLOAT3& Velocity, bool Anchored,
        float Roughness, float Brightness);

    void ClearBuffer(float r, float g, float b);
    void ClearSceneBuffer(float r, float g, float b);
    void ReSizeWindow(int width, int height, HWND hWnd);

    void CreateSceneResources(int width, int height);
    ID3D11ShaderResourceView* GetSceneSRV();
    ID3D11RenderTargetView* GetBackBufferRTV();
    ID3D11RenderTargetView* GetMainTarget();

    ID3D11DepthStencilView* GetDepthStencil();

private:
    std::unique_ptr<VulkanRender> VR;
    std::unique_ptr<Dx11Renderer> DR;

    bool UsesDx11 = true;
    bool UsesVulkan = false;
};