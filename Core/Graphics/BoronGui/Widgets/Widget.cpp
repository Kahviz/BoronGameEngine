#include "Widget.h"

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

void Borongui::Widget::lockInitialization(bool p_value) {
	initLocked = p_value;
}

bool Borongui::Widget::setSize(const BML::Vec2& p_size) {
	if (!initLocked) {
		m_size = p_size;
	}

	return true;
}

bool Borongui::Widget::setPosition(const BML::Vec2& p_position) {
	if (!initLocked) {
		m_position = p_position;
	}

	return true;
}

bool Borongui::Widget::setColor(const BML::Vec4& p_color) {
	if (!initLocked) {
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

const BML::Vec2& Borongui::Widget::getPosition() const {
	return m_position;
}

const BML::Vec2& Borongui::Widget::getSize() const {
	return m_size;
}

const BML::Vec4& Borongui::Widget::getColor() const {
	return m_color;
}

const float Borongui::Widget::getRounding() const {
	return m_rounding;
}