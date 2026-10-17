#include "Frustrum.h"

[[nodiscard]] const FrustumSpace::Frustum& Frustum::getFrustrum() {
	return FrustumSpace::m_frustrum;
}

void Frustum::setFrustrum(const FrustumSpace::Frustum& p_frustrum) {
	FrustumSpace::m_frustrum = p_frustrum;
}

void Frustum::createFrustumFromCamera(const Camera& p_camera, float p_aspect, float p_fovY, float p_zNear, float p_zFar) {
    const float halfVSide = p_zFar * tanf(p_fovY * 0.5f);
    const float halfHSide = halfVSide * p_aspect;
    const BML::Vec3 frontMultFar = p_zFar * p_camera.getForward();
    const BML::Vec3 camPos = p_camera.getPositionVector3();
    
    FrustumSpace::m_frustrum.nearFace = { camPos + p_zNear * p_camera.getForward(), p_camera.getForward()};
    FrustumSpace::m_frustrum.farFace = { camPos + frontMultFar, -p_camera.getForward() };
    FrustumSpace::m_frustrum.rightFace = { camPos, BML::Vec3::cross(frontMultFar - p_camera.getRight() * halfHSide, p_camera.getUp())};
    FrustumSpace::m_frustrum.leftFace = { camPos, BML::Vec3::cross(p_camera.getUp(), frontMultFar + p_camera.getRight() * halfHSide) };
    FrustumSpace::m_frustrum.topFace = { camPos, BML::Vec3::cross(p_camera.getRight(), frontMultFar - p_camera.getUp() * halfVSide) };
    FrustumSpace::m_frustrum.bottomFace = { camPos, BML::Vec3::cross(frontMultFar + p_camera.getUp() * halfVSide, p_camera.getRight())};
}

bool Frustum::intersects(const SphereVolume& p_sphere) const {
    const auto& frustum = getFrustrum();
    const BML::Vec3 center = p_sphere.m_pos;
    const float radius = p_sphere.m_radius;

    for (const auto& plane : frustum.planes) {
        float distance = BML::Vec3::dot(plane.normal, center - plane.point);

        if (distance < -radius) {
            return false;
        }
    }

    return true;
}