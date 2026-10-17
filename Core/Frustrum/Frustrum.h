#pragma once

#include "BoronMath/Boronmath.h"
#include "BoronPhysics/Volume/SphereVolume.h"
#include "Camera/Camera.h"

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
    };

    inline Frustum m_frustrum{};
}

class Frustum {
public:
    const FrustumSpace::Frustum& getFrustrum();
    void setFrustrum(const FrustumSpace::Frustum& p_frustrum);
    void createFrustumFromCamera(const Camera& p_camera, float p_aspect, float p_fovY, float p_zNear, float p_zFar);
    bool intersects(const SphereVolume& p_sphere) const;
private:
    
};