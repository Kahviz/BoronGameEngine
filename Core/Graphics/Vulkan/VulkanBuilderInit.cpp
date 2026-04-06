#include "GLOBALS.h"

#if VULKAN == 1
#include "VulkanBuilderInit.h"

bool Builder::MakeInstance(uint32_t& extCount, VkInstance& instance)
{
    const char** extensions = glfwGetRequiredInstanceExtensions(&extCount);

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "UntilitedGameEngine";
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledExtensionCount = extCount;
    instInfo.ppEnabledExtensionNames = extensions;

    if (vkCreateInstance(&instInfo, nullptr, &instance) != VK_SUCCESS) {
        MakeAError("Vulkan doesnt work");
        return false;
    }
    return true;
}

bool Builder::ChooseGPU(VkPhysicalDevice& physicaldevice, ScoreCounter& SC, VkInstance& instance)
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
        int score = SC.ScoreDevice(device);

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

#ifdef _DEBUG
        std::cout << props.deviceName << " score = " << score << "\n";
#endif // _DEBUG


        if (score > bestScore) {
            bestScore = score;
            physicaldevice = device;
        }
    }

    if (physicaldevice == VK_NULL_HANDLE) {
        MakeAError("No suitable GPU found");
        return false;
    }

    VkPhysicalDeviceProperties selectedProps;
    vkGetPhysicalDeviceProperties(physicaldevice, &selectedProps);

#ifdef _DEBUG
    std::cout << "Selected GPU: " << selectedProps.deviceName << "\n";
#endif // _DEBUG

    return true;
}

bool Builder::InitQueueFamily(VkPhysicalDevice& physicalDevice, uint32_t& graphicsFamilyIndex, VkSurfaceKHR& surface)
{
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

    return true;
}

bool Builder::CreateDevice(VkPhysicalDevice& physicalDevice, VkDevice& device, uint32_t& graphicsFamilyIndex)
{
    const char* deviceExtensions[] = { "VK_KHR_swapchain" };

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo{};
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

    return true;
}

void Builder::ChoosePresentMode(VkPresentModeKHR& presentMode, VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface)
{
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    if (vSync) {
        presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }
    else {
        presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

        for (const auto& mode : presentModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                presentMode = mode;
                break;
            }
        }
    }
}

void Builder::CreateAttachments(VkAttachmentDescription& colorAttachment, VkAttachmentReference& colorAttachmentRef, VkSurfaceFormatKHR& surfaceFormat)
{
    colorAttachment.format = surfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void Builder::ChooseSurfaceFormat(VkSurfaceFormatKHR& surfaceFormat, std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = availableFormat;
            break;
        }
    }
}
#endif