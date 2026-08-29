#pragma once
#include "GLOBALS.h"

#if VULKAN == 1
#include "vulkan/Vulkan.h"

struct BoronGuiNeeds {
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkExtent2D swapchainExtent{};
};

class BoronGui_implVulkan {
public:
    static void BeginFrame();
    static void SetupRenderState(VkCommandBuffer commandBuffer);
    static void EndFrame();
    static bool Init();
    static const BoronGuiNeeds& GetGuiNeeds();
    static void SetGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds);
    static bool InitPipeline();
private:
    static VkShaderModule m_vertShaderModule;
    static VkShaderModule m_fragShaderModule;
    static BoronGuiNeeds m_boronGuiNeeds;
    static VkPipelineLayout m_pipelineLayout;
    static VkPipeline m_graphicsPipeline;
};
#endif