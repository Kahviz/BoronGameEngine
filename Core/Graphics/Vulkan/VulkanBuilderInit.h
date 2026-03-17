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
	bool InitQueueFamily(VkPhysicalDevice& physicalDevice, size_t& graphicsFamilyIndex, VkSurfaceKHR& surface);
	bool CreateDevice(VkPhysicalDevice& physicalDevice, VkDevice& device, size_t& graphicsFamilyIndex);

	void CreateAttachments(VkAttachmentDescription& colorAttachment, VkAttachmentReference& colorAttachmentRef, VkSurfaceFormatKHR& surfaceFormat);
	void ChooseSurfaceFormat(VkSurfaceFormatKHR& surfaceFormat, std::vector<VkSurfaceFormatKHR>& formats);
private:
};
#endif