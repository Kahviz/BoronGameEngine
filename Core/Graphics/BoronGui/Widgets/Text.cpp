#include "Text.h"

#include "TextLayout/TextLayout.h"

void Borongui::Text::updateSettings() {
	
}

void Borongui::Text::setText(const std::string& p_text, const Borongui::Font p_font) {
	m_text = p_text;

	m_textLayout.Build(p_text, p_font, m_lineHeight);
}

void Borongui::Text::setLineHeight(const int& p_lineHeight) {
	if (p_lineHeight <= 0) {
		CreateError("Lineheight cannot be 0 or less then 0");
	}

	m_lineHeight = p_lineHeight;
}