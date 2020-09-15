#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/fwd.hpp>
#include <glm/common.hpp>

#include "render/VertexArray.h"
#include "render/VertexBuffer.h"
#include "render/ShaderProgram.h"
#include "FontQuad.h"
#include "render/ui/UIVert.h"
#include "Font.h"
#include "util/Color.h"
#include "util/text/unicode_utils.h"

namespace eng {

	enum class TextAlign {
		Left, Center, Right
	};

	struct TextOutline {
		float size = 0.0f;
		float spread = 0.0f;
		Color color { 0, 0, 0, 0 };
	};

	struct TextWrapResult {
		struct Line {
			size_t startPos, endPos;
			std::u32string_view text;
			float width;
		};
		float width; // the width of the area the text was drawn in
		float height; // the height of the area the text was drawn in
		size_t startPos;
		size_t endPos; // the index of the first character of the input string that was unable to fit within the area
		std::vector<Line> lines;
	};

	class Renderer;

	class FontRenderer {
	public:
		static constexpr float sdf_size = 32.0f;
		static constexpr int16_t sdf_padding = 6;
		static constexpr unsigned char sdf_on_edge_value = 180;
		static constexpr float sdf_pixel_dist_scale = 30.0f;

		struct MultilineConfig {
			glm::vec2 area;
			size_t maxLines = 0;
			TextAlign textAlign = TextAlign::Left;

			explicit MultilineConfig(const glm::vec2& area) noexcept :
					area(area), maxLines(0), textAlign(TextAlign::Left) {}
			explicit MultilineConfig(float maxWidth) noexcept :
					area(maxWidth, 0), maxLines(0), textAlign(TextAlign::Left) {}
			MultilineConfig(const glm::vec2& area, size_t maxLines, TextAlign textAlign = TextAlign::Left) noexcept :
					area(area), maxLines(maxLines), textAlign(textAlign) {}
			MultilineConfig(float maxWidth, size_t maxLines, TextAlign textAlign = TextAlign::Left) noexcept :
					area(maxWidth, 0), maxLines(maxLines), textAlign(textAlign) {}
			MultilineConfig(const glm::vec2& area, TextAlign textAlign) noexcept :
					area(area), maxLines(0), textAlign(textAlign) {}
			MultilineConfig(float maxWidth, TextAlign textAlign) noexcept :
					area(maxWidth, 0), maxLines(0), textAlign(textAlign) {}
		};

		using ColorVert = UIVertColor;

		using FontQuadBuffer = std::vector<FontQuad>;
		using ColorVertBuffer = std::vector<ColorVert>;

		struct TextVertexBuffers {
			FontQuadBuffer fontQuadBuffer;
			ColorVertBuffer underlineBuffer;
			//ColorVertBuffer backgroundBuffer; // TODO: add ability to render text with highlight
		};

	private:

		Renderer* renderer;

		VertexArray fontVAO;
		VertexBuffer fontVBO;
		VertexArray underlineVAO;
		VertexBuffer underlineVBO;

		ShaderProgram fontShader = ShaderProgram::load("text/sdf.vert", "text/sdf.frag", "text/sdf.geom");
		ShaderProgram underlineShader = ShaderProgram::load("text/underline.vert", "text/underline.frag");

		// TODO: get from font config
		//Font font = Font::loadFont("Quivira.otf");
		//Font font = Font::loadFont("Code200365k.ttf");
		Font font = Font::loadFont("Junicode/Junicode.ttf", {
			"Junicode/FoulisGreek.ttf",
			//"Symbola.ttf",
			"Code200365k.ttf",//"Quivira.otf"
		});
		//Font font = Font::loadFont("Symbola.ttf");
		//Font font = Font::loadFont("DejaVu/DejaVuSans.ttf");
		//Font font = Font::loadFont("BabelStoneRunic.ttf");
		//Font font = Font::loadFont("Kelvinch-Roman.otf");
		//Font font = Font::loadFont("LiberationSans-Regular.ttf");

		//FontQuadBuffer fontQuadBuffer;
		//LineBuffer underlineBuffer;
		TextVertexBuffers textVertBuffers;

	public:

		FontRenderer(Renderer* const);

		FontRenderer(const FontRenderer&) = delete;
		FontRenderer(FontRenderer&&) = default;
		FontRenderer& operator =(const FontRenderer&) = delete;
		FontRenderer& operator =(FontRenderer&&) = default;


		void render(const float partialTicks);

		void flush();


		inline float drawText(std::string_view text, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(textVertBuffers, text, origin, fontSize, color, outline, underline);
		}
		inline float drawText(std::u8string_view text, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(textVertBuffers, text, origin, fontSize, color, outline, underline);
		}
		inline float drawText(std::u32string_view text, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(textVertBuffers, text, origin, fontSize, color, outline, underline);
		}
		inline float drawText(std::string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(textVertBuffers, text, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}
		inline float drawText(std::u8string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(textVertBuffers, text, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}
		inline float drawText(std::u32string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(textVertBuffers, text, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}

		inline float drawTextToBuffer(TextVertexBuffers& textBuffers, std::string_view text, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const {
			return drawTextToBuffer(textBuffers, unicode::utf8ToUtf32(text), origin, fontSize, color, outline, underline);
		}
		inline float drawTextToBuffer(TextVertexBuffers& textBuffers, std::u8string_view text, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const {
			return drawTextToBuffer(textBuffers, unicode::utf8ToUtf32(text), origin, fontSize, color, outline, underline);
		}
		float drawTextToBuffer(TextVertexBuffers& textBuffers, std::u32string_view text, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const;
		inline float drawTextToBuffer(TextVertexBuffers& textBuffers, std::string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const {
			return drawTextToBuffer(textBuffers, unicode::utf8ToUtf32(text), glm::vec3(origin, 0), fontSize, color, outline, underline);
		}
		inline float drawTextToBuffer(TextVertexBuffers& textBuffers, std::u8string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const {
			return drawTextToBuffer(textBuffers, unicode::utf8ToUtf32(text), glm::vec3(origin, 0), fontSize, color, outline, underline);
		}
		inline float drawTextToBuffer(TextVertexBuffers& textBuffers, std::u32string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const {
			return drawTextToBuffer(textBuffers, text, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}

		void drawCursor(const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c);
		inline void drawCursor(const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c) {
			drawCursor(glm::vec3(origin, 0), fontSize, color);
		}


		inline float getTextWidth(std::string_view text, const float fontSize) const {
			return getTextWidth(unicode::utf8ToUtf32(text), fontSize);
		}
		inline float getTextWidth(std::u8string_view text, const float fontSize) const {
			return getTextWidth(unicode::utf8ToUtf32(text), fontSize);
		}
		float getTextWidth(std::u32string_view text, const float fontSize) const;

		float getFontSizeForLineHeight(const float lineHeight) const;
		float getLineHeight(const float fontSize) const;

		float getFontAscent(const float fontSize) const;
		float getFontDescent(const float fontSize) const;
		float getFontLineHeight(const float fontSize) const;
		float getFontLineGap(const float fontSize) const;


		TextWrapResult splitLines(std::u32string_view text, float fontSize, const MultilineConfig& multilineConfig, size_t startIndex = 0) const;

		void drawMultilineTextToBuffer(TextVertexBuffers& textBuffers, const TextWrapResult& textWrapRes, const MultilineConfig& multilineConfig, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const;
		inline void drawMultilineTextToBuffer(TextVertexBuffers& textBuffers, const TextWrapResult& textWrapRes, const MultilineConfig& multilineConfig, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, bool underline = false) const {
			drawMultilineTextToBuffer(textBuffers, textWrapRes, multilineConfig, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}

		inline void drawMultilineText(const TextWrapResult& textWrapRes, const MultilineConfig& multilineConfig, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			drawMultilineTextToBuffer(textVertBuffers, textWrapRes, multilineConfig, origin, fontSize, color, outline, underline);
		}
		inline void drawMultilineText(const TextWrapResult& textWrapRes, const MultilineConfig& multilineConfig, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			drawMultilineTextToBuffer(textVertBuffers, textWrapRes, multilineConfig, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}

		TextWrapResult drawMultilineText(std::u32string_view text, size_t startIndex, const MultilineConfig& multilineConfig, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false);
		inline TextWrapResult drawMultilineText(std::u32string_view text, size_t startIndex, const MultilineConfig& multilineConfig, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawMultilineText(text, startIndex, multilineConfig, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}
		inline TextWrapResult drawMultilineText(std::u8string_view text, const MultilineConfig& multilineConfig, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawMultilineText(unicode::utf8ToUtf32(text), 0, multilineConfig, origin, fontSize, color, outline, underline);
		}
		inline TextWrapResult drawMultilineText(std::u8string_view text, const MultilineConfig& multilineConfig, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawMultilineText(unicode::utf8ToUtf32(text), 0, multilineConfig, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}
		inline TextWrapResult drawMultilineText(std::string_view text, const MultilineConfig& multilineConfig, const glm::vec3& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawMultilineText(unicode::utf8ToUtf32(text), 0, multilineConfig, origin, fontSize, color, outline, underline);
		}
		inline TextWrapResult drawMultilineText(std::string_view text, const MultilineConfig& multilineConfig, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const TextOutline& outline = {}, const bool underline = false) {
			return drawMultilineText(unicode::utf8ToUtf32(text), 0, multilineConfig, glm::vec3(origin, 0), fontSize, color, outline, underline);
		}

	protected:



	};


}