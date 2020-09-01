#include "FontRenderer.h"

#include <span>
#include <type_traits>

#include "render/Renderer.h"

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
		underlineVAO.setVertexFormat(LineVert::format);
	}


	void FontRenderer::render(const float partialTicks) {
		// TODO: implement?

		flush();
	}

	void FontRenderer::flush() {
		const glm::mat4& orthoMatrix = renderer->getOrthoMatrix();

		if (!underlineBuffer.empty()) {
			underlineShader.bind();
			underlineShader.setUniform("mvpMatrix", orthoMatrix);

			glEnable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			underlineVAO.bind();
			underlineVBO.setData(std::span(underlineBuffer.data(), underlineBuffer.size()));
			underlineVAO.draw(DrawMode::LINES, 0, underlineBuffer.size());

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			underlineBuffer.clear();
		}
		if (!fontQuadBuffer.empty()) {
			font.getTexture().bind();

			fontShader.bind();
			fontShader.setUniform("framebufferSize", static_cast<glm::vec2>(renderer->getWindowSizeScaled()));
			fontShader.setUniform("edgeVal", FontRenderer::sdf_on_edge_value / 255.0f);
			fontShader.setUniform("padding", static_cast<float>(FontRenderer::sdf_padding));
			fontShader.setUniform("pixelDistScale", FontRenderer::sdf_pixel_dist_scale);
			fontShader.setUniform("textureSampler", 0);
			fontShader.setUniform("mvpMatrix", orthoMatrix);

			glEnable(GL_CULL_FACE);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			fontVAO.bind();
			fontVBO.setData(std::span(fontQuadBuffer.data(), fontQuadBuffer.size()));
			fontVAO.draw(DrawMode::POINTS, 0, fontQuadBuffer.size());

			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);

			fontQuadBuffer.clear();
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

	// returns true if the character should not add to the width of a line it appears at the end of
	static constexpr bool isSpace(const char32_t c) noexcept {
		return ((c >= 0x09) && (c <= 0x0D)) ||
			(c == 0x20) || (c == 0x85) ||
			(c == 0xA0) || (c == 0x1680) ||
			(c == 0x18E) ||
			((c >= 0x2000) && (c <= 0x200B)) ||
			(c == 0x2028) || (c == 0x2029) ||
			(c == 0x202F) || (c == 0x205F) ||
			(c == 0x3000) || (c == 0xFEFF);
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


	float FontRenderer::drawTextToBuffer(FontQuadBuffer& fontQuads, LineBuffer& underlineVerts, std::u32string_view text, const glm::vec2& origin, float fontSize, const Color& color, const FontOutline& outline, const bool underline) const {
		const float scale = fontSize / (font.fontData.ascent - font.fontData.descent);
		const float unscaledBaselineOffset = (font.fontData.lineGap * 0.5f) + font.fontData.ascent; // unscaled distance from top of a line of text to the baseline
		const float baselineOffset = unscaledBaselineOffset * scale; // distance from top of a line of text to the baseline

		float width = 0;
		glm::vec2 pos { origin.x, origin.y + baselineOffset };
		const Glyph* prevGlyph = nullptr;
		for (const auto& codepoint : text) {
			const Glyph& glyph = font.getGlyph(codepoint);
			if (prevGlyph) {
				const float kern = font.getKernAdvance(*prevGlyph, glyph) * scale;
				pos.x += kern; // apply kerning
				width += kern;
			}

			const glm::vec2 gOrig = pos + (static_cast<glm::vec2>(glyph.offset) * scale);
			const glm::vec2 glyphSize = static_cast<glm::vec2>(glyph.sdfSize) * scale;

			const glm::vec2 uvMin = glyph.getMinUV();
			const glm::vec2 uvMax = glyph.getMaxUV();

			// TODO: add a way to draw bold text
			// italicized font rendering
			//constexpr float slant = 0.15f; // amount to offset the x coordinates of the font quad based on the font size
			//const float slantOffset = slant * fontSize;
			//quadBuffer.emplace_back(
			//	FontVert { { gOrig.x + slantOffset, gOrig.y }, uvMin },
			//	FontVert { { gOrig.x - slantOffset, gOrig.y + glyphSize.y }, { uvMin.x, uvMax.y } },
			//	FontVert { { gOrig.x + slantOffset + glyphSize.x, gOrig.y }, { uvMax.x, uvMin.y } },
			//	FontVert { { gOrig.x - slantOffset + glyphSize.x, gOrig.y + glyphSize.y }, uvMax },
			//	color, outline.color, outline.size, outline.spread
			//);
			// regular font rendering
			fontQuads.emplace_back(
				FontVert { gOrig, uvMin },
				FontVert { { gOrig.x, gOrig.y + glyphSize.y }, { uvMin.x, uvMax.y } },
				FontVert { { gOrig.x + glyphSize.x, gOrig.y }, { uvMax.x, uvMin.y } },
				FontVert { gOrig + glyphSize, uvMax },
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
			underlineVerts.emplace_back(glm::vec2(origin.x, ulY), color);
			underlineVerts.emplace_back(glm::vec2(origin.x + width, ulY), color);
		}

		return width;
	}

}