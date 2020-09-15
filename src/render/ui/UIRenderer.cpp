#include "UIRenderer.h"

namespace eng {

	UIRenderer::UIRenderer(Renderer* const renderer) : renderer(renderer) {
		// create shader uniforms
		colorShader.createUniform("matrix");
		textureShader.createUniform("matrix");
		textureShader.createUniform("textureSampler");

		// VAO and VBO setup
		coloredVAO.bind();
		coloredVBO.setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
		coloredVBO.bind();
		coloredVAO.setVertexFormat(UIVertColor::format);
		lineVAO.bind();
		lineVBO.setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
		lineVBO.bind();
		lineVAO.setVertexFormat(UIVertColor::format);
		texturedVAO.bind();
		texturedVBO.setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
		texturedVBO.bind();
		texturedVAO.setVertexFormat(UIVertTex::format);
	}

	static inline void preRenderColored(const bool writeDepth) {
		glEnable(GL_CULL_FACE);
		if (!writeDepth) glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	static inline void postRenderColored() {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
	static inline void preRenderLines(const bool writeDepth) {
		glEnable(GL_CULL_FACE);
		if (!writeDepth) glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	static inline void postRenderLines() {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
	static inline void preRenderTextured(const bool writeDepth) {
		glEnable(GL_CULL_FACE);
		if (!writeDepth) glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	static inline void postRenderTextured() {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	void UIRenderer::flushColored(const glm::mat4& matrix, const TriMode mode, bool writeDepth) {
		if (!coloredTriBuffer.empty()) {
			colorShader.bind();
			colorShader.setUniform("matrix", matrix);
			preRenderColored(writeDepth);
			coloredVAO.bind();
			coloredVBO.setData(std::span(coloredTriBuffer.data(), coloredTriBuffer.size()));
			coloredVAO.draw(toDrawMode(mode), 0, coloredTriBuffer.size());
			postRenderColored();
			coloredTriBuffer.clear();
		}
	}
	void UIRenderer::flushLines(const glm::mat4& matrix, const LineMode mode, bool writeDepth) {
		if (!lineBuffer.empty()) {
			colorShader.bind();
			colorShader.setUniform("matrix", matrix);
			preRenderLines(writeDepth);
			lineVAO.bind();
			lineVBO.setData(std::span(lineBuffer.data(), lineBuffer.size()));
			lineVAO.draw(toDrawMode(mode), 0, lineBuffer.size());
			postRenderLines();
			lineBuffer.clear();
		}
	}
	void UIRenderer::flushTextured(const glm::mat4& matrix, const TriMode mode, bool writeDepth, int textureUnit) {
		if (!texturedTriBuffer.empty()) {
			textureShader.bind();
			textureShader.setUniform("matrix", matrix);
			textureShader.setUniform("textureSampler", textureUnit);
			preRenderTextured(writeDepth);
			texturedVAO.bind();
			texturedVBO.setData(std::span(texturedTriBuffer.data(), texturedTriBuffer.size()));
			texturedVAO.draw(toDrawMode(mode), 0, texturedTriBuffer.size());
			postRenderTextured();
			texturedTriBuffer.clear();
		}
	}

	void UIRenderer::render(const float partialTicks) {
		flushColored();
		flushLines();
		texturedTriBuffer.clear(); // don't render the textured tris since there's no way of knowing if the correct texture is bound
	}

	void UIRenderer::drawColoredQuad(const UIVertColor& a, const UIVertColor& b, const UIVertColor& c, const UIVertColor& d) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), { a, b, c, c, b, d, });
	}
	void UIRenderer::drawColoredQuad(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color }, { c, color }, { b, color }, { d, color },
		});
	}
	void UIRenderer::drawColoredQuad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color }, { c, color }, { b, color }, { d, color },
		});
	}
	void UIRenderer::drawColoredQuad(const glm::vec3& p, const glm::vec2& s, const Color& color) {
		const glm::vec3 b { p.x, p.y + s.y, p.z };
		const glm::vec3 c { p.x + s.x, p.y, p.z };
		const glm::vec3 d { p.x + s.x, p.y + s.y, p.z };
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ p, color }, { b, color }, { c, color }, { c, color }, { b, color }, { d, color },
		});
	}
	void UIRenderer::drawColoredQuad(const glm::vec2& p, const glm::vec2& s, const Color& color) {
		const glm::vec2 b { p.x, p.y + s.y };
		const glm::vec2 c { p.x + s.x, p.y };
		const glm::vec2 d { p.x + s.x, p.y + s.y };
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ p, color }, { b, color }, { c, color }, { c, color }, { b, color }, { d, color },
		});
	}
	void UIRenderer::drawTexturedQuad(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c, const UIVertTex& d) {
		texturedTriBuffer.insert(texturedTriBuffer.end(), { a, b, c, c, b, d, });
	}
	void UIRenderer::drawTexturedQuad(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c, const UIVertTex& d, const Color& color) {
		texturedTriBuffer.insert(texturedTriBuffer.end(), {
			{ a.pos, a.texCoord, a.color * color }, { b.pos, b.texCoord, b.color * color }, { c.pos, c.texCoord, c.color * color },
			{ c.pos, c.texCoord, c.color * color }, { b.pos, b.texCoord, b.color * color }, { d.pos, d.texCoord, d.color * color },
		});
	}
	void UIRenderer::drawTexturedQuad(const glm::vec3& p, const glm::vec2& s, const glm::vec2& minUV, const glm::vec2& maxUV) {
		const UIVertTex a { p, minUV };
		const UIVertTex b { { p.x, p.y + s.y, p.z }, { minUV.x, maxUV.y } };
		const UIVertTex c { { p.x + s.x, p.y, p.z }, { maxUV.x, minUV.y } };
		const UIVertTex d { { p.x + s.x, p.y + s.y, p.z }, maxUV };
		texturedTriBuffer.insert(texturedTriBuffer.end(), { a, b, c, c, b, d, });
	}
	void UIRenderer::drawTexturedQuad(const glm::vec2& p, const glm::vec2& s, const glm::vec2& minUV, const glm::vec2& maxUV) {
		const UIVertTex a { p, minUV };
		const UIVertTex b { { p.x, p.y + s.y }, { minUV.x, maxUV.y } };
		const UIVertTex c { { p.x + s.x, p.y }, { maxUV.x, minUV.y } };
		const UIVertTex d { { p.x + s.x, p.y + s.y }, maxUV };
		texturedTriBuffer.insert(texturedTriBuffer.end(), { a, b, c, c, b, d, });
	}
	void UIRenderer::drawTexturedQuad(const glm::vec3& p, const glm::vec2& s, const glm::vec2& minUV, const glm::vec2& maxUV, const Color& color) {
		const UIVertTex a { p, minUV, color };
		const UIVertTex b { { p.x, p.y + s.y, p.z }, { minUV.x, maxUV.y }, color };
		const UIVertTex c { { p.x + s.x, p.y, p.z }, { maxUV.x, minUV.y }, color };
		const UIVertTex d { { p.x + s.x, p.y + s.y, p.z }, maxUV, color };
		texturedTriBuffer.insert(texturedTriBuffer.end(), { a, b, c, c, b, d, });
	}
	void UIRenderer::drawTexturedQuad(const glm::vec2& p, const glm::vec2& s, const glm::vec2& minUV, const glm::vec2& maxUV, const Color& color) {
		const UIVertTex a { p, minUV, color  };
		const UIVertTex b { { p.x, p.y + s.y }, { minUV.x, maxUV.y }, color };
		const UIVertTex c { { p.x + s.x, p.y }, { maxUV.x, minUV.y }, color };
		const UIVertTex d { { p.x + s.x, p.y + s.y }, maxUV, color };
		texturedTriBuffer.insert(texturedTriBuffer.end(), { a, b, c, c, b, d, });
	}
	void UIRenderer::drawColoredTri(const UIVertColor& a, const UIVertColor& b, const UIVertColor& c) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), { a, b, c, });
	}
	void UIRenderer::drawColoredTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color },
		});
	}
	void UIRenderer::drawColoredTri(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color },
		});
	}
	void UIRenderer::drawTexturedTri(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c) {
		texturedTriBuffer.insert(texturedTriBuffer.end(), { a, b, c, });
	}
	void UIRenderer::drawTexturedTri(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c, const Color& color) {
		texturedTriBuffer.insert(texturedTriBuffer.end(), {
			{ a.pos, a.texCoord, a.color * color }, { b.pos, b.texCoord, b.color * color }, { c.pos, c.texCoord, c.color * color },
		});
	}
	void UIRenderer::drawLine(const UIVertColor& a, const UIVertColor& b) {
		lineBuffer.emplace_back(a);
		lineBuffer.emplace_back(b);
	}
	void UIRenderer::drawLine(const glm::vec3& a, const glm::vec3& b, const Color& color) {
		lineBuffer.emplace_back(a, color);
		lineBuffer.emplace_back(b, color);
	}
	void UIRenderer::drawLine(const glm::vec2& a, const glm::vec2& b, const Color& color) {
		lineBuffer.emplace_back(a, color);
		lineBuffer.emplace_back(b, color);
	}
	void UIRenderer::drawLines(std::initializer_list<UIVertColor> verts) {
		lineBuffer.insert(lineBuffer.end(), verts.begin(), verts.end());
	}
	void UIRenderer::drawLines(const Color& color, std::initializer_list<glm::vec3> positions) {
		for (const auto& p : positions)
			lineBuffer.emplace_back(p, color);
	}
	void UIRenderer::drawLines(const Color& color, std::initializer_list<glm::vec2> positions) {
		for (const auto& p : positions)
			lineBuffer.emplace_back(p, color);
	}

}