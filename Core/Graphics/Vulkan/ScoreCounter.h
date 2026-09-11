#pragma once
#include "Vulkan.h"
#include "GLOBALS.h"

class ScoreCounter {
public:
#if VULKAN == 1
	int ScoreDevice(VkPhysicalDevice gpu);
#endif
};