#pragma once

#include "BoronMathLibrary.h"
#include "Widget.h"
#include "Vertex2d.h"
#include "BoronGuiSettings.h"

namespace Borongui {
class Button : public Widget {
public:
	Button() {
		m_rounding = Borongui::buttonRounding;
	}
	void updateSettings() override;
private:
	
};
}