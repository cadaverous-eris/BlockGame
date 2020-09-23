#include "GuiScrollBar.h"

#include <algorithm>

#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"

namespace eng {

    const int GuiAbstractScrollBar::default_scroll_bar_width = 16;
    const int GuiAbstractScrollBar::default_scroll_bar_slice_width = 8;
    const int GuiAbstractScrollBar::default_thumb_height = 16;
    const int GuiAbstractScrollBar::default_thumb_width = 16;
    const float GuiAbstractScrollBar::min_thumb_pos = 0.0f;
    const float GuiAbstractScrollBar::max_thumb_pos = 1.0f;

    float GuiAbstractScrollBar::clampScrollPos(const float& scrollPos) {
        return std::clamp(scrollPos, min_thumb_pos, max_thumb_pos);
    }


    void GuiVerticalScrollBar::render(float) {
        const glm::vec3 origin(boundingRect.offset, zIndex);
        const float innerHeight = boundingRect.size.y - (2.0f * default_scroll_bar_slice_width);
        const glm::vec2 innerSize { default_scroll_bar_width, innerHeight };
        const glm::vec2 minUV { 32, 0 };
        const glm::vec3 thumbOrig { origin.x, origin.y + (scrollPos * (boundingRect.size.y - thumbSize.y)), zIndex - 1 };
        gui->drawVertical3Slice(origin, default_scroll_bar_slice_width, innerSize, minUV, { 256, 256 });
        gui->drawImage(thumbOrig, thumbSize, { 48, (disabled ? 16 : 0) }, { 256, 256 });
        // TODO: actually render the images?
        //Texture::bind(gui->getGuiTextures());
        //gui->getUIRenderer().flushTextured();
    }

    void GuiHorizontalScrollBar::render(float) {
        const glm::vec3 origin(boundingRect.offset, zIndex);
        const float innerWidth = boundingRect.size.x - (2.0f * default_scroll_bar_slice_width);
        const glm::vec2 innerSize { innerWidth, default_scroll_bar_width };
        const glm::vec2 minUV { 64, 0 };
        const glm::vec3 thumbOrig { origin.x + (scrollPos * (boundingRect.size.x - thumbSize.x)), origin.y, zIndex - 1 };
        gui->drawHorizontal3Slice(origin, default_scroll_bar_slice_width, innerSize, minUV, { 256, 256 });
        gui->drawImage(thumbOrig, thumbSize, { (disabled ? 80 : 64), 16 }, { 256, 256 });
        // TODO: actually render the images?
        //Texture::bind(gui->getGuiTextures());
        //gui->getUIRenderer().flushTextured();
    }

}