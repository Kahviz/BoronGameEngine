#include "Text.h"

#include "TextLayout/TextLayout.h"

void Borongui::Text::updateSettings() {
	
}

void Borongui::Text::setText(const std::string& p_text, const Borongui::Font p_font) {
	m_text = p_text;

	m_textLayout.Build(p_text, p_font);
}