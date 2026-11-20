#include "Font.h"

#include <fstream>
#include <vector>
#include <string>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "Logger/Logger.h"

bool Font::Load(const std::string& p_path) {
	std::ifstream file(p_path, std::ios::binary);

	if (!file.is_open()) {
		CreateError("Cannot open file: ", p_path, " for loading the font");
		return false;
	}

	file.seekg(file.end);

	std::streampos size = file.tellg();

	file.seekg(0, std::ios::beg);

	m_fontData.resize(size);

	file.read(reinterpret_cast<char*>(m_fontData.data()), size);

	if (!file) {
		CreateError("Failed to read the font file: ", p_path);
		file.clear();

		return false;
	}

	if (!stbtt_InitFont(&m_fontInfo, m_fontData.data(), stbtt_GetFontOffsetForIndex(m_fontData.data(), 0))) {
		CreateError("Failed to initialize font: ", p_path);
		m_fontData.clear();

		return false;
	}

    m_isLoaded = true;
	return true;
}

void Font::Destroy() {
	m_fontData = {};
	m_isLoaded = false;
}

bool Font::isLoaded() const {
	return m_isLoaded;
}