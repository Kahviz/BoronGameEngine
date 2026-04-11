#include "VulkanDevice.h"
#include <iostream>
//Does physical device, device jobs
#if VULKAN == 1
bool VulkanDevice::Init(GLFWwindow* window, VkInstance& instance)
{
    uint32_t gpuCount = 0;
    vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);

    if (gpuCount == 0) {
        MakeAError("No Vulkan GPU found");
        return false;
    }

    std::vector<VkPhysicalDevice> gpus(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());

    int bestScore = -100000;

    for (VkPhysicalDevice device : gpus) {
        int score = m_SC.ScoreDevice(device);

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        #ifdef _DEBUG
            std::cout << props.deviceName << " score = " << score << "\n";
        #endif // _DEBUG


        if (score > bestScore) {
            bestScore = score;
            physicalDevice = device;
        }
    }
    if (physicalDevice == VK_NULL_HANDLE) {
        MakeAError("No suitable GPU found");
        return false;
    }
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        MakeAError("Surface creation failed");
        return false;
    }

    MakeASuccess("Surface Created");


    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    graphicsFamilyIndex = -1;

    for (size_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            VkBool32 presentSupport = false;

            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                graphicsFamilyIndex = i;
                break;
            }
        }
    }

    if (graphicsFamilyIndex == -1) {
        MakeAError("graphicsFamilyIndex == -1");
        return false;
    }

    const char* deviceExtensions[] = { "VK_KHR_swapchain" };

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo{};
    #ifdef _DEBUG
        #if VALIDATIONLAYERS == 1
            deviceCreateInfo.enabledLayerCount = validationLayers.size();
            deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
            deviceCreateInfo.enabledExtensionCount = extensions.size();
            deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

            MakeAInfo("Using VulkanValidationLayers");
        #else
            MakeAInfo("Not Using VulkanValidationLayers");
        #endif
    #else
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.enabledExtensionCount = extensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    #endif
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        MakeAError("Can't Create a VkDevice");
        return false;
    }

    vkGetDeviceQueue(device, graphicsFamilyIndex, 0, &graphicsQueue);

    return true;
}
#endif
