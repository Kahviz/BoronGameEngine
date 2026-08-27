#pragma once

#include "ECS.h"

class Physics {
public:
	bool ApplyPhysics(ECS& ecs,EntityECS entity,float deltatime);
private:
};