#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <initializer_list>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

#include <stb_truetype.h>

#include "render/texture/Texture.h"

namespace eng {

	using FontInfo = stbtt_fontinfo;

	struct FontData;

	struct Glyph {
		static constexpr int16_t padding { 1 };

		FontData* const font;
		const char32_t codepoint;
		const int32_t index;
		const glm::i16vec2 sdfSize; // size of the sdf created by stb_truetype
		const glm::i16vec2 offset; // offset from the origin of the sdf to the top left corner of the glyph
		const int16_t xAdvance;
		const int16_t leftSideBearing;
		const glm::i16vec2 uvMin, uvMax; // min and max uv coords of the sdf on the atlas

		Glyph(FontData* const font, char32_t codepoint, int32_t index,
			  const glm::i16vec2& sdfSize, const glm::i16vec2& offset,
			  int16_t xAdvance, int16_t leftSideBearing,
			  const glm::i16vec2& uvMin, const glm::i16vec2& uvMax) noexcept :
				font(font), codepoint(codepoint), index(index),
				sdfSize(sdfSize), offset(offset),
				xAdvance(xAdvance), leftSideBearing(leftSideBearing),
				uvMin(uvMin), uvMax(uvMax) {}

		glm::vec2 getMinUV() const;
		glm::vec2 getMaxUV() const;
	};

	class Font;

	struct [[nodiscard]] FontData {
		Font* font = nullptr;
		std::vector<unsigned char> data;
		FontInfo fontInfo;
		float fontScale;
		float ascent, descent, lineGap;
		float lineHeight;

		[[nodiscard]] FontData(std::vector<unsigned char>&& data, int offset = 0);
	};


	class FontRenderer;

	class [[nodiscard]] Font {
		friend class FontRenderer;
	public:
		static constexpr size_t initialAtlasSize = 1024;
	private:

		FontData fontData;
		std::unordered_map<char32_t, Glyph> glyphs;
		Texture texture {};

		std::vector<FontData> fallbacks;

		[[nodiscard]] Font(const FontData&, const std::vector<FontData>& = {});
		[[nodiscard]] Font(FontData&&, std::vector<FontData>&& = {});

		void initFont();

	public:

		[[nodiscard]] static Font loadFont(const std::string& filePath, ::std::initializer_list<std::string> = {});

		Font(const Font&) = delete;
		[[nodiscard]] Font(Font&&) = default;
		Font& operator =(const Font&) = delete;
		Font& operator =(Font&&) = default;

		// get the glyph for a unicode codepoint
		const Glyph& getGlyph(const char32_t codepoint) const;

		float getKernAdvance(const Glyph& glyph1, const Glyph& glyph2) const;

		const Texture& getTexture() const noexcept;

		//void addFallback(std::string_view fallbackPath); // TODO: add?

	private:

		void initGlyphs();


	};

}