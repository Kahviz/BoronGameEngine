#include "CubeVolume.h"

const Boron::Enums::VolumeType& CubeVolume::getType() const {
	return Boron::Enums::VolumeType::Cube;
}

bool CubeVolume::contains(const BML::Vec3& p_point) const {
    return
        p_point.x >= m_position.x - m_halfExtents.x &&
        p_point.x <= m_position.x + m_halfExtents.x &&
        p_point.y >= m_position.y - m_halfExtents.y &&
        p_point.y <= m_position.y + m_halfExtents.y &&
        p_point.z >= m_position.z - m_halfExtents.z &&
        p_point.z <= m_position.z + m_halfExtents.z; //thx google
}

bool CubeVolume::intersects(const Volume& p_other) const {
    if (p_other.getType() != VolumeType::Cube) {
        return false;
    }

    const CubeVolume& other = static_cast<const CubeVolume&>(p_other);

    return std::abs(m_position.x - other.m_position.x) <= m_halfExtents.x + other.m_halfExtents.x &&
        std::abs(m_position.y - other.m_position.y) <= m_halfExtents.y + other.m_halfExtents.y &&
        std::abs(m_position.z - other.m_position.z) <= m_halfExtents.z + other.m_halfExtents.z;
}