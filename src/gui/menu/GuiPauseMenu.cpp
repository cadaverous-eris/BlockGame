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
            guiScaleSlider(*this, glm::vec2{}, 200)/*,
            textInputTest(*this, glm::vec2{}, 200, U"Placeholder Text")*/
    {
        components.emplace_back(&closeGuiButton);
        components.emplace_back(&exitGameButton);
        components.emplace_back(&guiScaleSlider);
        //components.emplace_back(&textInputTest);

        handleResize();
    }

    void GuiPauseMenu::handleResize() {
        Gui::handleResize();
        const glm::vec2 center = glm::vec2(width, height) / 2.0f;
        const float buttonSpacing = 10;
        const float scaleSliderOffset = 40;

        const glm::vec2 closeBtnSize = closeGuiButton.getSize();
        const glm::vec2 exitBtnSize = exitGameButton.getSize();
        const glm::vec2 guiScaleSliderSize = guiScaleSlider.getSize();
        //const glm::vec2 textInputTestSize = textInputTest.getSize();

        const float totalComponentsHeight = closeBtnSize.y + buttonSpacing + exitBtnSize.y + scaleSliderOffset + guiScaleSliderSize.y;
        //const float totalComponentsHeight = textInputTestSize.y + buttonSpacing + closeBtnSize.y + buttonSpacing + exitBtnSize.y + scaleSliderOffset + guiScaleSliderSize.y;

        float componentY = center.y - (totalComponentsHeight / 2);

        closeGuiButton.setPos({ center.x - (closeBtnSize.x / 2), componentY });
        componentY += closeBtnSize.y + buttonSpacing;
        exitGameButton.setPos({ center.x - (exitBtnSize.x / 2), componentY });
        componentY += exitBtnSize.y + scaleSliderOffset;
        guiScaleSlider.setPos({ center.x - (guiScaleSliderSize.x / 2), componentY });
        //componentY += guiScaleSliderSize.y + buttonSpacing;
        //textInputTest.setPos({ center.x - (textInputTestSize.x / 2), componentY });
    }

    void GuiPauseMenu::render(float partialTicks) {
        drawBackground();

        renderComponents(partialTicks);

        //if (!textInputTest.isFocused() && textInputTest.isMouseOver())
        //    drawTooltip(textInputTest.getU32Value());

        flushTextured();
    }

    bool GuiPauseMenu::handleInput() {
        if (Gui::handleInput()) return true;

        if (input::keybinds::PAUSE->didPress()) {
            game->closeGui();
            return true;
        }


        //if (input::keybinds::INVENTORY->didPress() && !focusedComponent)
        //    std::cout << textInputTest.getValue() << '\n';

        return false;
    }


}