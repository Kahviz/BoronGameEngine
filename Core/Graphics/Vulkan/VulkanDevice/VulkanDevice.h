#pragma once
#include "GLOBALS.h"
#if VULKAN == 1
#include "ErrorHandling/Errormessage.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <Graphics/Vulkan/ScoreCounter.h>

class VulkanDevice {
public:
	bool Init(GLFWwindow* window, VkInstance& instance);

	//Getters
	VkDevice& GetDevice() { return device; }
	const VkDevice& GetDevice() const { return device; }

	VkPhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
	const VkPhysicalDevice& GetPhysicalDevice() const { return physicalDevice; }
	

	VkSurfaceKHR& GetSurface() { return surface; }
	const VkSurfaceKHR& GetSurface() const { return surface; }

	uint32_t& GetFamilyIndex() { return graphicsFamilyIndex; }
	const uint32_t& GetFamilyIndex() const { return graphicsFamilyIndex; }

	VkQueue& GetGraphicsQueue() { return graphicsQueue; }
	const VkQueue& GetGraphicsQueue() const { return graphicsQueue; }
private:
	ScoreCounter m_SC;

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;

	uint32_t graphicsFamilyIndex = -1;
};
#endif