#pragma once

#include <stdint.h>

class GuiTextureManager {
public:
	static const uint32_t& getCurrentTextureID();
	static uint32_t genNewTextureID();
private:
	static uint32_t s_currentTextureID;
};