#include "GuiPauseMenu.h"

#include <algorithm>
#include <iostream>

#include <fmt/core.h>

namespace eng {

    int32_t GuiScaleSlider::getIntVal(float floatVal) {
        const int32_t minScale = Settings::min_ui_scale;
        const int32_t maxScale = Settings::max_ui_scale;
        const int32_t intVal = minScale + static_cast<int32_t>(std::round(floatVal * static_cast<float>(maxScale - minScale)));
        return std::clamp(intVal, minScale, maxScale);
    }
    float GuiScaleSlider::getFloatVal(int32_t intVal) {
        constexpr int32_t minScale = Settings::min_ui_scale;
        constexpr int32_t maxScale = Settings::max_ui_scale;
        return clampFloatValue(static_cast<float>(intVal - minScale) / static_cast<float>(maxScale - minScale));
    }

    std::string GuiScaleSlider::getDisplayText(int32_t intVal) {
        return fmt::format("GUI Scale: {}", intVal);
    }

    void GuiScaleSlider::onValueChanged() {
        prevIntValue = intValue;
        floatVal = getFloatVal(intValue);
        Game& game = Game::instance();
        if (intValue != game.settings.uiScale) {
            game.settings.uiScale = intValue;
            game.applySettingsChanges();
        }
    }
    void GuiScaleSlider::onDrag() {
        prevIntValue = intValue;
        intValue = getIntVal(floatVal);
        if (intValue != prevIntValue)
            displayText = getDisplayText(intValue);
    }



    GuiPauseMenu::GuiPauseMenu() :
            GuiMenu("pause_menu"),
            closeGuiButton(*this, glm::vec2{}, 200, "Return to Game", [this]() { this->game->closeGui(); }),
            exitGameButton(*this, glm::vec2{}, 200, "Quit Game", [this]() { this->game->quit(); }),
            guiScaleSlider(*this, glm::vec2{}, 200)
    {
        components.emplace_back(&closeGuiButton);
        components.emplace_back(&exitGameButton);
        components.emplace_back(&guiScaleSlider);

        handleResize();
    }

    void GuiPauseMenu::handleResize() {
        Gui::handleResize();
        //const glm::vec2 center = glm::vec2(width - 220, static_cast<float>(height) / 2.0f);
        const glm::vec2 center = glm::vec2(width, height) / 2.0f;
        const float buttonSpacing = 10;
        const float scaleSliderOffset = 40;

        const glm::vec2 closeBtnSize = closeGuiButton.getSize();
        const glm::vec2 exitBtnSize = exitGameButton.getSize();
        const glm::vec2 guiScaleSliderSize = guiScaleSlider.getSize();

        const float totalComponentsHeight = closeBtnSize.y + buttonSpacing + exitBtnSize.y + scaleSliderOffset + guiScaleSliderSize.y;

        float componentY = center.y - (totalComponentsHeight / 2);

        closeGuiButton.setPos({ center.x - (closeBtnSize.x / 2), componentY });
        componentY += closeBtnSize.y + buttonSpacing;
        exitGameButton.setPos({ center.x - (exitBtnSize.x / 2), componentY });
        componentY += exitBtnSize.y + scaleSliderOffset;
        guiScaleSlider.setPos({ center.x - (guiScaleSliderSize.x / 2), componentY });
        /*closeGuiButton.setPos({ center.x, componentY });
        componentY += closeBtnSize.y + buttonSpacing;
        exitGameButton.setPos({ center.x, componentY });
        componentY += exitBtnSize.y + scaleSliderOffset;
        guiScaleSlider.setPos({ center.x, componentY });*/
    }

    void GuiPauseMenu::render(float partialTicks) {
        drawBackground();

        renderComponents(partialTicks);

        // Multiline text test begin
        /*const std::u32string textPanelText = UR"(Suspendisse convallis justo a leo hendrerit suscipit. Donec eget bibendum leo. Sed vestibulum aliquam eleifend. Aliquam efficitur lectus quis nibh sollicitudin scelerisque. Aliquam pulvinar ex non quam feugiat pharetra. Nunc convallis leo quis accumsan ultricies. Maecenas tristique nisi ut mauris congue accumsan. Suspendisse semper erat ut lectus auctor tristique.

Sed libero ipsum, consectetur id dolor eget, sollicitudin vestibulum dolor. Vestibulum non faucibus tortor. Nulla semper odio in dolor hendrerit sagittis. Suspendisse fringilla imperdiet mollis. Phasellus in velit at metus egestas vehicula ac et justo. Suspendisse viverra non purus in condimentum. In non fringilla metus. Nulla rhoncus porta facilisis. Nunc consectetur interdum auctor. Integer viverra lacinia enim sollicitudin pulvinar. Donec malesuada velit ac lacus vulputate, vel tempor sapien convallis. Proin quis diam sed justo tincidunt fringilla.

Ut tempor egestas eros sit amet tincidunt. Aenean semper sem vel rhoncus volutpat. In ut leo lobortis, porttitor turpis id, viverra massa. Vivamus augue tortor, pulvinar eget sagittis id, egestas vitae sapien. Proin venenatis odio eu congue feugiat. Suspendisse justo lectus, pretium vel consequat vitae, pretium ac eros. In tincidunt tempor gravida.)";
        const float fontSize = 9.0f;
        const float maxTextAreaHeight = 0;//static_cast<float>(height - 48);
        const float textAreaWidth = static_cast<float>(width - 268);//static_cast<float>(width - 48) * 0.5f;
        const FontRenderer::MultilineConfig multilineConfig {
            { textAreaWidth, maxTextAreaHeight }, 0, TextAlign::Left
        };
        const auto textWrapRes = fontRenderer->splitLines(textPanelText, fontSize, multilineConfig);
        const glm::vec2 textPanelInnerArea { textAreaWidth, textWrapRes.height };
        draw9Slice(glm::vec3(20, 20, 5), 4, textPanelInnerArea, { 88, 0 }, { 256, 256 });
        fontRenderer->drawMultilineText(textWrapRes, multilineConfig, glm::vec3(24, 24, 4), fontSize, color::white, {}, true);
        */// Multiline text test end

        Texture::bind(guiTexture);
        uiRenderer->flushTextured();
    }

    bool GuiPauseMenu::handleInput() {
        if (Gui::handleInput()) return true;

        if (input::keybinds::PAUSE->didPress()) {
            game->closeGui();
            return true;
        }

        return false;
    }


}