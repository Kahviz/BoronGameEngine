#pragma once
#include "Widgets/Widgets.h"

namespace BoronGui {
class BoronGui {
public:
	static void InitBoronGui();
private:
	bool m_inited = false;
};
}