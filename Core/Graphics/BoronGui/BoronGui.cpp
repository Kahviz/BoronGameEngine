#include "BoronGui.h"
#include "BGE_ASSERTS.h"
#include "ErrorHandling/ErrorMessage.h"
#include "Backends/Backends.h"

std::unique_ptr<BoronGuiBackends::Backends> BoronGui::m_backend;
std::vector<std::unique_ptr<Borongui::Widget>> BoronGui::widgets;

void BoronGui::UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) {
	m_backend->UpdatePerFrameOBJ(p_perFrameStuct);
}

void BoronGui::InitBoronGui(BoronGuiNeeds& p_boronGuiNeeds) {
	bool Inited = false;

	#if VULKAN == 1
		m_backend = std::make_unique<BoronGui_implVulkan>();
		Inited = true;
	#endif

	#if DIRECTX11 == 1

	#endif

	if (Inited) {
		m_backend->SetBoronGuiNeeds(p_boronGuiNeeds);
		m_backend->Init();

		CreateInfo("Initing BoronGui");

		widgets.push_back(std::make_unique<Borongui::Frame>());
	}
}

void BoronGui::SubmitWidget(std::unique_ptr<Borongui::Widget> p_widget) {
	BGE_ASSERT_PTR(p_widget.get(), "Widget cannot be nullptr!");

	widgets.push_back(std::move(p_widget));
}

void BoronGui::EndFrame() {
	widgets.clear();
}

void BoronGui::DrawAFrame() {
	m_backend->RenderAFrame();
}
