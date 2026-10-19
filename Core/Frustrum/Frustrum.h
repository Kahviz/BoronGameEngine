#pragma once

#include "BoronMath/Boronmath.h"
#include "BoronPhysics/Volume/SphereVolume.h"
#include "Camera/Camera.h"
#include <iostream>

namespace FrustumSpace {
    struct Plane {
        BML::Vec3 normal{};
        float distance{};

        Plane() = default;

        Plane(BML::Vec3 p_normal, float p_distance)
            : normal(p_normal), distance(p_distance) {
        }
    };

    struct Frustum {
        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;
    };

    inline Frustum m_frustrum{};
}

class Frustum {
public:
    const FrustumSpace::Frustum& getFrustrum() const;
    void setFrustrum(const FrustumSpace::Frustum& p_frustrum);

    void createFrustumFromCamera(const Camera& p_camera, float p_aspect, float p_fovY, float p_zNear, float p_zFar);
    bool intersects(const SphereVolume& p_sphere) const;
};