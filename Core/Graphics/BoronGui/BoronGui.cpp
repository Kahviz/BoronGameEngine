#include "BoronGui.h"
#include "BGE_ASSERTS.h"
#include "ErrorHandling/ErrorMessage.h"
#include "Backends/Backends.h"

std::vector<std::unique_ptr<Borongui::Widget>> BoronGui::widgets;

void BoronGui::InitBoronGui(BoronGuiNeeds& p_boronGuiNeeds) {
	CreateInfo("Initing BoronGui");

	BoronGuiBackends::Init(p_boronGuiNeeds);

	widgets.push_back(std::make_unique<Borongui::Frame>());
}

void BoronGui::SubmitWidget(std::unique_ptr<Borongui::Widget> p_widget) {
	BGE_ASSERT_PTR(p_widget.get(), "Widget cannot be nullptr!");

	widgets.push_back(std::move(p_widget));
}

void BoronGui::EndFrame() {
	widgets.clear();
}
