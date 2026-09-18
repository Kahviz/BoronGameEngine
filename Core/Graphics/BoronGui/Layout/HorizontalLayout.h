#pragma	once

#include "Widgets/Widget.h"

namespace Borongui {
class HorizontalLayout {
public:
	void setOwner(Borongui::Widget& p_owner);
	void add(Borongui::Widget& p_widget);
	void setPadding(const float& p_padding);
	void setSpacing(const float& p_spacing);
	void update();
private:
	bool m_inited = false;
	Borongui::Widget* m_owner{};
	std::vector<Borongui::Widget*> m_childs{};

	float m_padding = 0.0f;
	float m_spacing = 10.0f;
};
};