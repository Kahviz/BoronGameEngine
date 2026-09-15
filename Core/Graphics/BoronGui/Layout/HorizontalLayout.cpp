#include "HorizontalLayout.h"

#include "BGE_ASSERTS.h"

void Borongui::HorizontalLayout::setOwner(Borongui::Widget& p_owner) {
	BGE_ASSERT(&p_owner != nullptr, "p_owner cannot be nullptr!");

	m_owner = &p_owner;

	m_inited = true;
}

void Borongui::HorizontalLayout::update() {
	float padding = m_startingPoint;

	for (auto& child : m_childs) {
		child->m_canDrag = false;

		padding += m_padding;

		float x = m_owner->getSize().x() / 2.0f - padding;

		child->setLocalPositionX(x, true);
	}

	m_childs.clear();
}

void Borongui::HorizontalLayout::add(Borongui::Widget& p_widget) {
	BGE_ASSERT(m_inited, "You should setOwner(), before calling add()");
	BGE_ASSERT(&p_widget != nullptr, "p_widget cannot be nullptr!");

	m_childs.push_back(&p_widget);
}

void Borongui::HorizontalLayout::setPadding(const float& p_padding) {
	m_padding = p_padding;
}

void Borongui::HorizontalLayout::setStartingPoint(const float& p_startingPoint) {
	m_startingPoint = p_startingPoint;
}