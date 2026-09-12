#pragma once

#include "BoronMathLibrary.h"
#include "Vertex2d.h"

namespace Borongui {
class Widget {
public:
    virtual ~Widget() = default;
    virtual void updateSettings() = 0;

    const bool isClicked() const;
    const bool isHovered() const;
    
    void lockInitialization(bool p_value);
    bool setSize(const BML::Vec2& p_size);
    bool setPosition(const BML::Vec2& p_position);
    bool setColor(const BML::Vec4& p_color);
    const BML::Vec2& getPosition() const;
    const BML::Vec2& getSize() const;
    const BML::Vec4& getColor() const;
    const float getRounding() const;

    //vertices
    std::vector<Vertex2d>& getVertices();

    const std::vector<Vertex2d>& getConstVertices() const;
    void setVertices(const std::vector<Vertex2d>& p_vertices);

    //indices
    const std::vector<uint32_t>& getIndices() const;

    void setIndices(const std::vector<uint32_t>& p_indices);

    BML::Vec2 m_size = { 0,0 };
    BML::Vec2 m_position = { 0,0 };

    bool m_isHovered = false;
    bool m_isClicked = false;
    bool m_isDragging = false;

    int m_zIndex = 0;

    BML::Vec4 m_color = { 0,0,0,0 };

    bool initLocked = false;

    float m_rounding = 10.0f;
    
    std::vector<Vertex2d> m_vertices = {
        Vertex2d({ -0.5f, -0.5f }),
        Vertex2d({ 0.5f, -0.5f }),
        Vertex2d({ 0.5f, 0.5f }),
        Vertex2d({ -0.5f, 0.5f })
    };

    std::vector<uint32_t> m_indices = {
        0, 1, 2,
        2, 3, 0
    };
};
}