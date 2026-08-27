#include "Frame.h"

bool BoronGui::Frame::setSize(const BML::Vec2& size) {
	m_size = size;

	return true;
}

bool BoronGui::Frame::setPosition(const BML::Vec2& position) {
	m_position = position;

	return true;
}
