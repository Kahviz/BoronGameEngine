#include "Frame.h"
#include "BoronGui.h"
#include "GLOBALS.h"

void Borongui::Frame::Render() {

}

bool Borongui::Frame::setSize(const BML::Vec2& p_size) {
	m_size = p_size;

	return true;
}

bool Borongui::Frame::setPosition(const BML::Vec2& p_position) {
	m_position = p_position;

	return true;
}