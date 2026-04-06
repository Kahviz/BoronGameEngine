#pragma once
#include "GLOBALS.h"
#include <GLFW/glfw3.h>
#include "ErrorHandling/ErrorMessage.h"
#include "ScoreCounter.h"

#if VULKAN == 1
#include "vulkan/vulkan.h"

class Builder {
public:
	bool MakeInstance(uint32_t& extCount, VkInstance& instance);
	bool ChooseGPU(VkPhysicalDevice& physicaldevice,ScoreCounter& SC, VkInstance& instance);
	bool InitQueueFamily(VkPhysicalDevice& physicalDevice, uint32_t& graphicsFamilyIndex, VkSurfaceKHR& surface);
	bool CreateDevice(VkPhysicalDevice& physicalDevice, VkDevice& device, uint32_t& graphicsFamilyIndex);

	void ChoosePresentMode(VkPresentModeKHR& presentMode, VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface);

	void CreateAttachments(VkAttachmentDescription& colorAttachment, VkAttachmentReference& colorAttachmentRef, VkSurfaceFormatKHR& surfaceFormat);
	void ChooseSurfaceFormat(VkSurfaceFormatKHR& surfaceFormat, std::vector<VkSurfaceFormatKHR>& formats);
private:
};
#endif