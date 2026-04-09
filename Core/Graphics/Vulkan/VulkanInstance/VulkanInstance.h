#pragma once
#include "GLOBALS.h"

#if VULKAN == 1
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "ErrorHandling/Errormessage.h"

class VulkanInstance {
public:
	bool Init();
	//Getters
	VkInstance& GetInstance() { return instance; }
	const VkInstance& GetInstance() const { return instance; }
private:
	VkInstance instance = VK_NULL_HANDLE;
};
#endif