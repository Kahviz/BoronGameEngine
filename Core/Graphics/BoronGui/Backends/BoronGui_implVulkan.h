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
};

class BoronGui_implVulkan {
public:
    const BoronGuiNeeds& GetGuiNeeds();
    void SetGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds);
private:
    BoronGuiNeeds m_boronGuiNeeds;
};
#endif