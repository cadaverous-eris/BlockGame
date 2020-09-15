#include "GuiMenu.h"


#include "Game.h"
#include "render/Renderer.h"
#include "render/ui/UIRenderer.h"

namespace eng {




    void GuiMenu::drawBackground() const {
        game->renderer.setClearColor(color::black);
        game->renderer.clear(Renderer::ClearBit::COLOR | Renderer::ClearBit::DEPTH | Renderer::ClearBit::STENCIL);
        const glm::vec2 guiSize { width, height };
        const glm::vec2 uvMax = guiSize / (glm::vec2(bgTexture.getWidth(), bgTexture.getHeight()) * 2.0f);
		uiRenderer->drawTexturedQuad({ 0, 0, 99 }, guiSize, { 0, 0 }, uvMax);
        Texture::bind(bgTexture);
		uiRenderer->flushTextured();
    }

}