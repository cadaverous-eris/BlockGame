#include "FontRenderer.h"

#include <span>
#include <algorithm>
#include <type_traits>

#include "render/Renderer.h"
#include "util/math/math.h"

#include <iostream> // TODO: remove
#include <glm/gtx/io.hpp> // TODO: remove

namespace eng {

	FontRenderer::FontRenderer(Renderer* const renderer) : renderer(renderer) {
		// create shader uniforms
		fontShader.createUniform("framebufferSize");
		fontShader.createUniform("edgeVal");
		fontShader.createUniform("padding");
		fontShader.createUniform("pixelDistScale");
		fontShader.createUniform("textureSampler");
		fontShader.createUniform("mvpMatrix");
		underlineShader.createUniform("mvpMatrix");

		// font VAO and VBO setup
		fontVAO.bind(); // bind the block VAO
		fontVBO.setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
		fontVBO.bind();
		fontVAO.setVertexFormat(FontQuad::format);
		// underline VAO and VBO setup
		underlineVAO.bind();
		underlineVBO.setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
		underlineVBO.bind();
		underlineVAO.setVertexFormat(ColorVert::format);
	}


	void FontRenderer::render(const float partialTicks) {
		// TODO: implement?

		flush();
	}

	void FontRenderer::flush() {
		const glm::mat4& orthoMatrix = renderer->getOrthoMatrix();

		if (!textVertBuffers.underlineBuffer.empty()) {
			underlineShader.bind();
			underlineShader.setUniform("mvpMatrix", orthoMatrix);

			glEnable(GL_CULL_FACE);
			//glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			underlineVAO.bind();
			underlineVBO.setData(std::span(textVertBuffers.underlineBuffer.data(), textVertBuffers.underlineBuffer.size()));
			underlineVAO.draw(DrawMode::LINES, 0, textVertBuffers.underlineBuffer.size());

			glDisable(GL_BLEND);
			//glDepthMask(GL_TRUE);

			textVertBuffers.underlineBuffer.clear();
		}
		if (!textVertBuffers.fontQuadBuffer.empty()) {
			font.getTexture().bind();

			fontShader.bind();
			// TODO: determine whether to use scaled or unscaled window size
			fontShader.setUniform("framebufferSize", static_cast<glm::vec2>(renderer->getWindowSizeScaled()));
			//fontShader.setUniform("framebufferSize", static_cast<glm::vec2>(renderer->getWindowSize()));
			fontShader.setUniform("edgeVal", FontRenderer::sdf_on_edge_value / 255.0f);
			fontShader.setUniform("padding", static_cast<float>(FontRenderer::sdf_padding));
			fontShader.setUniform("pixelDistScale", FontRenderer::sdf_pixel_dist_scale);
			fontShader.setUniform("textureSampler", 0);
			fontShader.setUniform("mvpMatrix", orthoMatrix);

			glEnable(GL_CULL_FACE);
			//glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			fontVAO.bind();
			fontVBO.setData(std::span(textVertBuffers.fontQuadBuffer.data(), textVertBuffers.fontQuadBuffer.size()));
			fontVAO.draw(DrawMode::POINTS, 0, textVertBuffers.fontQuadBuffer.size());

			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);

			textVertBuffers.fontQuadBuffer.clear();
		}
	}

	float FontRenderer::getFontSizeForLineHeight(const float lineHeight) const {
		const float scale = lineHeight / (font.fontData.lineHeight);
		return scale * (font.fontData.ascent - font.fontData.descent);
	}
	float FontRenderer::getLineHeight(const float fontSize) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		return scale * font.fontData.lineHeight;
	}

	float FontRenderer::getFontAscent(const float fontSize) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		return font.fontData.ascent * scale;
	}
	float FontRenderer::getFontDescent(const float fontSize) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		return font.fontData.descent * scale;
	}
	float FontRenderer::getFontLineHeight(const float fontSize) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		return font.fontData.lineHeight * scale;
	}
	float FontRenderer::getFontLineGap(const float fontSize) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		return font.fontData.lineGap * scale;
	}


	float FontRenderer::getTextWidth(std::u32string_view text, const float fontSize) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		float width = 0;
		const Glyph* prevGlyph = nullptr;
		for (const auto& codepoint : text) {
			const Glyph& glyph = font.getGlyph(codepoint);
			if (prevGlyph)
				width += font.getKernAdvance(*prevGlyph, glyph) * scale; // apply kerning
			width += glyph.xAdvance * scale;

			prevGlyph = &glyph;
		}
		return width;
	}


	void FontRenderer::drawCursor(const glm::vec3& origin, float fontSize, const Color& color) {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		const float topY = ((font.fontData.lineGap * 0.125f) + font.fontData.ascent) * scale;
		const float bottomY = ((font.fontData.lineGap * 0.875f) + font.fontData.descent) * scale;
		textVertBuffers.underlineBuffer.emplace_back(glm::vec2(origin.x, topY), color);
		textVertBuffers.underlineBuffer.emplace_back(glm::vec2(origin.x, bottomY), color);
	}


	float FontRenderer::drawTextToBuffer(TextVertexBuffers& textBuffers, std::u32string_view text, const glm::vec3& origin, float fontSize, const Color& color, const TextOutline& outline, const bool underline) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		const float unscaledBaselineOffset = (font.fontData.lineGap * 0.5f) + font.fontData.ascent; // unscaled distance from top of a line of text to the baseline
		const float baselineOffset = unscaledBaselineOffset * scale; // distance from top of a line of text to the baseline

		float width = 0;
		glm::vec3 pos { origin.x, origin.y + baselineOffset, origin.z };
		const Glyph* prevGlyph = nullptr;
		for (const auto& codepoint : text) {
			const Glyph& glyph = font.getGlyph(codepoint);
			if (prevGlyph) {
				const float kern = font.getKernAdvance(*prevGlyph, glyph) * scale;
				pos.x += kern; // apply kerning
				width += kern;
			}

			const glm::vec3 gOrig = pos + glm::vec3(static_cast<glm::vec2>(glyph.offset) * scale, 0);
			const glm::vec2 glyphSize = static_cast<glm::vec2>(glyph.sdfSize) * scale;

			const glm::vec2 uvMin = glyph.getMinUV();
			const glm::vec2 uvMax = glyph.getMaxUV();

			// TODO: add a way to draw bold text
			// italicized font rendering
			//constexpr float slant = 0.15f; // amount to offset the x coordinates of the font quad based on the font size
			//const float slantOffset = slant * fontSize;
			//quadBuffer.emplace_back(
			//	FontVert { { gOrig.x + slantOffset, gOrig.y, gOrig.z }, uvMin },
			//	FontVert { { gOrig.x - slantOffset, gOrig.y + glyphSize.y, gOrig.z }, { uvMin.x, uvMax.y } },
			//	FontVert { { gOrig.x + slantOffset + glyphSize.x, gOrig.y, gOrig.z }, { uvMax.x, uvMin.y } },
			//	FontVert { { gOrig.x - slantOffset + glyphSize.x, gOrig.y + glyphSize.y, gOrig.z }, uvMax },
			//	color, outline.color, outline.size, outline.spread
			//);
			// regular font rendering
			textBuffers.fontQuadBuffer.emplace_back(
				FontVert { gOrig, uvMin },
				FontVert { { gOrig.x, gOrig.y + glyphSize.y, gOrig.z }, { uvMin.x, uvMax.y } },
				FontVert { { gOrig.x + glyphSize.x, gOrig.y, gOrig.z }, { uvMax.x, uvMin.y } },
				FontVert { { gOrig.x + glyphSize.x, gOrig.y + glyphSize.y, gOrig.z }, uvMax },
				color, outline.color, outline.size, outline.spread
			);

			const float xAdv = glyph.xAdvance * scale;
			pos.x += xAdv;
			width += xAdv;

			prevGlyph = &glyph;
		}
		if (underline) {
			const float underlineOffset = (unscaledBaselineOffset - (1.0f * font.fontData.descent)) * scale; // distance from the top of a line to the underline
			const float ulY = origin.y + underlineOffset;
			textBuffers.underlineBuffer.emplace_back(glm::vec3(origin.x, ulY, origin.z), color);
			textBuffers.underlineBuffer.emplace_back(glm::vec3(origin.x + width, ulY, origin.z), color);
		}

		return width;
	}


	// returns true if the character should not add to the width of a line it appears at the end of
	static constexpr bool isSpace(const char32_t c) noexcept {
		return ((c >= 0x09) && (c <= 0x0D)) ||
			(c == 0x20) || (c == 0x85) ||
			(c == 0xA0) || (c == 0x1680) ||
			(c == 0x018E) ||
			((c >= 0x2000) && (c <= 0x200B)) ||
			(c == 0x2028) || (c == 0x2029) ||
			(c == 0x202F) || (c == 0x205F) ||
			(c == 0x3000) || (c == 0xFEFF);
	}
	// returns true if the character should force a line break
	static constexpr bool isLineBreak(const char32_t c) noexcept {
		return ((c >= 0x0A) && (c <= 0x0D)) ||
			(c == 0x85) || (c == 0x2028) || (c == 0x2029);
	}
	// returns true if the character should force a line break
	static constexpr bool isLineBreak(const char32_t c, const char32_t nextChar) noexcept {
		return ((c >= 0x0A) && (c <= 0x0C)) ||
			((c == 0x0D) && (nextChar != 0x0A)) ||
			(c == 0x85) || (c == 0x2028) || (c == 0x2029);
	}

	TextWrapResult FontRenderer::splitLines(const std::u32string_view text, const float fontSize, const MultilineConfig& multilineConfig, const size_t startIndex) const {
		using Line = TextWrapResult::Line;

		if (multilineConfig.area.x <= 1)
			return { 0, 0, startIndex, startIndex, std::vector<Line>{} };

		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		const float lineHeight = font.fontData.lineHeight * scale;
		std::vector<Line> lines {};
		float width = 0, lineWidth = 0, lineWidthTrailing = 0;
		float lineWidthPreWord = 0, lineWidthPreWordTrailing = 0;
		float wordWidth = 0;
		const Glyph* prevGlyph = nullptr;
		size_t lineStart = startIndex, lineEnd = startIndex;
		size_t wordStart = startIndex, wordEnd = startIndex;
		size_t midWordBreakIndex = startIndex;
		size_t forcedMidWordBreakIndex = startIndex;
		float lineWidthPreMidWordBreak = 0;
		bool wasSpace = false;

		size_t i = 0;
		const auto handleMidWordBreak = [&]() -> bool {
			//const auto wordWidthPreBreak = lineWidthPreMidWordBreak - lineWidthPreWordTrailing;
			const auto wordLength = wordEnd - wordStart;
			const float wordLineRatio = wordWidth / multilineConfig.area.x;
			const bool wordTooWide = wordLineRatio > 1;
			const size_t minBreakDist = std::min(static_cast<size_t>(std::ceil(static_cast<float>(wordLength) * 0.3f)), size_t{5});
			if (wordTooWide || ((wordLineRatio >= 0.65f) && ((midWordBreakIndex - wordStart) >= minBreakDist))) {
				if (wordTooWide || ((wordEnd - midWordBreakIndex) >= minBreakDist)) {
					i = midWordBreakIndex - 1;
					lineWidth = lineWidthPreMidWordBreak;
				} else {
					lineWidth = lineWidthTrailing = lineWidthPreMidWordBreak = lineWidthPreWordTrailing;
					wordWidth = 0;
					forcedMidWordBreakIndex = wordEnd - minBreakDist;
					i = wordStart - 1;
					wasSpace = false;
					if (i == (lineStart - 1)) prevGlyph = nullptr;
					else prevGlyph = &font.getGlyph(text[i]);
					return true;
				}
			} else {
				i = wordStart - 1;
				lineWidth = lineWidthPreWord;
			}
			return false;
		};
		for (; i < text.length(); i++) {
			const char32_t& codepoint = text[i];
			if (((i + 1) < text.length()) ? isLineBreak(codepoint, text[i + 1]) : isLineBreak(codepoint)) {
				if (!wasSpace && (i != lineStart)) wordEnd = i;
				if (midWordBreakIndex != wordStart) {
					if (handleMidWordBreak()) continue;
				}
				goto new_line;
			} else {
				const Glyph& glyph = font.getGlyph(codepoint);
				const bool isSpaceChar = isSpace(codepoint);
				const float glyphWidth = glyph.xAdvance * scale;
				const float kerning = (prevGlyph) ? font.getKernAdvance(*prevGlyph, glyph) * scale : 0;
				if (isSpaceChar) {
					if (!wasSpace) wordEnd = i;
					lineWidthTrailing += kerning + glyphWidth;
				} else {
					if (wasSpace) {
						if (lineWidthTrailing > multilineConfig.area.x) {
							if (midWordBreakIndex != wordStart) {
								if (handleMidWordBreak()) continue;
							} else {
								i--;
							}
							goto new_line;
						}
						wordStart = i;
						wordWidth = 0;
						lineWidthPreWord = lineWidth;
						lineWidth = lineWidthPreWordTrailing = lineWidthPreMidWordBreak = lineWidthTrailing;
						midWordBreakIndex = forcedMidWordBreakIndex = i;
					}
					const float newWordWidth = wordWidth + kerning + glyphWidth;
					const float newLineWidth = lineWidth + kerning + glyphWidth;

					if ((i + 1) == forcedMidWordBreakIndex) {
						lineWidth = newLineWidth;
						goto new_line;
					}
					if ((midWordBreakIndex == wordStart) && (newLineWidth > multilineConfig.area.x)) {
						if (wordStart == i) {
							if (i == lineStart) { // first character on the line is too wide to fit
								lineWidth = newLineWidth;
							} else {
								i--;
								lineWidth = lineWidthPreWord;
							}
							goto new_line;
						} else {
							midWordBreakIndex = i;
							lineWidthPreMidWordBreak = lineWidth;
						}
					}
					wordWidth = newWordWidth;
					lineWidth = lineWidthTrailing = newLineWidth;
				}
				prevGlyph = &glyph;
				wasSpace = isSpaceChar;
			}
			if ((i + 1) < text.length()) continue;

			if (midWordBreakIndex != wordStart) {
				if (handleMidWordBreak()) continue;
			}

			new_line:
			lineEnd = i + 1;
			if (lineEnd == lineStart)
				return { 0, 0, startIndex, startIndex, std::vector<Line>{} };
			lines.emplace_back(lineStart, lineEnd, text.substr(lineStart, lineEnd - lineStart), lineWidth);
			if (lineWidth > width) width = lineWidth;

			wordWidth = lineWidth = lineWidthTrailing = lineWidthPreWord = lineWidthPreWordTrailing = lineWidthPreMidWordBreak = 0;
			prevGlyph = nullptr;
			lineStart = wordStart = wordEnd = midWordBreakIndex = forcedMidWordBreakIndex = lineEnd;
			wasSpace = false;

			if (((multilineConfig.maxLines > 0) && (lines.size() == multilineConfig.maxLines)) ||
				((multilineConfig.area.y > 0) && ((lineHeight * static_cast<float>(lines.size() + 1)) > multilineConfig.area.y))
			) break;
		}

		return {
			width, lineHeight * static_cast<float>(lines.size()),
			startIndex, lineEnd,
			lines
		};
	}

	void FontRenderer::drawMultilineTextToBuffer(TextVertexBuffers& textBuffers, const TextWrapResult& textWrapRes, const MultilineConfig& multilineConfig, const glm::vec3& origin, float fontSize, const Color& color, const TextOutline& outline, bool underline) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		const float unscaledBaselineOffset = (font.fontData.lineGap * 0.5f) + font.fontData.ascent; // unscaled distance from top of a line of text to the baseline
		const float baselineOffset = unscaledBaselineOffset * scale; // distance from top of a line of text to the baseline
		const float underlineOffset = (unscaledBaselineOffset - (1.0f * font.fontData.descent)) * scale; // distance from the top of a line to the underline
		const float lineHeight = font.fontData.lineHeight * scale;

		glm::vec3 pos { origin.x, origin.y + baselineOffset, origin.z };
		for (const auto& line : textWrapRes.lines) {
			switch (multilineConfig.textAlign) {
			case TextAlign::Center:
				pos.x = origin.x + ((multilineConfig.area.x - line.width) / 2.0f);
				break;
			case TextAlign::Right:
				pos.x = origin.x + multilineConfig.area.x - line.width;
				break;
			default:
				pos.x = origin.x;
				break;
			}
			const float lineStartX = pos.x;
			const Glyph* prevGlyph = nullptr;
			for (const auto& codepoint : line.text) {
				if (isLineBreak(codepoint)) {
					prevGlyph = nullptr;
					continue;
				}

				const Glyph& glyph = font.getGlyph(codepoint);
				if (prevGlyph) {
					const float kern = font.getKernAdvance(*prevGlyph, glyph) * scale;
					pos.x += kern; // apply kerning
				}

				const glm::vec3 gOrig = pos + glm::vec3(static_cast<glm::vec2>(glyph.offset) * scale, 0);
				const glm::vec2 glyphSize = static_cast<glm::vec2>(glyph.sdfSize) * scale;

				const glm::vec2 uvMin = glyph.getMinUV();
				const glm::vec2 uvMax = glyph.getMaxUV();

				textBuffers.fontQuadBuffer.emplace_back(
					FontVert { gOrig, uvMin },
					FontVert { { gOrig.x, gOrig.y + glyphSize.y, gOrig.z }, { uvMin.x, uvMax.y } },
					FontVert { { gOrig.x + glyphSize.x, gOrig.y, gOrig.z }, { uvMax.x, uvMin.y } },
					FontVert { { gOrig.x + glyphSize.x, gOrig.y + glyphSize.y, gOrig.z }, uvMax },
					color, outline.color, outline.size, outline.spread
				);

				const float xAdv = glyph.xAdvance * scale;
				pos.x += xAdv;

				prevGlyph = &glyph;
			}
			if (underline) {
				const float ulY = pos.y - baselineOffset + underlineOffset;
				textBuffers.underlineBuffer.emplace_back(glm::vec3(lineStartX, ulY, origin.z), color);
				textBuffers.underlineBuffer.emplace_back(glm::vec3(lineStartX + line.width, ulY, origin.z), color);
			}
			pos.y += lineHeight;
		}
	}

	TextWrapResult FontRenderer::drawMultilineText(std::u32string_view text, size_t startIndex, const MultilineConfig& multilineConfig, const glm::vec3& origin, float fontSize, const Color& color, const TextOutline& outline, const bool underline) {
		const auto textWrapRes = splitLines(text, fontSize, multilineConfig, startIndex);
		drawMultilineText(textWrapRes, multilineConfig, origin, fontSize, color, outline, underline);
		return textWrapRes;
	}

}