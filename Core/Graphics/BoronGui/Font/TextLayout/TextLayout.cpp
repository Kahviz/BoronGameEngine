#include "TextLayout.h"

#include "Logger/Logger.h"

bool Borongui::TextLayout::Build(const std::string& p_text, const Borongui::Font& p_font, const int& p_lineHeight) {
	if (!p_font.isLoaded()) {
		CreateError("Font has to be loadded before building text");
	}

	if (p_text.empty()) {
		return true;
	}

	m_mesh.indices.clear();
	m_mesh.vertices.clear();

	float cursorX = 0.0f;
	float cursorY = 0.0f;

    for (char character : p_text) {
        if (character == '\n') {
            cursorX = 0.0f;
            cursorY -= p_lineHeight;
            m_previousCharacter = '\0';
            continue;
        }

        const Glyph& glyph = p_font.getGlyph(character);

        if (m_previousCharacter != '\0') {
            cursorX += p_font.getKerning(m_previousCharacter, character) * p_font.getScale();
        }

        if (character != ' ') {
            float x0 = cursorX + glyph.offset.x();
            float y0 = cursorY - glyph.offset.y();

            float x1 = x0 + glyph.size.x();
            float y1 = y0 + glyph.size.y();

            uint32_t startIndex = static_cast<uint32_t>(m_mesh.vertices.size());

            m_mesh.vertices.push_back({ x0, y0 });
            m_mesh.vertices.push_back({ x1, y0 });
            m_mesh.vertices.push_back({ x1, y1 });
            m_mesh.vertices.push_back({ x0, y1 });

            m_mesh.indices.push_back(startIndex + 0);
            m_mesh.indices.push_back(startIndex + 1);
            m_mesh.indices.push_back(startIndex + 2);

            m_mesh.indices.push_back(startIndex + 2);
            m_mesh.indices.push_back(startIndex + 3);
            m_mesh.indices.push_back(startIndex + 0);
        }

        cursorX += glyph.advanceWidth;
        m_previousCharacter = character;
    }

	return true;
}