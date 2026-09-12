#pragma once

#include "BoronMathLibrary.h"
#include "Widget.h"
#include "Vertex2d.h"
#include "BoronGuiSettings.h"

namespace Borongui {
class Frame : public Widget {
public:
	Frame() {
		m_rounding = Borongui::frameRounding;
	}

	void updateSettings() override;
private:
	
};
}