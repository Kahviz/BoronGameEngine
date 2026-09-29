#pragma once

#include <stdint.h>
#include <vector>
#include "Texture.h"
#include "GLOBALS.h"

class GuiTextureManager {
public:
	virtual ~GuiTextureManager() = default;

	static const uint32_t& getCurrentTextureID();
	static std::vector<Texture>& getTextures();
	static uint32_t genNewTextureID();
	static const size_t& getTextureCount();
private:
	static uint32_t s_currentTextureID;
	static std::vector<Texture> s_textures;
};