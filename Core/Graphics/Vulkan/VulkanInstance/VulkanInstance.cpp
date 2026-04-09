#include "VulkanInstance.h"
#include <iostream>

#if VULKAN == 1

bool VulkanInstance::Init() {
    uint32_t extCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extCount);

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "UntitledGameEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledExtensionCount = extCount;
    instInfo.ppEnabledExtensionNames = extensions;
    instInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&instInfo, nullptr, &instance) != VK_SUCCESS) {
        MakeAError("Vulkan instance creation failed!");
        return false;
    }

    MakeASuccess("Vulkan instance created");

    return true;
}
#endif // VULKAN == 1