#include "FontRenderer.h"

#include <span>
#include <algorithm>
#include <utility>
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
		highlightShader.createUniform("mvpMatrix");

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
		// highlight VAO and VBO setup
		highlightVAO.bind();
		highlightVBO.setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
		highlightVBO.bind();
		highlightVAO.setVertexFormat(ColorVert::format);
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
		if (!textVertBuffers.highlightBuffer.empty()) {
			highlightShader.bind();
			highlightShader.setUniform("mvpMatrix", orthoMatrix);

			glEnable(GL_CULL_FACE);
			//glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			highlightVAO.bind();
			highlightVBO.setData(std::span(textVertBuffers.highlightBuffer.data(), textVertBuffers.highlightBuffer.size()));
			highlightVAO.draw(DrawMode::TRIANGLES, 0, textVertBuffers.highlightBuffer.size());

			glDisable(GL_BLEND);
			//glDepthMask(GL_TRUE);

			textVertBuffers.highlightBuffer.clear();
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


	static bool addGlyphQuadToBuffer(FontRenderer::FontQuadBuffer& buffer, const Glyph& glyph, const float scale, const glm::vec3& pos, const Color& color, const TextOutline& outline, const glm::vec2& xRange) noexcept {
		const float& minX = xRange.x;
		const float& maxX = xRange.y;
		const bool doCutoff = minX != maxX;
		const glm::vec2 glyphOffset = static_cast<glm::vec2>(glyph.offset) * scale;
		const glm::vec3 gOrig { pos.x + glyphOffset.x, pos.y + glyphOffset.y, pos.z };
		const glm::vec2 glyphSize = static_cast<glm::vec2>(glyph.sdfSize) * scale;
		glm::vec3 minPos = gOrig;
		glm::vec3 maxPos { gOrig.x + glyphSize.x, gOrig.y + glyphSize.y, gOrig.z };
		if (doCutoff && ((maxPos.x < minX) || (minPos.x > maxX)))
			return false;
		glm::vec2 minUV = glyph.getMinUV();
		glm::vec2 maxUV = glyph.getMaxUV();
		if (doCutoff) {
			const float uvWidth = maxUV.x - minUV.x;
			if (minPos.x < minX) {
				const float cutoffAmount = (minX - minPos.x) / glyphSize.x;
				minPos.x = minX;
				minUV.x += uvWidth * cutoffAmount;
			}
			if (maxPos.x > maxX) {
				const float cutoffAmount = (maxPos.x - maxX) / glyphSize.x;
				maxPos.x = maxX;
				maxUV.x -= uvWidth * cutoffAmount;
			}
		}
		buffer.emplace_back(
			FontVert { minPos, minUV },
			FontVert { { minPos.x, maxPos.y, minPos.z }, { minUV.x, maxUV.y } },
			FontVert { { maxPos.x, minPos.y, maxPos.z }, { maxUV.x, minUV.y } },
			FontVert { maxPos, maxUV },
			color, outline.color, outline.size, outline.spread
		);
		return true;
	}
	static bool addSlantedGlyphQuadToBuffer(FontRenderer::FontQuadBuffer& buffer, const Glyph& glyph, const float scale, const glm::vec3& pos, const Color& color, const TextOutline& outline, const float slant, const glm::vec2& xRange) noexcept {
		const float& minX = xRange.x;
		const float& maxX = xRange.y;
		const bool doCutoff = minX != maxX;
		const glm::vec2 glyphOffset = static_cast<glm::vec2>(glyph.offset) * scale;
		const glm::vec3 gOrig { pos.x + glyphOffset.x, pos.y + glyphOffset.y, pos.z };
		const glm::vec2 glyphSize = static_cast<glm::vec2>(glyph.sdfSize) * scale;
		const glm::vec3 minPos = gOrig;
		const glm::vec3 maxPos { gOrig.x + glyphSize.x, gOrig.y + glyphSize.y, gOrig.z };
		if (doCutoff && (((maxPos.x + slant) < minX) || ((minPos.x - slant) > maxX)))
			return false;
		const glm::vec2 minUV = glyph.getMinUV();
		const glm::vec2 maxUV = glyph.getMaxUV();
		FontVert v0 { { minPos.x + slant, minPos.y, minPos.z }, minUV };
		FontVert v1 { { minPos.x - slant, maxPos.y, minPos.z }, { minUV.x, maxUV.y } };
		FontVert v2 { { maxPos.x + slant, minPos.y, minPos.z }, { maxUV.x, minUV.y } };
		FontVert v3 { { maxPos.x - slant, maxPos.y, minPos.z }, maxUV };
		if (doCutoff) {
			const float uvWidth = maxUV.x - minUV.x;
			if (v0.pos.x < minX) {
				const float cutoffAmount = (minX - v0.pos.x) / glyphSize.x;
				v0.pos.x = minX;
				v0.texCoord.x += uvWidth * cutoffAmount;
			}
			if (v1.pos.x < minX) {
				const float cutoffAmount = (minX - v1.pos.x) / glyphSize.x;
				v1.pos.x = minX;
				v1.texCoord.x += uvWidth * cutoffAmount;
			}
			if (v2.pos.x > maxX) {
				const float cutoffAmount = (v2.pos.x - maxX) / glyphSize.x;
				v2.pos.x = maxX;
				v2.texCoord.x -= uvWidth * cutoffAmount;
			}
			if (v3.pos.x > maxX) {
				const float cutoffAmount = (v3.pos.x - maxX) / glyphSize.x;
				v3.pos.x = maxX;
				v3.texCoord.x -= uvWidth * cutoffAmount;
			}
		}
		buffer.emplace_back(v0, v1, v2, v3, color, outline.color, outline.size, outline.spread);
		return true;
	}

	float FontRenderer::drawTextToBuffer(TextVertexBuffers& textBuffers, std::u32string_view text, const glm::vec3& origin, float fontSize, const Color& color, const TextOutline& outline, const bool underline, float maxWidth, float xOffset) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		const float unscaledBaselineOffset = (font.fontData.lineGap * 0.5f) + font.fontData.ascent; // unscaled distance from top of a line of text to the baseline
		const float baselineOffset = unscaledBaselineOffset * scale; // distance from top of a line of text to the baseline

		const float minX = origin.x;
		const float maxX = origin.x + maxWidth;
		const glm::vec2 xRange { minX, maxX };

		float width = 0;
		glm::vec3 pos { origin.x + xOffset, origin.y + baselineOffset, origin.z };
		const Glyph* prevGlyph = nullptr;
		for (const auto& codepoint : text) {
			const Glyph& glyph = font.getGlyph(codepoint);
			if (prevGlyph) {
				const float kern = font.getKernAdvance(*prevGlyph, glyph) * scale;
				pos.x += kern; // apply kerning
				width += kern;
			}

			// TODO: add a way to draw bold text
			// italicized font rendering
			//constexpr float slant = 0.15f; // amount to offset the x coordinates of the font quad based on the font size
			//addSlantedGlyphQuadToBuffer(textBuffers.fontQuadBuffer, glyph, scale, pos, color, outline, slant * fontSize, xRange);
			// regular font rendering
			addGlyphQuadToBuffer(textBuffers.fontQuadBuffer, glyph, scale, pos, color, outline, xRange);

			const float xAdv = glyph.xAdvance * scale;
			pos.x += xAdv;
			width += xAdv;

			prevGlyph = &glyph;
		}
		if (underline) {
			const float underlineOffset = (unscaledBaselineOffset - (1.0f * font.fontData.descent)) * scale; // distance from the top of a line to the underline
			const float ulY = origin.y + underlineOffset;
			const float ulMinX = vmax(origin.x + xOffset, minX);
			const float ulMaxX = vmin(origin.x + xOffset + width, maxX);
			textBuffers.underlineBuffer.emplace_back(glm::vec3(ulMinX, ulY, origin.z), color);
			textBuffers.underlineBuffer.emplace_back(glm::vec3(ulMaxX, ulY, origin.z), color);
		}

		return width;
	}

	EditableTextRenderResult FontRenderer::drawEditableTextToBuffer(TextVertexBuffers& textBuffers, std::u32string_view text, const std::pair<size_t, size_t>& cursorPos, const bool drawCursor, const glm::vec3& origin, float fontSize, const Color& color, const TextOutline& outline, const Color& cursorColor, const Color& highlightColor, const Color& highlightedTextColor, const TextOutline& highlightedOutline, float maxWidth, float xOffset) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		const float unscaledBaselineOffset = (font.fontData.lineGap * 0.5f) + font.fontData.ascent; // unscaled distance from top of a line of text to the baseline
		const float baselineOffset = unscaledBaselineOffset * scale; // distance from top of a line of text to the baseline
		const bool hasHighlight = (cursorPos.first != cursorPos.second);
		const size_t selectionStart = vmin(cursorPos.first, cursorPos.second);
		const size_t selectionEnd = vmax(cursorPos.first, cursorPos.second);

		const float minX = origin.x;
		const float maxX = origin.x + maxWidth;
		const glm::vec2 xRange { minX, maxX };

		float cursorX = origin.x + xOffset;
		float highlightStartX = origin.x + xOffset, highlightEndX = origin.x + xOffset;
		float width = 0;
		glm::vec3 pos { origin.x + xOffset, origin.y + baselineOffset, origin.z };
		const Glyph* prevGlyph = nullptr;
		for (size_t i = 0; i < text.size(); i++) {
			const auto& codepoint = text[i];
			const Glyph& glyph = font.getGlyph(codepoint);
			if (prevGlyph) {
				const float kern = font.getKernAdvance(*prevGlyph, glyph) * scale;
				pos.x += kern; // apply kerning
				width += kern;
			}
			if (i == cursorPos.second) {
				cursorX = pos.x;
			}
			if (i == selectionStart) {
				highlightStartX = pos.x;
			} else if (i == selectionEnd) {
				highlightEndX = pos.x;
			}
			const bool highlighted = (i >= selectionStart) && (i < selectionEnd);

			const Color& glyphColor = highlighted ? highlightedTextColor : color;
			addGlyphQuadToBuffer(textBuffers.fontQuadBuffer, glyph, scale, pos, glyphColor, outline, xRange);

			const float xAdv = glyph.xAdvance * scale;
			pos.x += xAdv;
			width += xAdv;

			prevGlyph = &glyph;
		}
		const float cursorTopY = origin.y + ((font.fontData.lineGap * 0.25f) * scale);
		const float cursorBottomY = origin.y + ((font.fontData.lineHeight - (font.fontData.lineGap * 0.25f)) * scale);
		if (cursorPos.second == text.size()) cursorX = origin.x + xOffset + width;
		if (hasHighlight) {
			if (selectionEnd == text.size()) {
				highlightEndX = origin.x + xOffset + width;
			}
			highlightStartX = vmax(highlightStartX, minX);
			highlightEndX = vmin(highlightEndX, maxX);
			if (highlightStartX < highlightEndX) {
				textBuffers.highlightBuffer.insert(textBuffers.highlightBuffer.end(), {
					{ glm::vec3(highlightStartX, cursorTopY, origin.z), highlightColor },
					{ glm::vec3(highlightStartX, cursorBottomY, origin.z), highlightColor },
					{ glm::vec3(highlightEndX, cursorTopY, origin.z), highlightColor },
					{ glm::vec3(highlightEndX, cursorTopY, origin.z), highlightColor },
					{ glm::vec3(highlightStartX, cursorBottomY, origin.z), highlightColor },
					{ glm::vec3(highlightEndX, cursorBottomY, origin.z), highlightColor },
				});
			}
		}
		if (drawCursor) {
			if ((cursorX >= minX) && (cursorX <= maxX)) {
				textBuffers.underlineBuffer.emplace_back(glm::vec3(cursorX, cursorTopY, origin.z), cursorColor);
				textBuffers.underlineBuffer.emplace_back(glm::vec3(cursorX, cursorBottomY, origin.z), cursorColor);
			}
		}

		return { width, cursorX - (origin.x + xOffset) };
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

				const glm::vec2 glyphOffset = static_cast<glm::vec2>(glyph.offset) * scale;
				const glm::vec3 gOrig { pos.x + glyphOffset.x, pos.y + glyphOffset.y, pos.z };
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