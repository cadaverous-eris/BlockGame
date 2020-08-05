#pragma once

#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "util/resources/Image.h"

namespace eng {

	class Game;

	namespace input {
		class InputManager; // forward declare InputManager so it can be made a friend of Window
	}

	class Window {
		friend class input::InputManager;
	private:
		GLFWwindow* handle;

		static inline bool vSync = false;
	public:
		Game* const game;

		Window(Game* const game, int width, int height, const char* title = "");
		~Window();

		Window(Window&&);

		Window(const Window&) = delete;
		Window& operator =(const Window&) = delete;
		Window& operator =(Window&&) = delete;

		// returns the width of the window's frame buffer
		int getWidth() const;
		// returns the height of the window's frame buffer
		int getHeight() const;
		glm::ivec2 getSize() const; // returns the framebuffer size

		glm::vec2 getContentScale() const;

		bool isVSync() const;
		void setVSync(bool vSync = true) const;

		void setTitle(const char* title) const;
		void setTitle(const std::string& title) const;

		void setIcon(std::vector<Image4> iconImages) const;

		void minimize() const;
		void maximize() const;
		// restores the window if it's been minimized or maximized
		void restore() const;
		bool isMinimized() const;
		bool isMaximized() const;

		bool hasFocus() const;

		void requestAttention() const;

		void swapBuffers() const;

		GLFWwindow* getHandle() const { return handle; } // TODO: remove

	};

}
