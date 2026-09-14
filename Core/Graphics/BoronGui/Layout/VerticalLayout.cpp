#include "VerticalLayout.h"

#include "BGE_ASSERTS.h"

void Borongui::VerticalLayout::setOwner(Borongui::Widget& p_owner) {
	BGE_ASSERT(&p_owner != nullptr, "p_owner cannot be nullptr!");

	m_owner = &p_owner;

	m_inited = true;
}

void Borongui::VerticalLayout::update() {
	float padding = m_startingPoint;

	for (auto& child : m_childs) {
		child->m_canDrag = false;

		padding += m_padding;

		float y = m_owner->getSize().y() / 2.0f - padding;

		child->setLocalPositionY(y, true);
	}

	m_childs.clear();
}

void Borongui::VerticalLayout::add(Borongui::Widget& p_widget) {
	BGE_ASSERT(m_inited, "You should setOwner(), before calling add()");
	BGE_ASSERT(&p_widget != nullptr, "p_widget cannot be nullptr!");

	m_childs.push_back(&p_widget);
}

void Borongui::VerticalLayout::setPadding(const float& p_padding) {
	m_padding = p_padding;
}

void Borongui::VerticalLayout::setStartingPoint(const float& p_startingPoint) {
	m_startingPoint = p_startingPoint;
}
