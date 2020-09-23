#pragma once

#include <memory>
#include <iostream>

#include "GuiMenu.h"
#include "Game.h"
#include "gui/component/GuiButton.h"
#include "gui/component/GuiSlider.h"
#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"
#include "input/key_input.h"
#include "input/KeyBind.h"

#include "gui/component/GuiTextInput.h"

namespace eng {

    class GuiScaleSlider : public GuiAbstractSlider {
    protected:
        int32_t prevIntValue;
        int32_t intValue;
        std::string displayText;

        static int32_t getIntVal(float floatVal);
        static float getFloatVal(int32_t intVal);

        static std::string getDisplayText(int32_t intVal);

    public:
        GuiScaleSlider(Gui& gui, const glm::vec2& pos, float width) :
                GuiAbstractSlider(gui, pos, width, getFloatVal(Game::instance().settings.uiScale)),
                prevIntValue(Game::instance().settings.uiScale),
                intValue(prevIntValue),
                displayText(getDisplayText(intValue)) {}

    protected:
        void onValueChanged() override;
        void onDrag() override;

        inline std::string_view getDisplayString() const override { return displayText; }
    };


    class GuiPauseMenu : public GuiMenu {
    protected:
        GuiButton closeGuiButton;
        GuiButton exitGameButton;
        GuiScaleSlider guiScaleSlider;
        //GuiTextInput textInputTest;

    public:
        GuiPauseMenu();

        void handleResize() override;

        void render(float partialTicks) override;

        bool handleInput() override;


    };

}