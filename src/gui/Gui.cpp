#include "Gui.h"

#include <utility>
#include <iostream>

#include "Game.h"
#include "render/Renderer.h"
#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"
#include "component/GuiComponent.h"

namespace eng {

    Gui::Gui(const std::string& guiId) :
            game(&Game::instance()),
            uiRenderer(game->renderer.getUIRenderer()),
            fontRenderer(game->renderer.getFontRenderer()),
            guiId(guiId),
            focusedComponent(nullptr) {
        construct();
    }
    Gui::Gui(std::string&& guiId) :
            game(&Game::instance()),
            uiRenderer(game->renderer.getUIRenderer()),
            fontRenderer(game->renderer.getFontRenderer()),
            guiId(std::move(guiId)),
            focusedComponent(nullptr) {
        construct();
    }

    void Gui::construct() {
        handleResize();
        updateMousePos();
    }

    void Gui::update() {
        for (auto component : components)
            component->update();
    }

    bool Gui::handleInput() {
        updateMousePos();
        if (focusedComponent) return focusedComponent->handleInput();
        return false;
    }

    void Gui::renderComponents(const float partialTicks) const {
        for (auto component : components)
            component->render(partialTicks);
    }

    void Gui::handleResize() {
        const auto& uiSize = uiRenderer->getUISize();
        width = static_cast<int>(uiSize.x);
        height = static_cast<int>(uiSize.y);
    }

    void Gui::updateMousePos() {
        mousePos = static_cast<glm::vec2>(game->inputManager.getCursorPos()) / uiRenderer->getUIScale();
    }

    void Gui::focusComponent(GuiComponent* const component) {
        if (focusedComponent == component) return;
        GuiComponent* const prevFocused = focusedComponent;
        focusedComponent = component;
        if (prevFocused)
            prevFocused->onBlur();
        if (focusedComponent)
            focusedComponent->onFocus();
    }

    bool Gui::onMouseDown(const input::KeyInput& keyInput) {
        bool isOverComponent = false;
        for (auto component : components) {
            if (component->getBoundingRect().contains(mousePos)) {
                if (component != focusedComponent) focusComponent(component);
                isOverComponent = true;
                break;
            }
        }
        if (!isOverComponent) focusComponent(nullptr);
        if (focusedComponent) focusedComponent->onMouseDown(keyInput);
        return true;
    }
    bool Gui::onMouseUp(const input::KeyInput& keyInput) {
        if (focusedComponent) focusedComponent->onMouseUp(keyInput);
        return true;
    }
    bool Gui::onKeyDown(const input::KeyInput& keyInput, const bool isRepeat) {
        //std::cout << "keyDown: " << keyInput << '\n';

        if (focusedComponent) focusedComponent->onKeyDown(keyInput, isRepeat);
        return true;
    }
    bool Gui::onKeyUp(const input::KeyInput& keyInput) {
        if (focusedComponent) focusedComponent->onKeyUp(keyInput);
        return true;
    }
    bool Gui::onTextInput(char32_t codePoint) {
        //std::cout << "charInput: " << static_cast<uint32_t>(codePoint) << '\n';

        if (focusedComponent) focusedComponent->onTextInput(codePoint);
        return true;
    }


    void Gui::drawImage(const glm::vec3& pos, const glm::vec2& size, const glm::vec2& minUV, const glm::vec2& maxUV, const glm::vec2& textureSize, const Color& color) {
        uiRenderer->drawTexturedQuad(glm::floor(pos), size, minUV / textureSize, maxUV / textureSize, color);
    }
    void Gui::drawImage(const glm::vec3& pos, const glm::vec2& size, const glm::vec2& minUV, const glm::vec2& textureSize, float scale, const Color& color) {
        uiRenderer->drawTexturedQuad(glm::floor(pos), size, minUV / textureSize, (minUV + (size / scale)) / textureSize, color);
    }

    void Gui::drawHorizontal3Slice(const glm::vec3& posIn, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale, const Color& color) {
        if ((innerSize.x < 0) || (innerSize.y < 0)) return;
        glm::vec3 pos = glm::floor(posIn);
        const float sliceSizeF = static_cast<float>(sliceSize);
        const glm::vec2 edgeSize { sliceSizeF * scale, innerSize.y };
        const glm::vec2 uvOffset = glm::vec2(sliceSizeF, innerSize.y / scale) / textureSize;
        const glm::vec2 uvMin = minUV / textureSize;
        const glm::vec2 uvMax = glm::vec2(minUV.x + static_cast<float>(sliceSize * 3), minUV.y + (innerSize.y / scale)) / textureSize;
        // draw center
        if (innerSize.x > 0)
            uiRenderer->drawTexturedQuad({ pos.x + edgeSize.x, pos.y, pos.z }, innerSize, { uvMin.x + uvOffset.x, uvMin.y }, { uvMax.x - uvOffset.x, uvMax.y }, color);
        // draw edges
        uiRenderer->drawTexturedQuad(pos, edgeSize, uvMin, { uvMin.x + uvOffset.x, uvMax.y }, color); // left edge
        uiRenderer->drawTexturedQuad({ pos.x + innerSize.x + edgeSize.x, pos.y, pos.z }, edgeSize, { uvMax.x - uvOffset.x, uvMin.y }, uvMax, color); // right edge
    }
    void Gui::drawVertical3Slice(const glm::vec3& posIn, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale, const Color& color) {
        if ((innerSize.x < 0) || (innerSize.y < 0)) return;
        glm::vec3 pos = glm::floor(posIn);
        const float sliceSizeF = static_cast<float>(sliceSize);
        const glm::vec2 edgeSize { innerSize.x, sliceSizeF * scale };
        const glm::vec2 uvOffset = glm::vec2(innerSize.x / scale, sliceSizeF) / textureSize;
        const glm::vec2 uvMin = minUV / textureSize;
        const glm::vec2 uvMax { uvMin.x + uvOffset.x, uvMin.y + (uvOffset.y * 3.0f) };
        // draw center
        if (innerSize.x > 0)
            uiRenderer->drawTexturedQuad({ pos.x, pos.y + edgeSize.y, pos.z }, innerSize, { uvMin.x, uvMin.y + uvOffset.y }, { uvMax.x, uvMax.y - uvOffset.y }, color);
        // draw edges
        uiRenderer->drawTexturedQuad(pos, edgeSize, uvMin, { uvMin.x + uvOffset.x, uvMax.y }, color); // top edge
        uiRenderer->drawTexturedQuad({ pos.x, pos.y + innerSize.y + edgeSize.y, pos.z }, edgeSize, { uvMin.x, uvMax.y - uvOffset.y }, uvMax, color); // bottom edge
    }
    void Gui::draw9Slice(const glm::vec3& posIn, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale, const Color& color) {
        if ((innerSize.x < 0) || (innerSize.y < 0)) return;
        glm::vec3 pos = glm::floor(posIn);
        const float sliceSizeF = static_cast<float>(sliceSize);
        const glm::vec2 cornerSize { sliceSizeF * scale, sliceSizeF * scale };
        const glm::vec2 hEdgeSize { innerSize.x, sliceSizeF * scale };
        const glm::vec2 vEdgeSize { sliceSizeF * scale, innerSize.y };
        const glm::vec2 uvOffset = glm::vec2(sliceSize, sliceSize) / textureSize;
        const glm::vec2 uvMin = minUV / textureSize;
        const glm::vec2 uvMax = uvMin + (uvOffset * 3.0f);
        // draw center
        if ((innerSize.x > 0) && (innerSize.y > 0))
            uiRenderer->drawTexturedQuad({ pos.x + cornerSize.x, pos.y + cornerSize.y, pos.z }, innerSize, uvMin + uvOffset, uvMax - uvOffset, color);
        // draw edges
        if (innerSize.x > 0) {
            uiRenderer->drawTexturedQuad({ pos.x + cornerSize.x, pos.y, pos.z }, hEdgeSize, { uvMin.x + uvOffset.x, uvMin.y }, { uvMax.x - uvOffset.x, uvMin.y + uvOffset.y }, color); // top edge
            uiRenderer->drawTexturedQuad({ pos.x + cornerSize.x, pos.y + cornerSize.y + innerSize.y, pos.z }, hEdgeSize, { uvMin.x + uvOffset.x, uvMax.y - uvOffset.y }, { uvMax.x - uvOffset.x, uvMax.y }, color); // bottom edge
        }
        if (innerSize.y > 0) {
            uiRenderer->drawTexturedQuad({ pos.x, pos.y + cornerSize.y, pos.z }, vEdgeSize, { uvMin.x, uvMin.y + uvOffset.x }, { uvMin.x + uvOffset.x, uvMax.y - uvOffset.y }, color); // left edge
            uiRenderer->drawTexturedQuad({ pos.x + cornerSize.x + innerSize.x, pos.y + cornerSize.y, pos.z }, vEdgeSize, { uvMax.x - uvOffset.x, uvMin.y + uvOffset.x }, { uvMax.x, uvMax.y - uvOffset.y }, color); // right edge
        }
        // draw corners
        uiRenderer->drawTexturedQuad(pos, cornerSize, uvMin, uvMin + uvOffset, color); // top left
        uiRenderer->drawTexturedQuad({ pos.x, pos.y + innerSize.y + cornerSize.y, pos.z }, cornerSize, { uvMin.x, uvMax.y - uvOffset.y }, { uvMin.x + uvOffset.x, uvMax.y }, color); // bottom left
        uiRenderer->drawTexturedQuad({ pos.x + innerSize.x + cornerSize.x, pos.y, pos.z }, cornerSize, { uvMax.x - uvOffset.x, uvMin.y }, { uvMax.x, uvMin.y + uvOffset.y }, color); // top right
        uiRenderer->drawTexturedQuad({ pos.x + innerSize.x + cornerSize.x, pos.y + innerSize.y + cornerSize.y, pos.z }, cornerSize, uvMax - uvOffset, uvMax, color); // bottom right
    }

    const int Gui::panel_slice_size = 8;
    void Gui::drawPanel(const glm::vec3& pos, const glm::vec2& size) {
        const glm::vec2 innerSize = glm::max(size - glm::vec2(2 * panel_slice_size), glm::vec2(0, 0));
        draw9Slice(pos, panel_slice_size, innerSize, { 0, 0 }, { 256, 256 });
    }

}