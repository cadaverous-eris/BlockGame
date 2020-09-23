#pragma once

#include <string>
#include <utility>
#include <type_traits>

#include "GuiComponent.h"
#include "gui/Gui.h"
#include "util/math/math.h"
#include "util/text/unicode_utils.h"
#include "util/HistoryBuffer.h"
#include "input/key_input.h"

namespace eng {

    class GuiTextInput : public GuiComponent {
    public:
        struct InputState {
            std::u32string value {};
			size_t cursorPos = 0, selectionStart = 0;
        };

        using InputHistory = HistoryBuffer<InputState>;

        static constexpr size_t npos = size_t(-1);
    protected:
        static const int default_text_input_height;
        static const int default_text_input_slice_width;
        static const int default_text_input_slice_padding;

        static constexpr size_t default_history_size = 20;


        InputHistory history;
        std::u32string placeholderText {};
        size_t maxLength = npos;
        int cursorBlinkTimer = 0;
        float textScroll = 0;
        float fontSize;
        float cachedTextWidth = 0;
        float cachedCursorOffset = 0;

    public:
        GuiTextInput(Gui& gui, const RectF& boundingRect, const std::u32string& placeholder, size_t maxLength, size_t historySize, const float zIndex = default_z_index);

        GuiTextInput(Gui& gui, const glm::vec3& pos, size_t historySize, float width, const std::u32string& placeholder, size_t maxLength = npos) :
                GuiTextInput(gui, { { pos.x, pos.y }, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, placeholder, maxLength, historySize, pos.z) {}
        GuiTextInput(Gui& gui, const glm::vec3& pos, size_t historySize, float width, size_t maxLength = npos) :
                GuiTextInput(gui, { { pos.x, pos.y }, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, U"", maxLength, historySize, pos.z) {}
        GuiTextInput(Gui& gui, const glm::vec2& pos, size_t historySize, float width, const std::u32string& placeholder, size_t maxLength = npos) :
                GuiTextInput(gui, { pos, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, placeholder, maxLength, historySize) {}
        GuiTextInput(Gui& gui, const glm::vec2& pos, size_t historySize, float width, size_t maxLength = npos) :
                GuiTextInput(gui, { pos, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, U"", maxLength, historySize) {}
        GuiTextInput(Gui& gui, const glm::vec3& pos, float width, const std::u32string& placeholder, size_t maxLength = npos) :
                GuiTextInput(gui, { { pos.x, pos.y }, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, placeholder, maxLength, default_history_size, pos.z) {}
        GuiTextInput(Gui& gui, const glm::vec3& pos, float width, size_t maxLength = npos) :
                GuiTextInput(gui, { { pos.x, pos.y }, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, U"", maxLength, default_history_size, pos.z) {}
        GuiTextInput(Gui& gui, const glm::vec2& pos, float width, const std::u32string& placeholder, size_t maxLength = npos) :
                GuiTextInput(gui, { pos, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, placeholder, maxLength, default_history_size) {}
        GuiTextInput(Gui& gui, const glm::vec2& pos, float width, size_t maxLength = npos) :
                GuiTextInput(gui, { pos, { vmax(width, default_text_input_slice_width * 2), default_text_input_height } }, U"", maxLength, default_history_size) {}


        inline const std::u32string& getU32Value() const noexcept { return history.current().value; }
        inline std::u8string getU8Value() const { return unicode::utf32ToUtf8<std::u8string>(history.current().value); }
        inline std::string getValue() const { return unicode::utf32ToUtf8<std::string>(history.current().value); }

        void setValue(std::u32string val) {
            processValue(val, false);
            history.reset({ val, val.size(), val.size() });
            onChange();
        }
        void setValue(std::u8string_view val) {
            std::u32string val32 = unicode::utf8ToUtf32(val);
            processValue(val32, false);
            history.reset({ val32, val32.size(), val32.size() });
            onChange();
        }
        void setValue(std::string_view val) {
            std::u32string val32 = unicode::utf8ToUtf32(val);
            processValue(val32, false);
            history.reset({ val32, val32.size(), val32.size() });
            onChange();
        }


        virtual float getWidth() const { return boundingRect.size.x; }
        virtual void setWidth(float width) {
            boundingRect.size.x = vmax(width, default_text_input_slice_width * 2);
        };


        void update() override;

        void render(float partialTicks) override;

        bool handleInput() override;


        void onFocus() override;
        void onBlur() override;

        bool onKeyDown(const input::KeyInput& keyInput, const bool isRepeat) override;
        bool onTextInput(char32_t codePoint) override;

    protected:
        // called after text input and component focus/blur, can modify the value
        virtual void processValue(std::u32string& value, bool isFocused) {
            // TODO: trim whitespace from beginning,  if !isFocused

            if (!isFocused) {
                // TODO: trim whitespace from end
            }
            // limit length to maxLength
            if ((maxLength != npos) && (value.size() > maxLength))
                value.resize(maxLength);
        }

        virtual void onChange();


    };

}