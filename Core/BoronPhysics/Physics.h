#pragma once

#include "Instances/Instance.h"
#include "ECS.h"

class Physics {
public:
	bool ApplyGravity(ECS& ecs,EntityECS entity,float deltatime);
private:
};