#include "BoronGui_implVulkan.h"

#if VULKAN == 1
#include "Backends.h"

void BoronGuiBackends::DrawRect() {

}
void BoronGuiBackends::Init(BoronGuiNeeds& p_boronGuiNeeds) {
	BoronGui_implVulkan::SetGuiNeeds(p_boronGuiNeeds);
}
#endif

const BoronGuiNeeds& BoronGui_implVulkan::GetGuiNeeds() {
	return m_boronGuiNeeds;
}

void BoronGui_implVulkan::SetGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
	m_boronGuiNeeds = p_boronGuiNeeds;
}
