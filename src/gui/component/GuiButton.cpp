#include "GuiButton.h"


#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"

namespace eng {

    const int GuiAbstractButton::default_button_height = 20;
    const int GuiAbstractButton::default_button_slice_width = 16;


    void GuiAbstractButton::render(float) {
        const bool hovered = isHovered();
        const glm::vec3 origin(boundingRect.offset, zIndex);
        const float innerWidth = boundingRect.size.x - (2.0f * default_button_slice_width);
        const glm::vec2 innerSize { innerWidth, default_button_height };
        const glm::vec2 minUV { 0, (hovered ? 72 : 52) };
        gui->drawHorizontal3Slice(origin, default_button_slice_width, innerSize, minUV, { 256, 256 });
        // TODO: actually render the image?
        //Texture::bind(gui->getGuiTextures());
        //gui->getUIRenderer().flushTextured();
        const auto text = getDisplayString();
        const auto fontSize = gui->getFontRenderer().getFontSizeForLineHeight(14.0f);
        const auto textWidth = gui->getFontRenderer().getTextWidth(text, fontSize);
        const Color textColor = (hovered ? color::gold : color::lightgray);
        const float centerX = boundingRect.offset.x + (boundingRect.size.x / 2);
        const glm::vec3 textOrigin = glm::floor(glm::vec3(centerX - (textWidth / 2), boundingRect.offset.y + 3.0f, zIndex - 1));
        gui->getFontRenderer().drawText(text, textOrigin, fontSize, textColor, {}, false);
    }


}