#include "Backends.h"

#if VULKAN == 1
#include "BoronGui_implVulkan.h"
#endif

void BoronGuiBackends::DrawRect() {

}

void BoronGuiBackends::Init(BoronGuiNeeds& p_boronGuiNeeds) {
	#if VULKAN == 1
		BoronGui_implVulkan::SetGuiNeeds(p_boronGuiNeeds);
	#endif
	#if DIRECTX11 == 1
		
	#endif
}