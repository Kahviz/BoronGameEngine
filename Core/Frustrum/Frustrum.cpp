#include "Frustrum.h"

#include <cmath>

[[nodiscard]] const FrustumSpace::Frustum& Frustum::getFrustrum() const {
    return FrustumSpace::m_frustrum;
}

void Frustum::setFrustrum(const FrustumSpace::Frustum& p_frustrum) {
    FrustumSpace::m_frustrum = p_frustrum;
}

void Frustum::createFrustumFromCamera(
    const Camera& p_camera,
    float p_aspect,
    float p_fovY,
    float p_zNear,
    float p_zFar)
{
    const BML::Vec3 camPos = p_camera.getPositionVector3();
    const BML::Vec3 forward = p_camera.getForward();
    const BML::Vec3 right = p_camera.getRight();
    const BML::Vec3 up = p_camera.getUp();

    const float halfVSide = p_zFar * std::tanf(p_fovY * 0.5f);
    const float halfHSide = halfVSide * p_aspect;

    const BML::Vec3 frontMultFar = forward * p_zFar;

    const BML::Vec3 nearNormal = forward;
    const BML::Vec3 nearPoint = camPos + forward * p_zNear;

    FrustumSpace::m_frustrum.nearFace = {nearNormal, nearNormal.dot(nearPoint)};

    const BML::Vec3 farNormal = -forward;
    const BML::Vec3 farPoint = camPos + frontMultFar;

    FrustumSpace::m_frustrum.farFace = {
        farNormal,
        farNormal.dot(farPoint)
    };

    BML::Vec3 rightNormal = up.cross(frontMultFar - right * halfHSide);

    rightNormal = rightNormal / rightNormal.length();

    FrustumSpace::m_frustrum.rightFace = {rightNormal, rightNormal.dot(camPos)};

    BML::Vec3 leftNormal = (frontMultFar + right * halfHSide).cross(up);

    leftNormal = leftNormal / leftNormal.length();

    FrustumSpace::m_frustrum.leftFace = {leftNormal, leftNormal.dot(camPos)};

    BML::Vec3 topNormal = right.cross(frontMultFar + up * halfVSide);
    topNormal = topNormal / topNormal.length();

    FrustumSpace::m_frustrum.topFace = { topNormal, topNormal.dot(camPos) };

    BML::Vec3 bottomNormal = (frontMultFar - up * halfVSide).cross(right);
    bottomNormal = bottomNormal / bottomNormal.length();

    FrustumSpace::m_frustrum.bottomFace = { bottomNormal, bottomNormal.dot(camPos) };
}

bool Frustum::intersects(const SphereVolume& p_sphere) const {
    const BML::Vec3 center = p_sphere.m_pos;
    const float radius = p_sphere.m_radius;

    const auto& frustum = FrustumSpace::m_frustrum;

    const FrustumSpace::Plane* planes[] = {
        &frustum.topFace,
        &frustum.bottomFace,
        &frustum.rightFace,
        &frustum.leftFace,
        &frustum.farFace,
        &frustum.nearFace
    };

    for (const auto* plane : planes) {
        const float distance = plane->normal.dot(center) - plane->distance;

        if (distance < -radius) {
            return false;
        }
    }

    return true;
}

bool Frustum::intersects(const CubeVolume& p_cube) const {
    const BML::Vec3 center = p_cube.m_pos;
    const auto& size = p_cube.m_size;

    const auto& frustum = FrustumSpace::m_frustrum;

    const FrustumSpace::Plane* planes[] = {
        &frustum.topFace,
        &frustum.bottomFace,
        &frustum.rightFace,
        &frustum.leftFace,
        &frustum.farFace,
        &frustum.nearFace
    };

    for (const auto* plane : planes) {
        const float distance = plane->normal.dot(center) - plane->distance;

        const float radius =
            std::abs(plane->normal.x()) * size.x() / 2.0f +
            std::abs(plane->normal.y()) * size.y() / 2.0f +
            std::abs(plane->normal.z()) * size.z() / 2.0f;

        if (distance < -radius) {
            return false;
        }
    }

    return true;
}