#pragma once

#include "Volume.h"
#include "Enums.h"

class CubeVolume : public Volume {
public:
    CubeVolume() = default;
    ~CubeVolume() override = default;

    const Boron::Enums::VolumeType& getType() const override;
    bool contains(const BML::Vec3& p_point) const override;
    bool intersects(const Volume& p_other) const override;

    BML::Vec3 m_size{};
    BML::Vec3 m_pos{};
};