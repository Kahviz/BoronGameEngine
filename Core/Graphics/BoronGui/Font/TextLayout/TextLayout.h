#pragma once

#include <vector>

#include "BoronMathLibrary.h"
#include "Font/Font.h"

namespace Borongui {
    struct TextVertex {
        BML::Vec2 pos{};
        BML::Vec2 uv{};
    };

    struct TextMesh {
        std::vector<TextVertex> vertices{};
        std::vector<uint32_t> indices{};
    };

	class TextLayout {
	public:
        bool Build(const std::string& p_text, const Borongui::Font& p_font);
	private:
        TextMesh m_mesh;
	};
};