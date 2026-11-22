#define STB_TRUETYPE_IMPLEMENTATION
#include "Font.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include "Logger/Logger.h"

bool Borongui::Font::Load(const std::string& p_path) {
	std::ifstream file(p_path, std::ios::binary);

	if (!file.is_open()) {
		CreateError("Cannot open file: ", p_path, " for loading the font, make sure its there");
		return false;
	}

	file.seekg(0, std::ios::end);

	std::streampos size = file.tellg();

	if (size <= 0) {
		CreateError("Font file is empty: ", p_path);
		return false;
	}

	file.seekg(0, std::ios::beg);

	m_fontData.resize(static_cast<size_t>(size));

	file.read(reinterpret_cast<char*>(m_fontData.data()), static_cast<std::streamsize>(size));

	if (!file) {
		CreateError("Failed to read the font file: ", p_path);
		file.clear();

		return false;
	}

	int fontOffset = stbtt_GetFontOffsetForIndex(m_fontData.data(), 0);

	if (fontOffset < 0) {
		CreateError("Font offset cant be: ",fontOffset);
		return false;
	}

	if (!stbtt_InitFont(&m_fontInfo, m_fontData.data(), fontOffset)) {
		CreateError("Failed to init font");
		return false;
	}

	float scale = stbtt_ScaleForPixelHeight(&m_fontInfo, m_fontSize);

	int ascent = 0;
	int descent = 0;
	int lineGap = 0;

	stbtt_GetFontVMetrics(&m_fontInfo, &ascent, &descent, &lineGap);

	float ascentPx = ascent * scale;
	float descentPx = descent * scale;
	float lineGapPx = lineGap * scale;

	for (int character = 32; character < 127; character++) {
		int advanceWidth = 0;
		int leftSideBearing = 0;

		int width = 0;
		int height = 0;
		int xOffset = 0;
		int yOffset = 0;

		stbtt_GetCodepointHMetrics(&m_fontInfo, character, &advanceWidth, &leftSideBearing);

		unsigned char* bitmap = stbtt_GetCodepointBitmap(&m_fontInfo, scale, scale, character, &width, &height, &xOffset, &yOffset);

		Glyph glyph{};
		
		if (width > 0 && height > 0) {
			if (!bitmap) {
				CreateError("Bitmap is nullptr for character: ", character);
				return false;
			}

			glyph.bitmap.assign(bitmap, bitmap + (width * height));
		}
		
		glyph.advanceWidth = advanceWidth * scale;
		glyph.bitmap.assign(bitmap, bitmap + (width * height));
		glyph.size = BML::Int2{ width,height };
		glyph.leftSideBearing = leftSideBearing * scale;
		glyph.offset = BML::Int2{ xOffset,yOffset };

		m_glyphs.push_back(glyph);

		stbtt_FreeBitmap(bitmap, nullptr);
	}

	m_atlasPixels.resize(m_atlasSize.x * m_atlasSize.y, 0);

	int currentX = 0;
	int currentY = 0;
	int rowHeight = 0;

	for (Glyph& glyph : m_glyphs) {
		if (currentX + glyph.size.x() > m_atlasSize.x) {
			currentX = 0;
			currentY += rowHeight + 1;
			rowHeight = 0;
		}

		if (currentY + glyph.size.y() > m_atlasSize.y) {
			CreateError("Font atlas is too small increase m_atlasWidth or m_atlasHeight");
			return false;
		}
		glyph.atlasPos = BML::Int2{ currentX,currentY };

		currentX += glyph.size.x() + 1;
		rowHeight = std::max(rowHeight, glyph.size.y());
	}

	for (const Glyph& glyph : m_glyphs) {
		for (int y = 0; y < glyph.size.y(); y++) {
			for (int x = 0; x < glyph.size.x(); x++) {
				int sourceIndex = y * glyph.size.x() + x;

				int destinationIndex = (glyph.atlasPos.y() + y) * m_atlasSize.x + (glyph.atlasPos.x() + x);

				m_atlasPixels[destinationIndex] = glyph.bitmap[sourceIndex];
			}
		}
	}

	const float invAtlasWidth = 1.0f / m_atlasSize.x;
	const float invAtlasHeight = 1.0f / m_atlasSize.x;

	for (Glyph& glyph : m_glyphs) {
		glyph.uv1.set(glyph.atlasPos.x() * invAtlasWidth, glyph.atlasPos.y() * invAtlasHeight);
		glyph.uv2.set((glyph.atlasPos.x() + glyph.size.x()) * invAtlasWidth, (glyph.atlasPos.y() + glyph.size.y()) * invAtlasHeight);
	}

    m_isLoaded = true;
	return true;
}

void Borongui::Font::Destroy() {
	m_glyphs.clear();

	m_fontData = {};
	m_fontInfo = {};
	m_isLoaded = false;
}

bool Borongui::Font::isLoaded() const {
	return m_isLoaded;
}

const std::vector<Borongui::Glyph>& Borongui::Font::getGlyphs() {
	return m_glyphs;
}

const std::vector<unsigned char>& Borongui::Font::getFontData() {
	return m_fontData;
}

const std::vector<unsigned char>& Borongui::Font::getAtlasPixels() {
	return m_atlasPixels;
}

const stbtt_fontinfo& Borongui::Font::getFontInfo() {
	return m_fontInfo;
}

const int& Borongui::Font::getFontSize() {
	return m_fontSize;
}

const GPUVector2& Borongui::Font::getAtlasSize(){
	return m_atlasSize;
}