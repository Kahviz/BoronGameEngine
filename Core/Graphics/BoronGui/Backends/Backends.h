#pragma once

#include "GLOBALS.h"
#include "ErrorHandling/ErrorMessage.h"

#if DIRECTX11 == 1
	#include "BoronGui_implDX11.h"
#endif

#if VULKAN == 1
	#include "BoronGui_implVulkan.h"
#endif

namespace BoronGuiBackends {
	void DrawRect();

	void Init(BoronGuiNeeds& p_boronGuiNeeds);
};