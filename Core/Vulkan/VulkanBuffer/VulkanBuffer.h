#pragma once
#include "GLOBALS.h"

#if VULKAN == 1
#include "vulkan/vulkan.h"

class VulkanBuffer {
public:
    bool Create(VkDevice& p_device, VkPhysicalDevice p_physicalDevice, VkDeviceSize p_size, VkBufferUsageFlags p_usage, VkMemoryPropertyFlags p_properties);
    void Destroy();

    bool UploadData(const void* p_data, VkDeviceSize p_size);
    bool Resize(VkDeviceSize p_newSize, VkCommandPool p_commandPool, VkQueue p_queue);

    VkBuffer GetBuffer() const;
    VkDeviceMemory GetMemory() const;
private:
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    //Resizing
    VkBufferUsageFlags m_usage{};
    VkMemoryPropertyFlags m_properties{};
    VkDeviceSize m_deviceSize{};
};
#endif