#pragma once

#include <utility>

#include "util/math/Rect.h"
#include "input/key_input.h"

namespace eng {

    class Gui;

    class GuiComponent {
    public:
        static constexpr float default_z_index = 10;
    protected:
        Gui* gui;
        RectF boundingRect;
        float zIndex = default_z_index;
        // bool disabled = false; // TODO: allow GuiComponents to be disabled

    public:

        GuiComponent(Gui& gui, const RectF& boundingRect, float zIndex = default_z_index) noexcept;

        GuiComponent(const GuiComponent&) noexcept = default;
		GuiComponent(GuiComponent&&) noexcept = default;
		GuiComponent& operator =(const GuiComponent&) noexcept = default;
		GuiComponent& operator =(GuiComponent&&) noexcept = default;

		virtual ~GuiComponent() = default;

        inline float getZIndex() const noexcept { return zIndex; }
        inline GuiComponent& setZIndex(float zIndex) & noexcept { this->zIndex = zIndex; return *this; }
        inline GuiComponent&& setZIndex(float zIndex) && noexcept { this->zIndex = zIndex; return std::move(*this); }

        inline RectF getBoundingRect() const noexcept { return boundingRect; }

        virtual RectF::vec_type getPos() const { return boundingRect.offset; }
        virtual void setPos(const RectF::vec_type& pos) { boundingRect.offset = pos; }
        virtual RectF::vec_type getSize() const { return boundingRect.size; }


        virtual void render(float partialTicks) = 0;

        virtual void update() {}

        // called only when focused
        virtual bool handleInput() { return false; }


        bool isFocused() const noexcept;
        bool isHovered() const noexcept;

        virtual void onFocus() {}
        virtual void onBlur() {}

		virtual bool onMouseDown(const input::KeyInput& keyInput) { return false; }
		virtual bool onMouseUp(const input::KeyInput& keyInput) { return false; }
		virtual bool onKeyDown(const input::KeyInput& keyInput, bool isRepeat) { return false; }
		virtual bool onKeyUp(const input::KeyInput& keyInput) { return false; }
		virtual bool onTextInput(char32_t codePoint) { return false; }



    };

}