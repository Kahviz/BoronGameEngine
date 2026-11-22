#pragma once

#include "Widget.h"

#include "TextLayout/TextLayout.h"

namespace Borongui {
	class Text : public Widget {
	public:
		Text() {

		}

		void updateSettings() override;
		void setText(const std::string& p_text, const Borongui::Font p_font);
	private:
		std::string m_text{};
		Borongui::TextLayout m_textLayout{};
	};
}