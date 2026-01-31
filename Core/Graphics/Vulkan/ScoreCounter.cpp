#include "ScoreCounter.h"

int ScoreCounter::ScoreDevice(VkPhysicalDevice gpu)
{
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceMemoryProperties memProps;

    vkGetPhysicalDeviceProperties(gpu, &props);
    vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);

    int score = 0;

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        score += 200;
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
        score -= 500;
    }

    for (uint32_t i = 0; i < memProps.memoryHeapCount; i++)
    {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            score += static_cast<int>(memProps.memoryHeaps[i].size / (1024 * 1024 * 1024));
        }
    }

    score += props.limits.maxImageDimension2D / 1024;
    score += props.limits.maxComputeWorkGroupInvocations;

    return score;
}
