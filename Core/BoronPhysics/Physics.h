#pragma once

#include "ECS.h"

class Physics {
public:
	bool ApplyGravity(ECS& ecs,EntityECS entity,float deltatime);
private:
};