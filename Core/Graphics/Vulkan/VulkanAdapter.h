#pragma once
#include "GLOBALS.h"

#if VULKAN == 1
#include "IRenderer.h"
#include <memory>
#include "VulkanRender.h"

class VulkanAdapter : public IRenderer
{
public:
    bool Init(GLFWwindow* window) override;
    void initViewport();
    void DrawFrame(float dt,
        std::vector<std::unique_ptr<Instance>>& drawables) override;

    void ClearBuffer(float r, float g, float b) override;

    Camera& GetCamera() override;

    void ReSizeWindow(int width, int height, Window* wnd) override;

    void SetRenderTargetToScene() override;
    void SetRenderTargetToBackBuffer() override;
    void CleanUp() override;
    VkDevice GetDevice() const;
    VkQueue GetGraphicsQueue() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkCommandPool GetCommandPool() const;
    std::vector<VkImageView> GetSwapChainImageViews() const;
    VkInstance& GetInstance() const;
    uint32_t GetGraphicsFamilyIndex() const;
    VkDescriptorPool& GetImGuiPool() const;
    VkRenderPass GetRenderPass() const;
    void UpdateDescriptorSet(std::vector<const Instance*> instances);
    void RenderAMesh(const Instance* drawable);
    Texture* GetViewport() override;

    void EndFrame() override;
    void CreateSceneResources(int width, int height) override;
    void ClearSceneBuffer(float r, float g, float b) override;
    void ReSizeViewport(int width, int height) override;
private:
    std::unique_ptr<VulkanRender> renderer;
};
#endif