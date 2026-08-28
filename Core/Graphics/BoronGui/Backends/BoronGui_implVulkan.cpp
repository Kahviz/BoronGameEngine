#include "BoronGui_implVulkan.h"

#if VULKAN == 1
BoronGuiNeeds BoronGui_implVulkan::m_boronGuiNeeds{};

const BoronGuiNeeds& BoronGui_implVulkan::GetGuiNeeds() {
	return m_boronGuiNeeds;
}

void BoronGui_implVulkan::SetGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
	m_boronGuiNeeds = p_boronGuiNeeds;
}

#endif