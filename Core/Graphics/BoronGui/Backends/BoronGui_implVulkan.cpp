#include "BoronGui_implVulkan.h"
#include <ErrorHandling/ErrorMessage.h>

#if VULKAN == 1
BoronGuiNeeds BoronGui_implVulkan::m_boronGuiNeeds{};

void BoronGui_implVulkan::BeginFrame() {

}

void BoronGui_implVulkan::DrawRect() {
}

void BoronGui_implVulkan::EndFrame() {
}

bool BoronGui_implVulkan::Init() {
	CreateInfo("Init func");

	return true;
}

const BoronGuiNeeds& BoronGui_implVulkan::GetGuiNeeds() {
	return m_boronGuiNeeds;
}

void BoronGui_implVulkan::SetGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
	m_boronGuiNeeds = p_boronGuiNeeds;
}

#endif