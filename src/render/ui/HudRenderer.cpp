#include "HudRenderer.h"

#include "game_states/PlayState.h"
#include "render/font/FontRenderer.h"
#include "render/ui/UIRenderer.h"
#include "Game.h"

#include <iostream>
#include <glm/gtx/io.hpp>
#include <sstream>
#include <charconv>

namespace eng {

	HudRenderer::HudRenderer(Renderer* const renderer) : renderer(renderer) {}

	void HudRenderer::render(const float partialTicks, const PlayState& gameState) {
		auto& uiRenderer = *renderer->getUIRenderer();

		const glm::vec2& uiSize = uiRenderer.getUISize();
		//const glm::mat4& orthoMatrix = renderer->getOrthoMatrix();

		constexpr float crosshairSize = 16.0f;
		constexpr float crosshairScale = 1.0f;
		const glm::vec2 pMin = (uiSize / 2.0f) - (crosshairScale * crosshairSize / 2.0f);
		const glm::vec2 pMax = (uiSize / 2.0f) + (crosshairScale * crosshairSize / 2.0f);
		constexpr glm::vec2 uvMin { 0.0f, 0.0f };
		constexpr glm::vec2 uvMax { 1.0f / 16.0f, 1.0f / 16.0f };
		uiRenderer.drawTexturedQuad({ { pMin.x, pMin.y }, { uvMin.x, uvMin.y } },  // top left
									{ { pMin.x, pMax.y }, { uvMin.x, uvMax.y } },  // bottom left
									{ { pMax.x, pMin.y }, { uvMax.x, uvMin.y } },  // top right
									{ { pMax.x, pMax.y }, { uvMax.x, uvMax.y } }); // bottom right


		Texture::bind(hudTextures);
		uiRenderer.flushTextured();


		//std::ostringstream windowTitleStream;
		//windowTitleStream << "FPS: " << gameState.getGame()->getAvgFPS();
		//windowTitleStream.str()

		auto& fontRenderer = *renderer->getFontRenderer();

		constexpr auto blockNameFontSize = 14.0f;
		const auto blockNameLength = fontRenderer.getTextWidth(gameState.getSelectedBlock()->getName(), blockNameFontSize);
		const auto blockNameLH = fontRenderer.getLineHeight(blockNameFontSize);
		const glm::vec2 blockNameTextOrigin { (uiSize.x / 2) - (blockNameLength / 2), uiSize.y - 10.0f - blockNameLH };
		uiRenderer.drawColoredQuad(
			glm::vec3(blockNameTextOrigin + glm::vec2{ -1, 0 }, 1),
			glm::vec3(blockNameTextOrigin + glm::vec2{ -1, blockNameLH }, 1),
			glm::vec3(blockNameTextOrigin + glm::vec2{ blockNameLength + 1, 0 }, 1),
			glm::vec3(blockNameTextOrigin + glm::vec2{ blockNameLength + 1, blockNameLH }, 1),
			0x0000007F_c
		);
		uiRenderer.flushColored();
		fontRenderer.drawText(gameState.getSelectedBlock()->getName(), blockNameTextOrigin, blockNameFontSize, 0xFFF_c, { /*0.0f, 2.5f, 0x000000BF_c*/ }, true);

		using namespace std::string_literals;
		using namespace std::string_view_literals;
		/*const auto testingText = u8"The quick brown fox jumps over the lazy dog. \u0394\u0398\u03A3\u03A6\u03A8\u03A9 \U0001F480 \U0001F571 \U0001F440 \U0001F435\U0001F44D \U0001F4A3 \U0001F494 \U0001F607 \U0001F608 \U0001F5E1\U0001F30E \U0001F631 \u26E7\u26B8 \u26A5 \u16C3 \u2620 \U0001F9B4";
		const auto testingText2 = u8"I \U0001F5A4 Unicode \U00013010\U000130B8 \u26E7";
		const float fontSizes[] { 24 };// { 128, 96, 64, 48, 36, 26, 18, 16, 14, 12 };
		glm::vec2 pos { 10, 10 };
		for (const auto fontSize : fontSizes) {
			std::ostringstream sstr;
			sstr << fontSize << " " << (char*) testingText;
			fontRenderer.drawText(sstr.str().c_str(), pos, fontSize, 0xFFF_c, { 2.0f, 0.0f, 0x000000BF_c });
			//fontRenderer.drawString(testingText, pos, fontSize, 0xFFF_c, { 1.0f, 2.5f, 0x000000BF_c });
			pos.y += fontRenderer.getLineHeight(fontSize);
		}*/

		constexpr auto debugInfoFontSize = 12.0f;
		const auto lineHeight = fontRenderer.getLineHeight(debugInfoFontSize);
		const auto fps = gameState.getGame()->getFPS();
		std::string fpsStr = "fps:      "s;
		if (auto [p, ec] = std::to_chars(fpsStr.data() + 5, fpsStr.data() + fpsStr.size(), fps); ec == std::errc()) {
			const auto fpsStrV = std::string_view(fpsStr.data(), p - fpsStr.data());
			fontRenderer.drawText(fpsStrV, glm::vec2(10), debugInfoFontSize, 0xFFF_c, { 1.0f, 2.5f, 0x000000BF_c });
		}
		const Direction cameraDir = direction::getDirection(gameState.getCamera().getLookVec());
		if (cameraDir != Direction::UNDEFINED) {
			const auto dirStr = "Looking: " + to_string(cameraDir);
			fontRenderer.drawText(dirStr, glm::vec2(10, 10 + lineHeight), debugInfoFontSize, 0xFFF_c, { 1.0f, 2.5f, 0x000000BF_c });
		}

		fontRenderer.flush();
	}

}