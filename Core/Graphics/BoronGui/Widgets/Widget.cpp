#include "Widget.h"

int Borongui::Widget::currentzIndex = 0;

Borongui::Widget::Widget() {
	currentzIndex++;
	m_zIndex = currentzIndex;
}

const bool Borongui::Widget::isClicked() const {
	return m_isClicked;
}

const bool Borongui::Widget::isHovered() const {
    return m_isHovered;
}

const std::vector<uint32_t>& Borongui::Widget::getIndices() const {
	return m_indices;
}

std::vector<Vertex2d>& Borongui::Widget::getVertices() {
	return m_vertices;
}

const std::vector<Vertex2d>& Borongui::Widget::getConstVertices() const {
	return m_vertices;
}

void Borongui::Widget::setIndices(const std::vector<uint32_t>& p_indices) {
	m_indices = p_indices;
}

void Borongui::Widget::setVertices(const std::vector<Vertex2d>& p_vertices) {
	m_vertices = p_vertices;
}

void Borongui::Widget::lockInitialization(const bool& p_value) {
	m_initLocked = p_value;
}

void Borongui::Widget::setZIndex(const int& p_value) {
	m_zIndex = p_value;
}

bool Borongui::Widget::setSize(const BML::Vec2& p_size, const bool& p_force = false) {
	if (!m_initLocked || p_force) {
		m_size = p_size;
	}

	return true;
}

bool Borongui::Widget::setPosition(const BML::Vec2& p_position, const bool& p_force = false) {
	if (!m_initLocked || p_force) {
		m_position = p_position;
	}

	return true;
}

bool Borongui::Widget::setLocalPosition(const BML::Vec2& p_position, const bool& p_force = false) {
	if (m_parent == nullptr) {
		return false;
	}

	if (!m_initLocked || p_force) {
		m_position = p_position + m_parent->getPosition();
	}

	return true;
}
bool Borongui::Widget::setLocalPositionX(const float& p_x, const bool& p_force) {
	if (m_parent == nullptr) {
		return false;
	}

	if (!m_initLocked || p_force) {
		m_position.x() = p_x + m_parent->getPosition().x();
	}

	return true;
}

bool Borongui::Widget::setLocalPositionY(const float& p_y, const bool& p_force) {
	if (m_parent == nullptr) {
		return false;
	}

	if (!m_initLocked || p_force) {
		m_position.y() = p_y + m_parent->getPosition().y();
	}

	return true;
}

bool Borongui::Widget::setColor(const BML::Vec4& p_color, const bool& p_force) {
	if (!m_initLocked || p_force) {
		m_color = p_color;

		GPUVector4 color(
			m_color.x(),
			m_color.y(),
			m_color.z(),
			m_color.w()
		);

		for (auto& vertex : m_vertices) {
			vertex.color = color;
		}
	}

	return true;
}

bool Borongui::Widget::setClickColor(const BML::Vec4& p_color) {
	m_clickColor = p_color;
	return true;
}

bool Borongui::Widget::setHoverColor(const BML::Vec4& p_color) {
	m_hoverColor = p_color;
	return true;
}

const BML::Vec2& Borongui::Widget::getPosition() const {
	return m_position;
}

const BML::Vec2& Borongui::Widget::getSize() const {
	return m_size;
}

const BML::Vec4& Borongui::Widget::getColor() const {
	return m_color;
}

const BML::Vec4& Borongui::Widget::getClickColor() const {
	return m_clickColor;
}

const BML::Vec4& Borongui::Widget::getHoverColor() const {
	return m_hoverColor;
}

const float Borongui::Widget::getRounding() const {
	return m_rounding;
}

int Borongui::Widget::getZIndex() {
	return m_zIndex;
}

std::vector<Borongui::Widget*>& Borongui::Widget::getChildren() {
	return m_children;
}

const std::vector<Borongui::Widget*>& Borongui::Widget::getConstChildren() const {
	return m_children;
}

void Borongui::Widget::setParent(Borongui::Widget& p_parent) {
	if (!m_initLocked) {
		p_parent.m_children.push_back(this);
		m_parent = &p_parent;
	}
}