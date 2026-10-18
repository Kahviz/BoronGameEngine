#include "SphereVolume.h"

#include <stdlib.h>

constexpr float EPSILON = 0.0001f; //[ADD] this to mathlib

const Boron::Enums::VolumeType& SphereVolume::getType() {
    return Boron::Enums::VolumeType::Sphere;
}

bool SphereVolume::contains(const BML::Vec3& p_point) const {
    const BML::Vec3 diff = p_point - m_pos;
    const float distSquared = diff.dot(diff);

    return distSquared <= m_radius * m_radius;
}

bool SphereVolume::intersects(const Volume& p_other) const {
    return true;
}
