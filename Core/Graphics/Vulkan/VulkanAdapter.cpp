#include "GLOBALS.h"
#include "VulkanAdapter.h"
#if VULKAN == 1
#include <GLFW/glfw3.h>

bool VulkanAdapter::Init(GLFWwindow* window)
{
    renderer = std::make_unique<VulkanRender>();
    return renderer->Init(window);
}

void VulkanAdapter::DrawFrame(
    float dt,
    std::vector<std::unique_ptr<Instance>>& drawables)
{
    renderer->DrawFrame(dt, drawables);
}

void VulkanAdapter::ClearBuffer(float r, float g, float b)
{
    renderer->ClearBuffer(r, g, b);
}

Camera& VulkanAdapter::GetCamera()
{
    return renderer->GetCamera();
}

void VulkanAdapter::ReSizeWindow(int width, int height, Window* wnd)
{
    renderer->RecreateSwapchain();
}

void VulkanAdapter::SetRenderTargetToScene() {}
void VulkanAdapter::SetRenderTargetToBackBuffer() {}

VkPhysicalDevice VulkanAdapter::GetPhysicalDevice() const
{
    return renderer->GetPhysicalDevice();
}

VkCommandPool VulkanAdapter::GetCommandPool() const
{
    return renderer->GetCommandPool();
}


VkDevice VulkanAdapter::GetDevice() const
{
    VkDevice device = renderer->GetDevice();

    if (device == VK_NULL_HANDLE)
        CreateError("Logical device not initialized!");

    return device;
}

VkQueue VulkanAdapter::GetGraphicsQueue() const
{
    return renderer->GetGraphicsQueue();
}

void VulkanAdapter::EndFrame()
{
    renderer->EndFrame();
}

void VulkanAdapter::CreateSceneResources(int width, int height)
{
    //renderer->CreateSceneResources(width, height);
}

void VulkanAdapter::ClearSceneBuffer(float r, float g, float b)
{
    //renderer->ClearSceneBuffer(r, g, b);
}

void VulkanAdapter::CleanUp()
{
    renderer->Cleanup();
}
std::vector<VkImageView> VulkanAdapter::GetSwapChainImageViews() const
{
    return renderer->GetSwapChainImageViews();
}

VkInstance& VulkanAdapter::GetInstance() const
{
    return renderer->GetInstance();
}

uint32_t VulkanAdapter::GetGraphicsFamilyIndex() const
{
    return renderer->GetGraphicsFamilyIndex();
}

VkDescriptorPool& VulkanAdapter::GetImGuiPool() const
{
    return renderer->GetImGuiPool();
}

VkRenderPass VulkanAdapter::GetRenderPass() const
{
    return renderer->GetRenderPass();
}

Texture* VulkanAdapter::GetDefaultTexture()
{
    return renderer->GetDefaultTexture();
}

void VulkanAdapter::UpdateDescriptorSet(const Instance* inst)
{
    renderer->UpdateDescriptorSet(inst);
}
void VulkanAdapter::RenderAMesh(const Instance* drawable) {
    renderer->RenderAMesh(drawable);
}

#endif