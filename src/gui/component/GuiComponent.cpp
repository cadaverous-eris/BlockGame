#include "GuiComponent.h"


#include "gui/Gui.h"

namespace eng {

    GuiComponent::GuiComponent(Gui& gui, const RectF& boundingRect, const float zIndex) noexcept :
            gui(&gui), boundingRect(boundingRect), zIndex(zIndex) {}

    bool GuiComponent::isFocused() const noexcept {
        return gui->getFocusedComponent() == this;
    }
    bool GuiComponent::isHovered() const noexcept {
        return boundingRect.contains(gui->getMousePos());
    }


}