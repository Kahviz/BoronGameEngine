#include "Graphics.h"
#include "Releaser.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include "GLOBALS.h"
#include "Window/Window.h"

#if DIRECTX11 == 1 
    #include <Dx11Adapter.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#if VULKAN == 1
    #include "VulkanAdapter.h"
    #include "VulkanRender.h"
#endif

#include <GLFW/glfw3native.h>

bool Graphics::InitGraphics(GLFWwindow* window)
{
    #if VULKAN == 1
        Renderer = std::make_unique<VulkanAdapter>();
    #elif DIRECTX11 == 1
        Renderer = std::make_unique<Dx11Adapter>();
    #endif

    return Renderer->Init(window);
}

void Graphics::SetRenderTargetToScene() {
    Renderer->SetRenderTargetToScene();
}

void Graphics::RenderAMesh(
    float Deltatime,
    const Instance* drawable
) 
{
    #if VULKAN == 1
        auto& vk = static_cast<VulkanAdapter&>(*Renderer.get());
        vk.RenderAMesh(drawable);
    #endif

    #if DIRECTX11 == 1
    #endif
}
void Graphics::SetRenderTargetToBackBuffer() {
    Renderer->SetRenderTargetToBackBuffer();
}


void Graphics::ReSizeWindow(int width, int height, Window* wnd)
{
    Renderer->ReSizeWindow(width, height, wnd);
}

void Graphics::CreateSceneResources(int width, int height) {
    Renderer->CreateSceneResources(width, height);
}

#if DIRECTX11 == 1
    ID3D11DepthStencilView* Graphics::GetDepthStencil()
    {
        auto* dx = static_cast<Dx11Adapter*>(Renderer.get());
        return dx->GetDepthStencil();
    }

    ID3D11Device* Graphics::GetDevice() noexcept
    {
        auto* dx = static_cast<Dx11Adapter*>(Renderer.get());
        return dx->GetDevice();
    }

    ID3D11DeviceContext* Graphics::GetpContext() noexcept
    {
        auto* dx = static_cast<Dx11Adapter*>(Renderer.get());
        return dx->GetContext();
    }

    ID3D11ShaderResourceView* Graphics::GetSceneSRV() {
        auto* dx = static_cast<Dx11Adapter*>(Renderer.get());
        return dx->GetSceneSRV();
    }

    ID3D11RenderTargetView* Graphics::GetBackBufferRTV()
    {
        auto* dx = static_cast<Dx11Adapter*>(Renderer.get());
        return dx->GetBackBufferRTV();
    }

    ID3D11RenderTargetView* Graphics::GetMainTarget()
    {
        auto* dx = static_cast<Dx11Adapter*>(Renderer.get());
        return dx->GetMainTarget();
    }
#endif

Camera& Graphics::GetCamera()
{
    return Renderer->GetCamera();
}

void Graphics::EndFrame()
{
    Renderer->EndFrame();
}

void Graphics::DrawAFrame(float DELTATIME, std::vector<std::unique_ptr<Instance>>& Drawables) {
    Renderer->DrawFrame(DELTATIME, Drawables);
}

void Graphics::ClearBuffer(float r, float g, float b)
{
    Renderer->ClearBuffer(r,g,b);
}

void Graphics::ClearSceneBuffer(float r, float g, float b)
{
    Renderer->ClearSceneBuffer(r, g, b);
}
