#pragma once

#include <string>
#include <utility>

#include "GuiComponent.h"
#include "gui/Gui.h"
#include "util/math/math.h"

namespace eng {

    class GuiAbstractSlider : public GuiComponent {
    protected:
        static const int default_slider_height;
        static const int default_slider_slice_width;

        static const int default_thumb_height;
        static const int default_thumb_width;

        static const float min_float_value;
        static const float max_float_value;

        static float clampFloatValue(const float& floatVal);

        glm::vec2 thumbSize;
        float floatVal;
        bool isDragging = false;

    public:
        GuiAbstractSlider(Gui& gui, const RectF& boundingRect, const glm::vec2& thumbSize, float floatVal, float zIndex = default_z_index) noexcept :
                GuiComponent(gui, boundingRect, zIndex), thumbSize(thumbSize), floatVal(clampFloatValue(floatVal)) {}
        GuiAbstractSlider(Gui& gui, const glm::vec2& pos, float width, float floatVal) noexcept :
                GuiComponent(gui, { pos, { vmax(width, default_slider_slice_width * 2), default_slider_height } }),
                thumbSize(default_thumb_width, default_thumb_height),
                floatVal(clampFloatValue(floatVal)) {}
        GuiAbstractSlider(Gui& gui, const glm::vec3& pos, float width, float floatVal) noexcept :
                GuiComponent(gui, { { pos.x, pos.y }, { vmax(width, default_slider_slice_width * 2), default_slider_height } }, pos.z),
                thumbSize(default_thumb_width, default_thumb_height),
                floatVal(clampFloatValue(floatVal)) {}

        virtual float getWidth() const { return boundingRect.size.x; }
        virtual void setWidth(float width) {
            boundingRect.size.x = vmax(width, default_slider_slice_width * 2);
        }

        void render(float partialTicks) override;

        inline bool handleInput() override {
            if (isDragging)
                updateThumbPos();

            return false;
        }

        inline bool onMouseDown(const input::KeyInput& keyInput) override {
            if (keyInput.key == input::MouseButtons::MOUSE_LEFT) {
                isDragging = true;
                updateThumbPos();
            }
            return true;
        }
        inline bool onMouseUp(const input::KeyInput& keyInput) override {
            if (isDragging && (keyInput.key == input::MouseButtons::MOUSE_LEFT)) {
                updateThumbPos();
                isDragging = false;
                onValueChanged();
            }
            return true;
        }
        inline void onBlur() override {
            if (isDragging) {
                updateThumbPos();
                isDragging = false;
                onValueChanged();
            }
        }

    protected:
        // called after dragging stops
        virtual void onValueChanged() = 0;
        // called whenever the the thumb is moved
        virtual void onDrag() = 0;

        virtual std::string_view getDisplayString() const = 0;

        void updateThumbPos() {
            const float thumbPosRange = boundingRect.size.x - thumbSize.x;
            const float newVal = clampFloatValue((gui->getMousePos().x - (thumbSize.x / 2.0f) - boundingRect.offset.x) / thumbPosRange);
            const bool moved = newVal != floatVal;
            floatVal = newVal;
            if (moved) onDrag();
        }

    };


}