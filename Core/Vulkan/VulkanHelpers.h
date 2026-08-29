#pragma once

#include "GLOBALS.h"

#if VULKAN == 1
#include "Vulkan/vulkan.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "BoronMathLibrary.h"
#include "ErrorHandling/ErrorMessage.h"

#include "BGE_ASSERTS.h"

struct UniformBufferObject {
    BML::Matrix4x4 model{};
    BML::Matrix4x4 view{};
    BML::Matrix4x4 proj{};
    GPUVector3 color{};
    float UsesTexture = 0.0f;
    BML::Matrix4x4 lightSpaceMatrix{};
};

inline std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        CreateError("Failed to open file: " + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

template<size_t N>
std::vector<char> ReadShader(const uint32_t(&shader)[N])
{
    const char* data = reinterpret_cast<const char*>(shader);

    return std::vector<char>(
        data,
        data + N * sizeof(uint32_t)
    );
}

inline VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    BGE_ASSERT_VKRESULT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "Failed to create shader module!");

    return shaderModule;
}

inline uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, & memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    CreateError("Failed to find suitable memory type!");
}
inline VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice) {
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };

    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }

    CreateError("Failed to find depth format!");
}
inline VkCommandBuffer BeginSingleTimeCommands(VkCommandPool commandPool,VkDevice device)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

inline void EndSingleTimeCommands(VkCommandBuffer commandBuffer,VkQueue graphicsQueue, VkDevice device, VkCommandPool commandPool)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, & submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

inline void CreateBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory,
    VkDevice device,
    VkPhysicalDevice psyDevice
) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    BGE_ASSERT_VKRESULT(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer), "Failed to create buffer!");

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, buffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, properties, psyDevice);

    BGE_ASSERT_VKRESULT(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory), "Failed to allocate buffer memory!");

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

inline void CopyBuffer(VkBuffer p_src, VkBuffer p_dst, VkDeviceSize p_size, VkCommandPool p_cmdp, VkDevice p_device, VkQueue p_gQ) {
    VkCommandBuffer cmd = BeginSingleTimeCommands(p_cmdp, p_device);

    VkBufferCopy copy{};
    copy.size = p_size;
    vkCmdCopyBuffer(cmd, p_src, p_dst, 1, &copy);

    EndSingleTimeCommands(cmd, p_gQ, p_device, p_cmdp);
}
#endif