#include "GuiTextureManager.h"
#include "Logger/Logger.h"

uint32_t GuiTextureManager::s_currentTextureID = 1;
std::vector<Texture> GuiTextureManager::s_textures{};

[[nodiscard]]
const uint32_t& GuiTextureManager::getCurrentTextureID() {
    return s_currentTextureID;
}

std::vector<Texture>& GuiTextureManager::getTextures() {
    return s_textures;
}

[[nodiscard]]
uint32_t GuiTextureManager::genNewTextureID() {
    if (s_currentTextureID == UINT32_MAX) [[unlikely]] {
        CreateError("textureID is too big! ( 4,294,967,294 ), how did we get here.");
        return UINT32_MAX;
    }

    return  s_currentTextureID++;
}

[[nodiscard]]
const size_t& GuiTextureManager::getTextureCount() {
    return s_textures.size();
}