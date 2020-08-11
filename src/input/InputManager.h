#pragma once

#include <glm/vec2.hpp>

#include "render/Window.h"
#include "key_input.h"
#include "KeyBind.h"


namespace eng::input {

	enum class CursorMode {
		NORMAL   = GLFW_CURSOR_NORMAL,
		DISABLED = GLFW_CURSOR_DISABLED,
		HIDDEN   = GLFW_CURSOR_HIDDEN,
	};

	void key_callback(GLFWwindow*, int32_t, int32_t, int32_t, int32_t);
	void mouse_button_callback(GLFWwindow*, int32_t, int32_t, int32_t);
	void scroll_callback(GLFWwindow*, double, double);
	void text_input_callback(GLFWwindow*, uint32_t codePoint); // codePoint is native endian UTF-32
	//void cursor_pos_callback(GLFWwindow*, double x, double y); // unused
	//void cursor_enter_callback(GLFWwindow*, int entered); // entered == 1 when entering, 0 when leaving // unused

	class InputManager {
		friend void key_callback(GLFWwindow*, int32_t, int32_t, int32_t, int32_t);
		friend void mouse_button_callback(GLFWwindow*, int32_t, int32_t, int32_t);
		friend void scroll_callback(GLFWwindow*, double, double);
		friend void text_input_callback(GLFWwindow*, uint32_t codePoint);
	private:
		Window* window;

		CursorMode prevCursorMode;
		glm::dvec2 cursorPos, prevCursorPos;
		glm::dvec2 mouseScrollOffset;

	public: // public methods
		explicit InputManager(Window* window);

		~InputManager() = default;

		// handle input events
		// called every iteration of the game loop
		void input();

		// called every game tick
		void update();


		const glm::dvec2& getCursorPos() const noexcept { return cursorPos; }
		const glm::dvec2& getPrevCursorPos() const noexcept { return prevCursorPos; }

		const glm::dvec2& getMouseScroll() const noexcept { return mouseScrollOffset; }

		CursorMode getCursorMode() const noexcept { return static_cast<CursorMode>(glfwGetInputMode(window->handle, GLFW_CURSOR)); }
		void setCursorMode(CursorMode cursorMode) const noexcept { glfwSetInputMode(window->handle, GLFW_CURSOR, static_cast<int>(cursorMode)); }

		bool cursorModeChanged() const noexcept { return prevCursorMode != getCursorMode(); }

	};

}
