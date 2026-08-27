#pragma once
#include "Widgets/Widgets.h"
#include <vector>
#include "Widgets/Widget.h"
#include "Backends/Backends.h"

class BoronGui {
public:
	static void InitBoronGui(BoronGuiNeeds& p_boronGuiNeeds);
	static void SubmitWidget(std::unique_ptr<Borongui::Widget> p_widget);
	static void EndFrame();
private:
	static std::vector<std::unique_ptr<Borongui::Widget>> widgets;
	bool m_inited = false;
};