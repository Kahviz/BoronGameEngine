#pragma once

#include <BoronMath/Boronmath.h>

namespace FrustumSpace {
    struct Plane {
        BML::Vec3 normal{};
        float distance{};
    };

    struct Frustum {
        Plane planes[6];
    } m_frustrum;
}

class Frustum {
public:
    FrustumSpace::Frustum& getFrustrum();
private:
    
};