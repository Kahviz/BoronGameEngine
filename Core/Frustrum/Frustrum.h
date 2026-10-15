#pragma once

#include <BoronMath/Boronmath.h>

namespace FrustumSpace {
    struct Plane {
        BML::Vec3 normal{};
        float distance{};
    };

    struct Frustum {
        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;
    } m_frustrum;
}

class Frustum {
public:
    const FrustumSpace::Frustum& getFrustrum();
    void setFrustrum(const FrustumSpace::Frustum& p_frustrum);
private:
    
};