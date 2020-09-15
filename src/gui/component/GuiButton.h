#pragma once

#include <string>
#include <utility>
#include <functional>
#include <type_traits>

#include "GuiComponent.h"
#include "gui/Gui.h"
#include "util/math/math.h"

namespace eng {

    class GuiAbstractButton : public GuiComponent {
    protected:
        static const int default_button_height;
        static const int default_button_slice_width;

    public:
        GuiAbstractButton(Gui& gui, const RectF& boundingRect, const float zIndex = default_z_index) noexcept :
                GuiComponent(gui, boundingRect, zIndex) {}

        virtual float getWidth() const { return boundingRect.size.x; }
        virtual void setWidth(float width) {
            boundingRect.size.x = vmax(width, default_button_slice_width * 2);
        };

        void render(float partialTicks) override;

        inline bool onMouseDown(const input::KeyInput& keyInput) override {
            onClick();
            return true;
        }
        inline bool onKeyDown(const input::KeyInput& keyInput, const bool isRepeat) override {
            const bool isEnterKey = keyInput.key == input::Keys::KEY_ENTER;
            if (isEnterKey) onClick();
            return isEnterKey;
        }

    protected:
        virtual void onClick() = 0;

        virtual std::string_view getDisplayString() const = 0;

    };


    class GuiButton : public GuiAbstractButton {
    protected:
        std::string text;
        std::function<void()> clickHandler;

    public:
        template<typename Handler, std::enable_if_t<std::is_invocable_v<Handler>, int> = 0>
        GuiButton(Gui& gui, const glm::vec2& pos, float width, const std::string& text, Handler&& clickHandler) :
                GuiAbstractButton(gui, { pos, { vmax(width, default_button_slice_width * 2), default_button_height } }),
                text(text),
                clickHandler(std::forward<Handler>(clickHandler)) {}
        template<typename Handler, std::enable_if_t<std::is_invocable_v<Handler>, int> = 0>
        GuiButton(Gui& gui, const glm::vec3& pos, float width, const std::string& text, Handler&& clickHandler) :
                GuiAbstractButton(gui, { { pos.x, pos.y }, { vmax(width, default_button_slice_width * 2), default_button_height } }, pos.z),
                text(text),
                clickHandler(std::forward<Handler>(clickHandler)) {}

        const std::string& getButtonText() const noexcept { return text; }
        void setButtonText(const std::string& text) { this->text = text; }

    protected:
        inline void onClick() override { clickHandler(); }

        inline std::string_view getDisplayString() const override { return text; }
    };

}