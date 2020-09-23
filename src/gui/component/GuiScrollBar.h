#pragma once

#include <utility>

#include "GuiComponent.h"
#include "gui/Gui.h"
#include "util/math/math.h"
#include "util/type_traits.h"

namespace eng {

    enum class ScrollBarOrientation {
        Vertical, Horizontal
    };

    class GuiAbstractScrollBar : public GuiComponent {
    public:
        using Orientation = ScrollBarOrientation;

    protected:
        static const int default_scroll_bar_width;
        static const int default_scroll_bar_slice_width;

        static const int default_thumb_height;
        static const int default_thumb_width;

        static const float min_thumb_pos;
        static const float max_thumb_pos;

        static float clampScrollPos(const float& scrollPos);

        glm::vec2 thumbSize;
        float scrollPos;
        bool isDragging = false;

    public:
        GuiAbstractScrollBar(Gui& gui, const RectF& boundingRect, const glm::vec2& thumbSize, float scrollPos, float zIndex = default_z_index) noexcept :
                GuiComponent(gui, boundingRect, zIndex), thumbSize(thumbSize), scrollPos(clampScrollPos(scrollPos)) {}


        float getScrollPos() const noexcept { return scrollPos; }
        virtual void setScrollPos(float newScrollPos) {
            const float prevScrollPos = this->scrollPos;
            this->scrollPos = clampScrollPos(newScrollPos);
            if (prevScrollPos != this->scrollPos) onDrag();
        }

        virtual Orientation getOrientation() const = 0;

        virtual float getWidth() const { return boundingRect.size.x; }
        virtual float getHeight() const { return boundingRect.size.y; }

        virtual float getScrollOffset(const float contentSize, const float viewSize) const {
            const auto scrollRange = vmax(contentSize - viewSize, 0);
            return scrollPos * scrollRange;
        }


        inline bool handleInput() override {
            if (isDragging)
                updateScrollPos();

            return false;
        }

        inline bool onMouseDown(const input::KeyInput& keyInput) override {
            if (keyInput.key == input::MouseButtons::MOUSE_LEFT) {
                isDragging = true;
                updateScrollPos();
            }
            return true;
        }
        inline bool onMouseUp(const input::KeyInput& keyInput) override {
            if (isDragging && (keyInput.key == input::MouseButtons::MOUSE_LEFT)) {
                updateScrollPos();
                isDragging = false;
            }
            return true;
        }
        inline void onBlur() override {
            if (isDragging) {
                updateScrollPos();
                isDragging = false;
            }
        }

    protected:
        // called whenever the the thumb is moved
        virtual inline void onDrag() {}

        // sets the scroll position based on the current mouse position
        virtual void updateScrollPos() = 0;

    };


    class GuiVerticalScrollBar : public GuiAbstractScrollBar {
    public:
        GuiVerticalScrollBar(Gui& gui, const glm::vec2& pos, float height, float scrollPos, float zIndex = default_z_index) noexcept :
                GuiAbstractScrollBar(gui, { pos, { default_scroll_bar_width, vmax(height, default_scroll_bar_slice_width * 2) } }, { default_thumb_width, default_thumb_height }, scrollPos, zIndex) {}
        GuiVerticalScrollBar(Gui& gui, const glm::vec3& pos, float height, float scrollPos) noexcept :
                GuiAbstractScrollBar(gui, { { pos.x, pos.y }, { default_scroll_bar_width, vmax(height, default_scroll_bar_slice_width * 2) } }, { default_thumb_width, default_thumb_height }, scrollPos, pos.z) {}

        inline Orientation getOrientation() const override { return Orientation::Vertical; }

        virtual void setHeight(float height) {
            boundingRect.size.y = vmax(height, default_scroll_bar_slice_width * 2);
        }


        void render(float partialTicks) override;

    protected:
        void updateScrollPos() override {
            const float scrollPosRange = boundingRect.size.y - thumbSize.y;
            const float newScrollPos = clampScrollPos((gui->getMousePos().y - (thumbSize.y / 2.0f) - boundingRect.offset.y) / scrollPosRange);
            const bool moved = newScrollPos != scrollPos;
            scrollPos = newScrollPos;
            if (moved) onDrag();
        }

    };

    class GuiHorizontalScrollBar : public GuiAbstractScrollBar {
    public:
        GuiHorizontalScrollBar(Gui& gui, const glm::vec2& pos, float width, float scrollPos, float zIndex = default_z_index) noexcept :
                GuiAbstractScrollBar(gui, { pos, { vmax(width, default_scroll_bar_slice_width * 2), default_scroll_bar_width } }, { default_thumb_width, default_thumb_height }, scrollPos, zIndex) {}
        GuiHorizontalScrollBar(Gui& gui, const glm::vec3& pos, float width, float scrollPos) noexcept :
                GuiAbstractScrollBar(gui, { { pos.x, pos.y }, { vmax(width, default_scroll_bar_slice_width * 2), default_scroll_bar_width } }, { default_thumb_width, default_thumb_height }, scrollPos, pos.z) {}

        inline Orientation getOrientation() const override { return Orientation::Horizontal; }

        virtual void setWidth(float width) {
            boundingRect.size.x = vmax(width, default_scroll_bar_slice_width * 2);
        }


        void render(float partialTicks) override;

    protected:
        void updateScrollPos() override {
            const float scrollPosRange = boundingRect.size.x - thumbSize.x;
            const float newScrollPos = clampScrollPos((gui->getMousePos().x - (thumbSize.x / 2.0f) - boundingRect.offset.x) / scrollPosRange);
            const bool moved = newScrollPos != scrollPos;
            scrollPos = newScrollPos;
            if (moved) onDrag();
        }

    };

}