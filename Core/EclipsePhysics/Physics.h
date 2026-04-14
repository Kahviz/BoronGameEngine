#pragma once

#include "Instances/Instance.h"
#include "Gravity.h"

class Physics {
public:
	bool ApplyGravity(Instance& inst,float Deltatime);
private:
};