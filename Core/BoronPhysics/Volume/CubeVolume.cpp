#include "CubeVolume.h"

const Boron::Enums::VolumeType& CubeVolume::getType() const {

}

bool CubeVolume::contains(const BML::Vec3& p_point) const {
	return false;
}

bool CubeVolume::intersects(const Volume& p_other) const {
	return false;
}
