#pragma once

#include "ECS.h"

class Physics {
public:
	bool ApplyPhysics(ECS& p_ecs,EntityECS p_entity,float p_deltatime);
private:
};