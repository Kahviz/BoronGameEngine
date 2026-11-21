#pragma once

#include <string>
#include <vector>

#include <stb_truetype.h>

#include "BoronMathLibrary.h"

namespace Borongui {
    struct Glyph {
        BML::Int2 size = { 0,0 };

        BML::Int2 offset = { 0,0 };

        float advanceWidth = 0.0f;
        float leftSideBearing = 0.0f;

        BML::Int2 atlasPos = { 0,0 };

        //UV
        BML::Vec2 uv1 = { 0,0 };
        BML::Vec2 uv2 = { 0,0 };

        std::vector<unsigned char> bitmap;
    };

    class Font {
    public:
        bool Load(const std::string& p_path);
        void Destroy();

        bool isLoaded() const;
    private:
        std::vector<Glyph> m_glyphs{};
        std::vector<unsigned char> m_fontData{};

        stbtt_fontinfo m_fontInfo{};

        int m_fontSize = 20;
        bool m_isLoaded = false;

        int m_atlasWidth = 512;
        int m_atlasHeight = 512;
    };
};