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

    return std::abs(m_pos.x() - other.m_pos.x()) <= m_pos.x() + other.m_pos.x() / 2 &&
        std::abs(m_pos.y() - other.m_pos.y()) <= m_pos.y() + other.m_pos.y() / 2 &&
        std::abs(m_pos.z() - other.m_pos.z()) <= m_pos.z() + other.m_pos.z() / 2;
}