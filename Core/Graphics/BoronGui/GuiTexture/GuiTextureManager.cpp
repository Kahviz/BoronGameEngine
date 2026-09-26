#include "GuiTextureManager.h"
#include "Logger/Logger.h"

uint32_t GuiTextureManager::s_currentTextureID = 0;

[[nodiscard]]
const uint32_t& GuiTextureManager::getCurrentTextureID() {
    return s_currentTextureID;
}

[[nodiscard]]
uint32_t GuiTextureManager::genNewTextureID() {
    if (s_currentTextureID == UINT32_MAX) [[unlikely]] {
        CreateError("textureID is too big! ( 4,294,967,294 ), how did we get here.");
        return UINT32_MAX;
    }

    return  s_currentTextureID++;
}