#include "SphereVolume.h"

#include <stdlib.h>

constexpr float EPSILON = 0.0001f; //[ADD] this to mathlib

const Boron::Enums::VolumeType& SphereVolume::getType() {
    return Boron::Enums::VolumeType::Sphere;
}

bool SphereVolume::contains(const BML::Vec3& p_point) const {
    const float dx = p_point.x() - m_pos.x();
    const float dy = p_point.y() - m_pos.y();
    const float dz = p_point.z() - m_pos.z();

    float distSquared = dx * dx + dy * dy + dz * dz;

    return distSquared <= m_radius * m_radius;
}

bool SphereVolume::intersects(const Volume& p_other) const {
    return true;
}
