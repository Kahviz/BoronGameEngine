#pragma once
#include <string>
#include <filesystem>
#include "ErrorHandling/ErrorMessage.h"
class Texture {
public:
	bool Load(std::string path);
	//void UnLoad();
private:
};