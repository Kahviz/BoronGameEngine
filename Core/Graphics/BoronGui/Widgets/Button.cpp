#include "Button.h"
#include "BoronGui.h"
#include "GLOBALS.h"

void Borongui::Button::lockInitialization(bool p_value) {
	initLocked = p_value;
}

bool Borongui::Button::setSize(const BML::Vec2& p_size) {
	if (!initLocked) {
		m_size = p_size;
	}

	return true;
}

bool Borongui::Button::setPosition(const BML::Vec2& p_position) {
	if (!initLocked) {
		m_position = p_position;
	}

	return true;
}

bool Borongui::Button::setColor(const BML::Vec4& p_color) {
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

const BML::Vec2& Borongui::Button::getPosition() const {
	return m_position;
}

const BML::Vec2& Borongui::Button::getSize() const {
	return m_size;
}

const BML::Vec4& Borongui::Button::getColor() const {
	return m_color;
}

const float Borongui::Button::getRounding() const {
	return m_rounding;
}

const std::vector<uint32_t>& Borongui::Button::getIndices() const {
	return m_indices;
}

std::vector<Vertex2d>& Borongui::Button::getVertices() {
	return m_vertices;
}

const std::vector<Vertex2d>& Borongui::Button::getConstVertices() const {
	return m_vertices;
}

void Borongui::Button::setIndices(const std::vector<uint32_t>& p_indices) {
	m_indices = p_indices;
}

void Borongui::Button::setVertices(const std::vector<Vertex2d>& p_vertices) {
	m_vertices = p_vertices;
}
