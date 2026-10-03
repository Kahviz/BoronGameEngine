#pragma once

#include "GLOBALS.h"

#if VULKAN == 1
	#include <vulkan/vulkan.h>
	#include <vulkan/vulkan_core.h>

	//structs
	struct MeshStruct {
		VkDevice device{};
		VkPhysicalDevice physicalDevice{};
		VkCommandPool commandPool{};
		VkQueue graphicsQueue{};
	};

	struct MeshDrawStruct {
		VkCommandBuffer commandBuffer{};
	};
//Made imgui vulkan here include
#endif


