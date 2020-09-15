#pragma once

#include <utility>


#include "gui/Gui.h"

namespace eng {

    class GuiMenu : public Gui {
    protected:
        Texture bgTexture = Texture::loadTexture("gui/menu_background.png")
                                    .scaleNearest().setWrapMode(TextureWrapMode::REPEAT);



    public:
        GuiMenu(const std::string& guiId) : Gui(guiId) {}
        GuiMenu(std::string&& guiId) : Gui(std::move(guiId)) {}


        bool shouldPauseGame() const noexcept override { return true; }
        bool isTransparent() const noexcept override { return false; }


    protected:

        void drawBackground() const;


    };

}