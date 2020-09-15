#include "GuiPlayerInventory.h"

#include <algorithm>
#include <iostream>

#include <fmt/core.h>

namespace eng {

    GuiPlayerInventory::GuiPlayerInventory() :
            GuiIngame("player_inventory"),
            closeGuiButton(*this, glm::vec2{}, 200, "Return to Game", [this]() { this->game->closeGui(); }),
            exitGameButton(*this, glm::vec2{}, 200, "Quit Game", [this]() { this->game->quit(); })
    {
        components.emplace_back(&closeGuiButton);
        components.emplace_back(&exitGameButton);

        handleResize();
    }

    void GuiPlayerInventory::handleResize() {
        Gui::handleResize();

        const glm::vec2 center = glm::vec2(width, height) / 2.0f;
        const float buttonSpacing = 20;

        const glm::vec2 closeBtnSize = closeGuiButton.getSize();
        const glm::vec2 exitBtnSize = exitGameButton.getSize();

        const float totalComponentsHeight = closeBtnSize.y + buttonSpacing + exitBtnSize.y;

        float componentY = center.y - (totalComponentsHeight / 2);

        closeGuiButton.setPos({ center.x - (closeBtnSize.x / 2), componentY });
        componentY += closeBtnSize.y + buttonSpacing;
        exitGameButton.setPos({ center.x - (exitBtnSize.x / 2), componentY });
    }

    void GuiPlayerInventory::render(float partialTicks) {
        drawBackground();

        renderComponents(partialTicks);

        Texture::bind(guiTexture);
        uiRenderer->flushTextured();
    }

    bool GuiPlayerInventory::handleInput() {
        if (Gui::handleInput()) return true;

        if (input::keybinds::INVENTORY->didPress()) {
            game->closeGui();
            return true;
        }
        if (input::keybinds::PAUSE->didPress()) {
            game->closeGui();
            return true;
        }

        return false;
    }


}