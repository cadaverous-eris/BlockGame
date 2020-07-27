#pragma once

#include <vector>
#include <initializer_list>
#include <iterator>
#include <type_traits>

#include "render/VertexArray.h"
#include "render/VertexBuffer.h"
#include "render/ShaderProgram.h"
#include "UIVert.h"
#include "render/Renderer.h"

namespace eng {

	class UIRenderer {
	public:
		enum class LineMode {
			LINES = GL_LINES,
			LINE_LOOP = GL_LINE_LOOP,
			LINE_STRIP = GL_LINE_STRIP,
		};
		enum class TriMode {
			TRIANGLES = GL_TRIANGLES,
			TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
			TRIANGLE_FAN = GL_TRIANGLE_FAN,
		};
		static constexpr DrawMode toDrawMode(const LineMode mode) {
			switch (mode) {
			case LineMode::LINES:
				return DrawMode::LINES;
			case LineMode::LINE_LOOP:
				return DrawMode::LINE_LOOP;
			case LineMode::LINE_STRIP:
				return DrawMode::LINE_STRIP;
			default:
				return DrawMode::LINES;
			}
		}
		static constexpr DrawMode toDrawMode(const TriMode mode) {
			switch (mode) {
			case TriMode::TRIANGLES:
				return DrawMode::TRIANGLES;
			case TriMode::TRIANGLE_STRIP:
				return DrawMode::TRIANGLE_STRIP;
			case TriMode::TRIANGLE_FAN:
				return DrawMode::TRIANGLE_FAN;
			default:
				return DrawMode::TRIANGLES;
			}
		}
	private:
		Renderer* renderer;

		VertexArray coloredVAO;
		VertexBuffer coloredVBO;
		VertexArray lineVAO;
		VertexBuffer lineVBO;
		VertexArray texturedVAO;
		VertexBuffer texturedVBO;

		ShaderProgram colorShader = ShaderProgram::load("ui/ui_colored.vert", "ui/ui_colored.frag");
		ShaderProgram textureShader = ShaderProgram::load("ui/ui_textured.vert", "ui/ui_textured.frag");

		std::vector<UIVertColor> coloredTriBuffer, lineBuffer;
		std::vector<UIVertTex> texturedTriBuffer;

	public:
		UIRenderer(Renderer* const);

		UIRenderer(const UIRenderer&) = delete;
		UIRenderer& operator =(const UIRenderer&) = delete;
		UIRenderer(UIRenderer&&) = default;
		UIRenderer& operator =(UIRenderer&&) = default;

		const glm::ivec2& getUISize() const { return renderer->getWindowSizeScaled(); }
		const glm::vec2& getContentScale() const { return renderer->getWindowContentScale(); }

		void drawQuad(const UIVertColor&, const UIVertColor&, const UIVertColor&, const UIVertColor&);
		void drawQuad(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, const Color& color);
		void drawQuad(const glm::vec2&, const glm::vec2&, const glm::vec2&, const glm::vec2&, const Color& color);
		void drawQuad(const UIVertTex&, const UIVertTex&, const UIVertTex&, const UIVertTex&);
		void drawQuad(const UIVertTex&, const UIVertTex&, const UIVertTex&, const UIVertTex&, const Color& color);
		void drawTri(const UIVertColor&, const UIVertColor&, const UIVertColor&);
		void drawTri(const glm::vec3&, const glm::vec3&, const glm::vec3&, const Color& color);
		void drawTri(const glm::vec2&, const glm::vec2&, const glm::vec2&, const Color& color);
		void drawTri(const UIVertTex&, const UIVertTex&, const UIVertTex&);
		void drawTri(const UIVertTex&, const UIVertTex&, const UIVertTex&, const Color& color);
		void drawLine(const UIVertColor&, const UIVertColor&);
		void drawLine(const glm::vec3&, const glm::vec3&, const Color& color);
		void drawLine(const glm::vec2&, const glm::vec2&, const Color& color);
		void drawLines(std::initializer_list<UIVertColor>);
		void drawLines(const Color& color, std::initializer_list<glm::vec3>);
		void drawLines(const Color& color, std::initializer_list<glm::vec2>);
		template<typename Iter, std::enable_if_t<std::is_same_v<std::remove_cv_t<std::remove_reference_t<typename std::iterator_traits<Iter>::value_type>>, UIVertColor>>* = nullptr>
		void drawLines(Iter begin, Iter end) {
			lineBuffer.insert(lineBuffer.end(), begin, end);
		}
		template<typename Iter, std::enable_if_t<std::is_same_v<std::remove_cv_t<std::remove_reference_t<typename std::iterator_traits<Iter>::value_type>>, glm::vec3>>* = nullptr>
		void drawLines(const Color& color, Iter begin, Iter end) {
			for (auto it = begin; it != end; it++)
				lineBuffer.emplace_back(*it, color);
		}
		template<typename Iter, std::enable_if_t<std::is_same_v<std::remove_cv_t<std::remove_reference_t<typename std::iterator_traits<Iter>::value_type>>, glm::vec2>>* = nullptr>
		void drawLines(const Color& color, Iter begin, Iter end) {
			for (auto it = begin; it != end; it++)
				lineBuffer.emplace_back(*it, color);
		}

		void render(const float partialTicks);

		void flushColored(const glm::mat4& matrix, const TriMode mode = TriMode::TRIANGLES);
		inline void flushColored(const TriMode mode = TriMode::TRIANGLES) { flushColored(renderer->getOrthoMatrix(), mode); }
		void flushLines(const glm::mat4& matrix, const LineMode mode = LineMode::LINES);
		inline void flushLines(const LineMode mode = LineMode::LINES) { flushLines(renderer->getOrthoMatrix(), mode); }
		void flushTextured(const glm::mat4& matrix, int textureUnit = 0);
		inline void flushTextured(int textureUnit = 0) { flushTextured(renderer->getOrthoMatrix(), textureUnit); }

	private:


	};

}