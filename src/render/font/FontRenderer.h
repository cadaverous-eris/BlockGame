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

	struct FontOutline {
		float size = 0.0f;
		float spread = 0.0f;
		Color color { 0, 0, 0, 0 };
	};

	class Renderer;

	class FontRenderer {
	public:
		static constexpr float sdf_size = 32.0f;
		static constexpr int16_t sdf_padding = 6;
		static constexpr unsigned char sdf_on_edge_value = 180;
		static constexpr float sdf_pixel_dist_scale = 30.0f;

		// struct containing info about the size of drawn text
		struct TextSizeResult {
			float width, height;
			int lines = 1;
		};

		using LineVert = UIVertColor;

		using FontQuadBuffer = std::vector<FontQuad>;
		using LineBuffer = std::vector<LineVert>;

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

		FontQuadBuffer fontQuadBuffer;
		LineBuffer underlineBuffer;

	public:

		FontRenderer(Renderer* const);

		FontRenderer(const FontRenderer&) = delete;
		FontRenderer(FontRenderer&&) = default;
		FontRenderer& operator =(const FontRenderer&) = delete;
		FontRenderer& operator =(FontRenderer&&) = default;


		void render(const float partialTicks);

		void flush();


		inline float drawText(std::string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const FontOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(fontQuadBuffer, underlineBuffer, text, origin, fontSize, color, outline, underline);
		}
		inline float drawText(std::u8string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const FontOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(fontQuadBuffer, underlineBuffer, text, origin, fontSize, color, outline, underline);
		}
		inline float drawText(std::u32string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const FontOutline& outline = {}, const bool underline = false) {
			return drawTextToBuffer(fontQuadBuffer, underlineBuffer, text, origin, fontSize, color, outline, underline);
		}

		inline float drawTextToBuffer(FontQuadBuffer& quadBuffer, LineBuffer& lineBuffer, std::string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const FontOutline& outline = {}, bool underline = false) const {
			return drawTextToBuffer(quadBuffer, lineBuffer, unicode::utf8ToUtf32(text), origin, fontSize, color, outline, underline);
		}
		inline float drawTextToBuffer(FontQuadBuffer& quadBuffer, LineBuffer& lineBuffer, std::u8string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const FontOutline& outline = {}, bool underline = false) const {
			return drawTextToBuffer(quadBuffer, lineBuffer, unicode::utf8ToUtf32(text), origin, fontSize, color, outline, underline);
		}
		float drawTextToBuffer(FontQuadBuffer& quadBuffer, LineBuffer& lineBuffer, std::u32string_view text, const glm::vec2& origin, float fontSize, const Color& color = 0xFFF_c, const FontOutline& outline = {}, bool underline = false) const;


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

	protected:



	};


}