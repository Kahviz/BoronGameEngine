#pragma once

namespace Borongui {
class Widget {
public:
    virtual ~Widget() = default;

    BML::Vec2 m_size = { 0,0 };
    BML::Vec2 m_position = { 0,0 };
};
}