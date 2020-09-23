#include "GuiSlider.h"

#include <algorithm>

#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"

namespace eng {

    const int GuiAbstractSlider::default_slider_height = 20;
    const int GuiAbstractSlider::default_slider_slice_width = 16;
    const int GuiAbstractSlider::default_thumb_height = 20;
    const int GuiAbstractSlider::default_thumb_width = 8;

    const float GuiAbstractSlider::min_float_value = 0.0f;
    const float GuiAbstractSlider::max_float_value = 1.0f;

    float GuiAbstractSlider::clampFloatValue(const float& floatVal) {
        return std::clamp(floatVal, min_float_value, max_float_value);
    }

    void GuiAbstractSlider::render(float) {
        const bool doHighlight = !disabled && (isDragging || isMouseOver());
        const glm::vec3 origin(boundingRect.offset, zIndex);
        const float innerWidth = boundingRect.size.x - (2.0f * default_slider_slice_width);
        const glm::vec2 innerSize { innerWidth, default_slider_height };
        const glm::vec2 minUV { 0, (doHighlight ? 112 : 92) };
        gui->drawHorizontal3Slice(origin, default_slider_slice_width, innerSize, minUV, { 256, 256 });

        const glm::vec3 thumbPos { origin.x + (floatVal * (boundingRect.size.x - thumbSize.x)), origin.y, zIndex - 1 };
        gui->drawImage(thumbPos, thumbSize, { 48, (disabled ? 112 : 132) }, { 256, 256 });

        // TODO: actually render the images?
        //Texture::bind(gui->getGuiTextures());
        //gui->getUIRenderer().flushTextured();
        const auto text = getDisplayString();
        const auto fontSize = gui->getFontRenderer().getFontSizeForLineHeight(14.0f);
        const auto textWidth = gui->getFontRenderer().getTextWidth(text, fontSize);
        const Color textColor = (disabled ? color::gray : (doHighlight ? color::gold : color::lightgray));
        const float centerX = boundingRect.offset.x + (boundingRect.size.x / 2);
        const glm::vec3 textOrigin = glm::floor(glm::vec3(centerX - (textWidth / 2), boundingRect.offset.y + 3.0f, zIndex - 2));
        gui->getFontRenderer().drawText(text, textOrigin, fontSize, textColor, {}, false);
    }

}