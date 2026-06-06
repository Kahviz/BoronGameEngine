#pragma once
#include "BoronMathLibrary.h"
#include "Instances/Instance.h"

class Instance;

class Raycasting {
public:
    bool IsClicked(Instance* inst, const BML::Vector3& rayOrigin, const BML::Vector3& rayDir);
    bool RayIntersectsTriangle(const BML::Vector3& rayOrigin, const BML::Vector3& rayDir, const BML::Vector3& v0, const BML::Vector3& v1, const BML::Vector3& v2, float& tOut);
};