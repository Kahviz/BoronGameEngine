#pragma once
#include "Widgets/Widgets.h"
#include <vector>
#include "Widgets/Widget.h"
#include "Backends/Backends.h"
#include "Backends/BoronGui_implVulkan.h"
#include "BoronGuiTypes.h"

class BoronGui {
public:
	static void UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct);
	static void InitBoronGui(BoronGuiNeeds& p_boronGuiNeeds);
	static void SubmitWidget(Borongui::Widget& p_widget);
	static void EndFrame();
	static void DrawAFrame();
	static void DrawWidgets();
private:
	static std::unique_ptr<BoronGuiBackends::Backends> m_backend;

	static std::vector<Borongui::Widget*> widgets;
	bool m_inited = false;
};