#pragma once

#include "BoronMathLibrary.h"

namespace Borongui {
class Widget {
public:
    virtual ~Widget() = default;

    BML::Vec2 m_size = { 0,0 };
    BML::Vec2 m_position = { 0,0 };

    bool m_isHovered = false;
    bool m_isClicked = false;
    bool m_isDragging = false;

    int m_zIndex = 0;
};
}