#pragma once
#include <vector>
#include <memory>
#include "Window/Window.h"

class Instance;

class SaveProject {
public:
	static void Save(const std::vector<std::unique_ptr<Instance>>& Drawables);
	static std::vector<std::unique_ptr<Instance>> Load(Window& window);
private:

};