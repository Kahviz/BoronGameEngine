#include "SphereVolume.h"

#include <stdlib.h>

constexpr float EPSILON = 0.0001f; //[ADD] this to mathlib

const Boron::Enums::VolumeType& SphereVolume::getType() const {
    return Boron::Enums::VolumeType::Sphere;
}

bool SphereVolume::contains(const BML::Vec3& p_point) const {
    const BML::Vec3 diff = p_point - m_pos;
    const float distSquared = diff.dot(diff);

    return distSquared <= m_radius * m_radius;
}

bool SphereVolume::intersects(const Volume& p_other) const {
    if (p_other.getType() != Boron::Enums::VolumeType::Sphere) {
        return false;
    }

    const auto& other = static_cast<const SphereVolume&>(p_other);

    const float radiusSum = m_radius + other.m_radius;
    const BML::Vec3 difference = m_pos - other.m_pos;

    return difference.dot(difference) <= radiusSum * radiusSum;
}