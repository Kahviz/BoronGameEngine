#pragma once
#include "BoronMathLibrary.h"
#include "Widget.h"

namespace Borongui {
class Frame : public Widget {
public:
	void Render() override;
	bool setSize(const BML::Vec2& p_size);
	bool setPosition(const BML::Vec2& p_position);
private:
	BML::Vec2 m_size = { 0,0 };
	BML::Vec2 m_position = { 0,0 };
};
}