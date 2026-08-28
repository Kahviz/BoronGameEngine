#include "BoronGui_implVulkan.h"

const BoronGuiNeeds& BoronGui_implVulkan::GetGuiNeeds() {
	return m_boronGuiNeeds;
}

void BoronGui_implVulkan::SetGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
	m_boronGuiNeeds = p_boronGuiNeeds;
}
