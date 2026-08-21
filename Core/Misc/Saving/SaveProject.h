#pragma once
#include <vector>
#include <memory>
#include "Window/Window.h"
#include "ECS.h"
class Instance;

class SaveProject {
public:
	static void Save(ECS& ecs);
	static void Load(ECS& ecs, Window& window, EntityECS world);
private:

};