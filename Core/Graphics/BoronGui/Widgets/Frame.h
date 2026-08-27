#pragma once
#include "BoronMathLibrary.h"

namespace BoronGui {
class Frame {
public:
	bool setSize(const BML::Vec2& size);
	bool setPosition(const BML::Vec2& position);
private:
	BML::Vec2 m_size = { 0,0 };
	BML::Vec2 m_position = { 0,0 };
};
}