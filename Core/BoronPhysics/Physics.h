#pragma once

#include "Instances/Instance.h"
#include "ECS.h"

class Physics {
public:
	bool ApplyGravity(Instance& inst,float deltatime);
private:
};