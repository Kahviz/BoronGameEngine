#pragma once

#include <string>
#include <vector>

#include <stb_truetype.h>

class Font {
public:
    bool Load(const std::string& p_path);
    void Destroy();

    bool isLoaded() const;
private:
    std::vector<unsigned char> m_fontData;
    bool m_isLoaded = false;

    stbtt_fontinfo m_fontInfo{};
};