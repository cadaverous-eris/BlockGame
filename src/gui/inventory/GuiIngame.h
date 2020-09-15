#pragma once

#include <utility>


#include "gui/Gui.h"

namespace eng {

    class GuiIngame : public Gui {
    protected:


    public:
        GuiIngame(const std::string& guiId) : Gui(guiId) {}
        GuiIngame(std::string&& guiId) : GuiIngame((guiId)) {}


        bool shouldPauseGame() const noexcept override { return false; }
        bool isTransparent() const noexcept override { return true; }

    protected:

        void drawBackground() const;


    };

}