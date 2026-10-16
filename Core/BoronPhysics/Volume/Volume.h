#pragma once

#include "Enums.h"

class Volume {
public:
	Volume() = default;
	virtual ~Volume() = default;

	virtual const Boron::Enums::VolumeType& getType() = 0;
	virtual const bool& contains() = 0;
	virtual const bool& intersects() = 0;
};