#pragma once

#include "GLOBALS.h"
#if VULKAN == 1
#include "ErrorHandling/Errormessage.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <Graphics/Vulkan/ScoreCounter.h>

class VulkanCommandBuffer {
public:
	bool CreateCommandPool(VkDevice device, uint32_t graphicsFamilyIndex);
	bool AllocateCommandBuffers(VkDevice device, uint32_t count);

	//Getters
	VkCommandPool& GetCommandPool() { return m_CommandPool; }
	const VkCommandPool& GetCommandPool() const { return m_CommandPool; }

	std::vector<VkCommandBuffer>& GetCommandBuffers() { return m_CommandBuffers; }
	const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return m_CommandBuffers; }
private:
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_CommandBuffers;
};
#endif