#include "Frustrum.h"

#include "Camera.h"

[[nodiscard]] const FrustumSpace::Frustum& Frustum::getFrustrum() {
	return FrustumSpace::m_frustrum;
}

void Frustum::setFrustrum(const FrustumSpace::Frustum& p_frustrum) {
	FrustumSpace::m_frustrum = p_frustrum;
}

void createFrustumFromCamera(const Camera& p_camera, float p_aspect, float p_fovY, float p_zNear, float p_zFar) {
    const float halfVSide = p_zFar * tanf(fovY * 0.5f);
    const float halfHSide = halfVSide * p_aspect;
    const BML::Vec3 frontMultFar = p_zFar * p_camera.GetForward();
    const BML::Vec3 camPos = p_camera.GetPositionVector3();
    
    m_frustum.nearFace = { camPos + p_zNear * p_camera.GetForward(), p_camera.GetForward()};
    m_frustum.farFace = { camPos + frontMultFar, -p_camera.GetForward() };
    m_frustum.rightFace = { camPos, BML::Vec3::cross(frontMultFar - p_camera.GetRight() * halfHSide, p_camera.GetUp())};
    m_frustum.leftFace = { camPos, BML::Vec3::cross(p_camera.GetUp(), frontMultFar + p_camera.GetRight() * halfHSide) };
    m_frustum.topFace = { camPos, BML::Vec3::cross(p_camera.GetRight(), frontMultFar - p_camera.GetUp() * halfVSide) };
    m_frustum.bottomFace = { camPos, BML::Vec3::cross(frontMultFar + p_camera.GetUp() * halfVSide, p_camera.GetRight())};
}