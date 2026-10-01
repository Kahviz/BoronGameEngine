#pragma once
#include <vector>
#include <memory>
#include "Window/Window.h"
#include "ECS.h"

class SaveProject {
public:
	static void Save(ECS& p_ecs);
	static void Load(ECS& p_ecs, Window& p_window, EntityECS p_world);
private:

};