#include "GLOBALS.h"

#if DIRECTX11 == 1
#include "Dx11Adapter.h"

#include "Dx11Renderer.h"

#if DIRECTX11 == 1 
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3native.h>
#include "Dx11Renderer.h"

bool Dx11Adapter::Init(GLFWwindow* window)
{
    MakeASuccess("Inited DX11 Graphics!");

    HWND hwnd = glfwGetWin32Window(window);

    renderer = std::make_unique<Dx11Renderer>();
    renderer.get()->InitDx11Renderer(hwnd);

    return true;
}
void Dx11Adapter::DrawFrame(float dt, std::vector<std::unique_ptr<Instance>>& drawables)
{
    renderer->DrawAFrame(dt, drawables);
}

void Dx11Adapter::CreateSceneResources(int width, int height)
{
    renderer->CreateSceneResources(width, height);
}

void Dx11Adapter::ClearSceneBuffer(float r, float g, float b) {
    renderer->ClearSceneBuffer(r,g,b);
}

void Dx11Adapter::ClearBuffer(float r, float g, float b)
{
    renderer->ClearBuffer(r, g, b);
}

Camera& Dx11Adapter::GetCamera()
{
    return renderer->GetCamera();
}

void Dx11Adapter::ReSizeWindow(
    int width,
    int height,
    Window* wnd)
{
    HWND hwnd = glfwGetWin32Window(wnd->GetWindow());

    renderer->ReSizeWindow(width, height, hwnd);
}

void Dx11Adapter::SetRenderTargetToScene()
{
    renderer->SetRenderTargetToScene();
}

void Dx11Adapter::SetRenderTargetToBackBuffer()
{
    renderer->SetRenderTargetToBackBuffer();
}

void Dx11Adapter::EndFrame() {
    renderer->EndFrame();
}

ID3D11Device* Dx11Adapter::GetDevice() noexcept
{
    return renderer->GetDevice();
}

ID3D11DeviceContext* Dx11Adapter::GetContext() noexcept
{
    return renderer->GetpContext();
}

ID3D11ShaderResourceView* Dx11Adapter::GetSceneSRV() noexcept
{
    return renderer->GetSceneSRV();
}

ID3D11RenderTargetView* Dx11Adapter::GetBackBufferRTV() noexcept
{
    return renderer->GetBackBufferRTV();
}

ID3D11RenderTargetView* Dx11Adapter::GetMainTarget() noexcept
{
    return renderer->GetMainTarget();
}

ID3D11DepthStencilView* Dx11Adapter::GetDepthStencil() noexcept
{
    return renderer->GetDepthStencil();
}
void Dx11Adapter::CleanUp()
{
    
}
#endif