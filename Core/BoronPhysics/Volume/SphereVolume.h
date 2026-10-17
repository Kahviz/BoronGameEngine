#pragma once

#include "Volume.h"
#include "Enums.h"

class SphereVolume : public Volume {
public:
    SphereVolume() = default;
    ~SphereVolume() override = default;

    const Boron::Enums::VolumeType& getType() override;
    bool contains(const BML::Vec3& p_point) const override;
    bool intersects(const Volume& p_other) const override;

    float m_radius = 10.0f;
};