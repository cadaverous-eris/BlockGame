#include "GuiTextInput.h"

#include <algorithm>

#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"
#include "Game.h"

namespace eng {

    const int GuiTextInput::default_text_input_height = 20;
    const int GuiTextInput::default_text_input_slice_width = 16;
    const int GuiTextInput::default_text_input_slice_padding = 6;


    GuiTextInput::GuiTextInput(Gui& gui, const RectF& boundingRect, const std::u32string& placeholder, size_t maxLength, size_t historySize, const float zIndex) :
            GuiComponent(gui, boundingRect, zIndex),
            history(historySize, {}),
            placeholderText(placeholder),
            maxLength(maxLength),
            fontSize(gui.getFontRenderer().getFontSizeForLineHeight(14.0f)) {}


    void GuiTextInput::update() {
        // increase cursor blink timer
        cursorBlinkTimer = (cursorBlinkTimer + 1) % (Game::TICKRATE);
    }

    void GuiTextInput::render(float) {
        constexpr Color highlightColor = 0x00CCCCBF_c;
        constexpr Color cursorColor = 0xFFFFFF_c;
        const Color textColor = (disabled ? 0x575757_c : 0xCCCCCC_c);
        const Color placeholderColor = (disabled ? 0x474747_c : 0x707070_c);
        const bool focused = isFocused();
        const bool cursorVisible = focused && (cursorBlinkTimer < (Game::TICKRATE / 2));

        const auto& state = history.current();

        const glm::vec3 origin(boundingRect.offset, zIndex);
        const float innerWidth = boundingRect.size.x - (2.0f * default_text_input_slice_width);
        const glm::vec2 innerSize { innerWidth, default_text_input_height };
        const glm::vec2 minUV { 0, (focused ? 112 : 92) };
        gui->drawHorizontal3Slice(origin, default_text_input_slice_width, innerSize, minUV, { 256, 256 });
        // TODO: actually render the image?
        //Texture::bind(gui->getGuiTextures());
        //gui->getUIRenderer().flushTextured();

        std::u32string_view displayText = state.value;
        const bool usePlaceholder = displayText.empty() && !placeholderText.empty();
        if (usePlaceholder) displayText = placeholderText;
        const float maxTextWidth = boundingRect.size.x - static_cast<float>(default_text_input_slice_padding * 2);
        const glm::vec3 textOrigin = glm::floor(glm::vec3(origin.x + static_cast<float>(default_text_input_slice_padding), boundingRect.offset.y + 3.0f, zIndex - 1));
        if (focused) {
            const auto drawResult = gui->getFontRenderer().drawEditableText(displayText, { state.selectionStart, state.cursorPos }, cursorVisible, textOrigin, fontSize, usePlaceholder ? placeholderColor : textColor, cursorColor, highlightColor, maxTextWidth, usePlaceholder ? 0 : -textScroll);
            if (usePlaceholder) {
                cachedTextWidth = 0;
                cachedCursorOffset = 0;
            } else {
                cachedTextWidth = drawResult.textWidth;
                cachedCursorOffset = drawResult.cursorOffset;
            }
        } else {
            gui->getFontRenderer().drawText(displayText, textOrigin, fontSize, usePlaceholder ? placeholderColor : textColor, {}, false, maxTextWidth, 0);
        }
    }

    bool GuiTextInput::handleInput() {
        const auto& val = history.current().value;
        if (val.empty()) {
            textScroll = 0;
        } else {
            const float maxTextWidth = boundingRect.size.x - static_cast<float>(default_text_input_slice_padding * 2);
            const float cursorOffsetX = cachedCursorOffset - textScroll;
            if ((textScroll > 0) && (cachedTextWidth <= maxTextWidth)) {
                textScroll = 0;
            } else if (cursorOffsetX < 0) {
                textScroll = vmax(cachedCursorOffset, 0);
            } else if (cursorOffsetX > maxTextWidth) {
                textScroll = vmax(cachedCursorOffset - maxTextWidth, 0);
            }
        }
        return false;
    }


    void GuiTextInput::onFocus() {
        InputState& current = history.current();
        std::u32string newVal = current.value;
        processValue(newVal, true);
        if (newVal != current.value) {
            history.push({ newVal, newVal.size(), newVal.size() });
            onChange();
        } else {
            current.cursorPos = current.selectionStart = current.value.size();
        }
        cursorBlinkTimer = 0;

        const auto val = history.current().value;
        if (val.empty()) {
            textScroll = 0;
        } else {
            const float maxTextWidth = boundingRect.size.x - static_cast<float>(default_text_input_slice_padding * 2);
            cachedTextWidth = gui->getFontRenderer().getTextWidth(val, fontSize);
            cachedCursorOffset = cachedTextWidth;
            textScroll = vmax(cachedCursorOffset - maxTextWidth, 0);
        }
    }
    void GuiTextInput::onBlur() {
        const auto& current = history.current();
        std::u32string newVal = current.value;
        processValue(newVal, false);
        if (newVal != current.value)
            history.push({ newVal, vmin(current.cursorPos, newVal.size()), vmin(current.selectionStart, newVal.size()) });
        textScroll = 0;
        onChange();
    }

    void GuiTextInput::onChange() {
        auto& state = history.current();
        if (state.cursorPos > state.value.size()) state.cursorPos = state.value.size();
        if (state.selectionStart > state.value.size()) state.selectionStart = state.value.size();
    }

    // removes newline characters from pasted text and replaces them with spaces
    static std::u32string processClipboardContents(std::string_view clipboard) {
        auto str = unicode::utf8ToUtf32(clipboard);
        for (size_t i = 0; i < str.size(); i++) {
            const char32_t c = str[i];
            if (((c >= 0x0A) && (c <= 0x0D)) || (c == 0x85) || (c == 0x2028) || (c == 0x2029)) {
                str[i] = 0x20;
                if ((c == 0x0D) && ((i + 1) < str.size()) && (str[i + 1] == 0x0A)) {
                    str.erase(i + 1, 1);
                }
            }
        }
        return str;
    }

    bool GuiTextInput::onKeyDown(const input::KeyInput& keyInput, const bool isRepeat) {
        using namespace input;
        auto& state = history.current();
        if (!keyInput.modifiers[ModifierBits::ALT] && !keyInput.modifiers[ModifierBits::SUPER] && !keyInput.modifiers[ModifierBits::CONTROL]) {
            if (keyInput.key == Keys::KEY_LEFT) {
                if (keyInput.modifiers[input::ModifierBits::SHIFT]) {
                    if (state.cursorPos > 0) state.cursorPos--;
                } else if (state.cursorPos < state.selectionStart) {
                    state.selectionStart = state.cursorPos;
                } else if (state.cursorPos > state.selectionStart) {
                    state.cursorPos = state.selectionStart;
                } else {
                    if (state.cursorPos > 0) state.cursorPos = state.selectionStart = (state.cursorPos - 1);
                }
                cursorBlinkTimer = 0;
                return true;
            } else if (keyInput.key == Keys::KEY_RIGHT) {
                if (keyInput.modifiers[ModifierBits::SHIFT]) {
                    state.cursorPos = vmin(state.cursorPos + 1, state.value.size());
                } else if (state.cursorPos > state.selectionStart) {
                    state.selectionStart = state.cursorPos;
                } else if (state.cursorPos < state.selectionStart) {
                    state.cursorPos = state.selectionStart;
                } else {
                    state.cursorPos = state.selectionStart = vmin(state.cursorPos + 1, state.value.size());
                }
                cursorBlinkTimer = 0;
                return true;
            } else if (keyInput.key == Keys::KEY_BACKSPACE) {
                if (state.cursorPos == state.selectionStart) {
                    if (state.cursorPos > 0) {
                        InputState newState = state;
                        newState.cursorPos = newState.selectionStart = (newState.cursorPos - 1);
                        newState.value.erase(newState.cursorPos, 1);
                        processValue(newState.value, true);
                        history.push(newState);
                        onChange();
                    }
                } else {
                    InputState newState = state;
                    const auto selectionStart = vmin(newState.cursorPos, newState.selectionStart);
                    const auto selectionEnd = vmax(newState.cursorPos, newState.selectionStart);
                    newState.cursorPos = newState.selectionStart = selectionStart;
                    newState.value.erase(selectionStart, selectionEnd - selectionStart);
                    processValue(newState.value, true);
                    history.push(newState);
                    onChange();
                }
                cursorBlinkTimer = 0;
                return true;
            } else if ((keyInput.key == Keys::KEY_DELETE)) {
                if (state.cursorPos == state.selectionStart) {
                    if (/*!keyInput.modifiers[ModifierBits::SHIFT] && */(state.cursorPos < state.value.size())) {
                        InputState newState = state;
                        newState.value.erase(newState.cursorPos, 1);
                        processValue(newState.value, true);
                        history.push(newState);
                        onChange();
                        cursorBlinkTimer = 0;
                    }
                } else {
                    InputState newState = state;
                    const auto selectionStart = vmin(newState.cursorPos, newState.selectionStart);
                    const auto selectionEnd = vmax(newState.cursorPos, newState.selectionStart);
                    newState.cursorPos = newState.selectionStart = selectionStart;
                    newState.value.erase(selectionStart, selectionEnd - selectionStart);
                    processValue(newState.value, true);
                    history.push(newState);
                    onChange();
                    cursorBlinkTimer = 0;
                }
                return true;
            }
        }
        if ((keyInput.modifiers & ~(ModifierBits::CAPS_LOCK | ModifierBits::NUM_LOCK)) == ModifierBits::KEY_COMMAND_MODIFIER) {
            if (keyInput.key == Keys::KEY_A) {
                if (!state.value.empty()) {
                    state.selectionStart = 0;
                    state.cursorPos = state.value.size();
                    cursorBlinkTimer = 0;
                    return true;
                }
            } else if (keyInput.key == Keys::KEY_X) {
                if (state.cursorPos != state.selectionStart) {
                    InputState newState = state;
                    const auto selectionStart = vmin(newState.cursorPos, newState.selectionStart);
                    const auto selectionEnd = vmax(newState.cursorPos, newState.selectionStart);
                    const auto subStr = std::u32string_view(newState.value).substr(selectionStart, selectionEnd - selectionStart);
                    Game::instance().inputManager.setClipboardString(unicode::utf32ToUtf8<std::string>(subStr));
                    newState.cursorPos = newState.selectionStart = selectionStart;
                    newState.value.erase(selectionStart, selectionEnd - selectionStart);
                    processValue(newState.value, true);
                    history.push(newState);
                    onChange();
                    cursorBlinkTimer = 0;
                    return true;
                }
            } else if (keyInput.key == Keys::KEY_C) {
                if (state.cursorPos != state.selectionStart) {
                    const auto selectionStart = vmin(state.cursorPos, state.selectionStart);
                    const auto selectionEnd = vmax(state.cursorPos, state.selectionStart);
                    const auto subStr = std::u32string_view(state.value).substr(selectionStart, selectionEnd - selectionStart);
                    Game::instance().inputManager.setClipboardString(unicode::utf32ToUtf8<std::string>(subStr));
                    return true;
                }
            } else if (keyInput.key == Keys::KEY_V) {
                const auto clipboardStr = processClipboardContents(Game::instance().inputManager.getClipboardString());
                const size_t clipboardStrSize = clipboardStr.size();
                if (state.cursorPos == state.selectionStart) {
                    if (!clipboardStr.empty()) {
                        InputState newState = state;
                        newState.value.insert(newState.cursorPos, clipboardStr);
                        newState.cursorPos = newState.selectionStart = (newState.cursorPos + clipboardStrSize);
                        processValue(newState.value, true);
                        history.push(newState);
                        onChange();
                        cursorBlinkTimer = 0;
                    }
                } else {
                    InputState newState = state;
                    const auto selectionStart = vmin(newState.cursorPos, newState.selectionStart);
                    const auto selectionEnd = vmax(newState.cursorPos, newState.selectionStart);
                    newState.value.replace(selectionStart, selectionEnd - selectionStart, clipboardStr);
                    newState.cursorPos = newState.selectionStart = (selectionStart + clipboardStrSize);
                    processValue(newState.value, true);
                    history.push(newState);
                    onChange();
                    cursorBlinkTimer = 0;
                }
                return true;
            } else if (keyInput.key == Keys::KEY_Z) {
                if (history.undo()) {
                    onChange();
                    cursorBlinkTimer = 0;
                }
                return true;
            } else if (keyInput.key == Keys::KEY_Y) {
                if (history.redo()) {
                    onChange();
                    cursorBlinkTimer = 0;
                }
                return true;
            }
        }
        return false;
    }
    bool GuiTextInput::onTextInput(char32_t codePoint) {
        auto& state = history.current();
        InputState newState = state;

        if (newState.cursorPos == newState.selectionStart) {
            newState.value.insert(newState.cursorPos, 1, codePoint);
            newState.cursorPos = newState.selectionStart = (newState.cursorPos + 1);
        } else {
            const auto selectionStart = vmin(newState.cursorPos, newState.selectionStart);
            const auto selectionEnd = vmax(newState.cursorPos, newState.selectionStart);
            newState.cursorPos = newState.selectionStart = (selectionStart + 1);
            const auto eraseCount = selectionEnd - (selectionStart + 1);
            if (eraseCount)
                newState.value.erase(selectionStart + 1, eraseCount);
            newState.value[selectionStart] = codePoint;
        }
        processValue(newState.value, true);
        history.push(newState);
        onChange();
        cursorBlinkTimer = 0;

        return true;
    }


}