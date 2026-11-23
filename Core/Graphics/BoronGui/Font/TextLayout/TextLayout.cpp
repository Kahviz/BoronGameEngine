#include "TextLayout.h"

#include "Logger/Logger.h"

bool Borongui::TextLayout::Build(const std::string& p_text, const Borongui::Font& p_font) {
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
		const Glyph& glyph = p_font.getGlyph(character);

		std::cout << "Character: " << character << '\n';
		std::cout << "Width: " << glyph.size.x() << '\n';
		std::cout << "Height: " << glyph.size.y() << '\n';
	}
}