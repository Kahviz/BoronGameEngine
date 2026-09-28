#pragma once

#include <stdint.h>
#include <vector>
#include "Texture.h"

class GuiTextureManager {
public:
	static const uint32_t& getCurrentTextureID();
	static std::vector<Texture>& getTextures();
	static uint32_t genNewTextureID();
private:
	static uint32_t s_currentTextureID;
	static std::vector<Texture> s_textures;
};