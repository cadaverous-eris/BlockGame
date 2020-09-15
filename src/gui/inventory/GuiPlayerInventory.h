#pragma once

#include <memory>
#include <iostream>

#include "GuiIngame.h"
#include "Game.h"
#include "gui/component/GuiButton.h"
#include "gui/component/GuiSlider.h"
#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"
#include "input/key_input.h"
#include "input/KeyBind.h"

namespace eng {

    class GuiPlayerInventory : public GuiIngame {
    protected:
        GuiButton closeGuiButton;
        GuiButton exitGameButton;

    public:
        GuiPlayerInventory();

        void handleResize() override;

        void render(float partialTicks) override;

        bool handleInput() override;


    };

}