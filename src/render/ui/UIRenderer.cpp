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

	void UIRenderer::flushColored(const glm::mat4& matrix, const TriMode mode) {
		if (!coloredTriBuffer.empty()) {
			colorShader.bind();
			colorShader.setUniform("matrix", matrix);

			glEnable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			coloredVAO.bind();
			coloredVBO.setData(std::span(coloredTriBuffer.data(), coloredTriBuffer.size()));
			coloredVAO.draw(toDrawMode(mode), 0, coloredTriBuffer.size());

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			coloredTriBuffer.clear();
		}
	}
	void UIRenderer::flushLines(const glm::mat4& matrix, const LineMode mode) {
		if (!lineBuffer.empty()) {
			colorShader.bind();
			colorShader.setUniform("matrix", matrix);

			glEnable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			lineVAO.bind();
			lineVBO.setData(std::span(lineBuffer.data(), lineBuffer.size()));
			lineVAO.draw(toDrawMode(mode), 0, lineBuffer.size());

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			lineBuffer.clear();
		}
	}
	void UIRenderer::flushTextured(const glm::mat4& matrix, int textureUnit) {
		if (!texturedTriBuffer.empty()) {
			textureShader.bind();
			textureShader.setUniform("matrix", matrix);
			textureShader.setUniform("textureSampler", textureUnit);

			glEnable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			texturedVAO.bind();
			texturedVBO.setData(std::span(texturedTriBuffer.data(), texturedTriBuffer.size()));
			texturedVAO.draw(DrawMode::TRIANGLES, 0, texturedTriBuffer.size());

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			texturedTriBuffer.clear();
		}
	}

	void UIRenderer::render(const float partialTicks) {
		flushColored();
		flushLines();
		texturedTriBuffer.clear(); // don't render the textured tris since there's no way of knowing if the correct texture is bound
	}
	
	void UIRenderer::drawQuad(const UIVertColor& a, const UIVertColor& b, const UIVertColor& c, const UIVertColor& d) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			a, b, c, c, b, d,
		});
	}
	void UIRenderer::drawQuad(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color }, { c, color }, { b, color }, { d, color },
		});
	}
	void UIRenderer::drawQuad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color }, { c, color }, { b, color }, { d, color },
		});
	}
	void UIRenderer::drawQuad(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c, const UIVertTex& d) {
		texturedTriBuffer.insert(texturedTriBuffer.end(), {
			a, b, c, c, b, d,
		});
	}
	void UIRenderer::drawQuad(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c, const UIVertTex& d, const Color& color) {
		texturedTriBuffer.insert(texturedTriBuffer.end(), {
			{ a.pos, a.texCoord, a.color * color }, { b.pos, b.texCoord, b.color * color }, { c.pos, c.texCoord, c.color * color },
			{ c.pos, c.texCoord, c.color * color }, { b.pos, b.texCoord, b.color * color }, { d.pos, d.texCoord, d.color * color },
		});
	}
	void UIRenderer::drawTri(const UIVertColor& a, const UIVertColor& b, const UIVertColor& c) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			a, b, c,
		});
	}
	void UIRenderer::drawTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color },
		});
	}
	void UIRenderer::drawTri(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const Color& color) {
		coloredTriBuffer.insert(coloredTriBuffer.end(), {
			{ a, color }, { b, color }, { c, color },
		});
	}
	void UIRenderer::drawTri(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c) {
		texturedTriBuffer.insert(texturedTriBuffer.end(), {
			a, b, c,
		});
	}
	void UIRenderer::drawTri(const UIVertTex& a, const UIVertTex& b, const UIVertTex& c, const Color& color) {
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