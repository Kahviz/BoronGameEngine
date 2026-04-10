#include "VulkanCommandBuffer.h"

#if VULKAN == 1
bool VulkanCommandBuffer::CreateCommandPool(VkDevice device, uint32_t graphicsFamilyIndex) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
        MakeAError("Failed to create command pool!");
        return false;
    }

    MakeASuccess("Command pool created");
    return true;
}
bool VulkanCommandBuffer::AllocateCommandBuffers(VkDevice device, uint32_t count) {
    m_CommandBuffers.resize(count);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    if (vkAllocateCommandBuffers(device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
        MakeAError("Failed to allocate command buffers!");
        return false;
    }

    MakeASuccess("Command buffers allocated");
    return true;
}

#endif