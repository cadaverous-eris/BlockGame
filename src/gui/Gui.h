#pragma once

#include <vector>
#include <string>
#include <functional>

#include <glm/vec2.hpp>

#include "input/key_input.h"
#include "util/Color.h"
#include "render/texture/Texture.h"

namespace eng {

    class Game;
    class Renderer;
    class UIRenderer;
    class FontRenderer;

    class GuiComponent;

    class Gui {
    protected:
        Game* game;
        UIRenderer* uiRenderer;
        FontRenderer* fontRenderer;

        Texture guiTexture = Texture::loadTexture("gui/gui.png").scaleNearest();

    private:
        std::string guiId;

    protected:
        int width = 0, height = 0;
        glm::vec2 mousePos { 0, 0 };

        std::vector<GuiComponent*> components {};

        GuiComponent* focusedComponent;

        std::optional<input::KeyInput> keyPressInput;
        std::optional<char32_t> charInput;

	public:
		Gui(const std::string& guiId);
        Gui(std::string&& guiId);

    private:
        void construct();
    public:

		Gui(const Gui&) = delete;
		Gui& operator =(const Gui&) = delete;

		Gui(Gui&&) noexcept = default;
		Gui& operator =(Gui&&) noexcept = default;

		virtual ~Gui() = default;


        inline const std::string& getIdentifier() const noexcept { return guiId; }


        // if returns true, then the game will pause while this GUI is open
        virtual bool shouldPauseGame() const = 0;
        // if returns true, then the game will continue to render while this GUI is open
        virtual bool isTransparent() const = 0;


		virtual void render(float partialTicks) = 0;

		virtual void update();

        virtual bool handleInput();


        virtual void handleResize();

        // return true to prevent default input handling
		virtual bool onMouseDown(const input::KeyInput& keyInput);
        // return true to prevent default input handling
		virtual bool onMouseUp(const input::KeyInput& keyInput);
        // return true to prevent default input handling
		virtual bool onKeyDown(const input::KeyInput& keyInput, bool isRepeat);
        // return true to prevent default input handling
		virtual bool onKeyUp(const input::KeyInput& keyInput);
        // return true to prevent default input handling
		virtual bool onTextInput(char32_t codePoint);


        int getWidth() const noexcept { return width; }
        int getHeight() const noexcept { return height; }
        glm::ivec2 getGuiSize() const noexcept { return { width, height }; }
        glm::vec2 getGuiSizeF() const noexcept { return { width, height }; }

        const glm::vec2& getMousePos() const noexcept { return mousePos; }

        GuiComponent* getFocusedComponent() const noexcept { return focusedComponent; }
        void focusComponent(GuiComponent* component);


        const Texture& getGuiTextures() const noexcept { return guiTexture; }

        UIRenderer& getUIRenderer() const noexcept { return *uiRenderer; }
        FontRenderer& getFontRenderer() const noexcept { return *fontRenderer; }


        void drawImage(const glm::vec3& pos, const glm::vec2& size, const glm::vec2& minUV, const glm::vec2& maxUV, const glm::vec2& textureSize, const Color& color = color::white);
        inline void drawImage(const glm::vec2& pos, const glm::vec2& size, const glm::vec2& minUV, const glm::vec2& maxUV, const glm::vec2& textureSize, const Color& color = color::white) {
            drawImage(glm::vec3(pos, 0), size, minUV, maxUV, textureSize, color);
        }
        void drawImage(const glm::vec3& pos, const glm::vec2& size, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white);
        inline void drawImage(const glm::vec2& pos, const glm::vec2& size, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white) {
            drawImage(glm::vec3(pos, 0), size, minUV, textureSize, scale, color);
        }

        void drawHorizontal3Slice(const glm::vec3& pos, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white);
        inline void drawHorizontal3Slice(const glm::vec2& pos, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white) {
            drawHorizontal3Slice(glm::vec3(pos, 0), sliceSize, innerSize, minUV, textureSize, scale, color);
        }
        void drawVertical3Slice(const glm::vec3& pos, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white);
        inline void drawVertical3Slice(const glm::vec2& pos, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white) {
            drawVertical3Slice(glm::vec3(pos, 0), sliceSize, innerSize, minUV, textureSize, scale, color);
        }

        void draw9Slice(const glm::vec3& pos, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white);
        inline void draw9Slice(const glm::vec2& pos, int sliceSize, const glm::vec2& innerSize, const glm::vec2& minUV, const glm::vec2& textureSize, float scale = 1, const Color& color = color::white) {
            draw9Slice(glm::vec3(pos, 0), sliceSize, innerSize, minUV, textureSize, scale, color);
        }

        static const int panel_slice_size;
        void drawPanel(const glm::vec3& pos, const glm::vec2& size);
        inline void drawPanel(const glm::vec2& pos, const glm::vec2& size) {
            drawPanel(glm::vec3(pos, 0), size);
        }

        // TODO: add tooltip rendering function(s)



    protected:

        void updateMousePos();

        void renderComponents(float partialTicks) const;


	};

}