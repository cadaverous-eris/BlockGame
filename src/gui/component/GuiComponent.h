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
        bool disabled = false;
        bool visible = true;
        // a pointer to a bounding rect that the component must be inside of in order for it to be able to be focused, hovered, rendered, etc.
        const RectF* containerRect = nullptr;

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

        inline const RectF* getContainerRect() const noexcept { return containerRect; }
        inline GuiComponent& setContainerRect(const RectF* const rect) & noexcept { this->containerRect = rect; return *this; }
        inline GuiComponent&& setContainerRect(const RectF* const rect) && noexcept { this->containerRect = rect; return std::move(*this); }

        inline bool getDisabled() const noexcept { return disabled; }
        inline GuiComponent& setDisabled(const bool disabled) & noexcept { this->disabled = disabled; return *this; }
        inline GuiComponent&& setDisabled(const bool disabled) && noexcept { this->disabled = disabled; return std::move(*this); }
        inline bool getVisible() const noexcept { return visible; }
        inline GuiComponent& setVisible(const bool visible) & noexcept { this->visible = visible; return *this; }
        inline GuiComponent&& setVisible(const bool visible) && noexcept { this->visible = visible; return std::move(*this); }

        virtual RectF::vec_type getPos() const { return boundingRect.offset; }
        virtual void setPos(const RectF::vec_type& pos) { boundingRect.offset = pos; }
        virtual RectF::vec_type getSize() const { return boundingRect.size; }


        virtual void render(float partialTicks) = 0;

        virtual void update() {}

        // called only when focused
        virtual bool handleInput() { return false; }


        virtual bool shouldRender() const { return visible && (!containerRect || containerRect->intersects(boundingRect)); }


        bool isFocused() const noexcept;
        bool isMouseOver() const noexcept;

        virtual void onFocus() {}
        virtual void onBlur() {}

		virtual bool onMouseDown(const input::KeyInput& keyInput) { return false; }
		virtual bool onMouseUp(const input::KeyInput& keyInput) { return false; }
		virtual bool onKeyDown(const input::KeyInput& keyInput, bool isRepeat) { return false; }
		virtual bool onKeyUp(const input::KeyInput& keyInput) { return false; }
		virtual bool onTextInput(char32_t codePoint) { return false; }



    };

}