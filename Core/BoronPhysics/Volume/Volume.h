#pragma once

#include "BoronMathLibrary.h"

#include "Enums.h"

class Volume {
public:
	Volume() = default;
	virtual ~Volume() = default;

	virtual const Boron::Enums::VolumeType& getType() const = 0;
	virtual bool contains(const BML::Vec3& point) const = 0;
	virtual bool intersects(const Volume& other) const = 0;

	BML::Vec3 m_pos = { 0,0,0 };
};