#include "Game.h" // must be included before anything else
#include "InputManager.h"

namespace eng {
//	void key_callback(GLFWwindow* windowHandle, int key, int scancode, int action, int mods); // TODO: remove
}

namespace eng::input {

	InputManager::InputManager(Window* window) : window(window), prevCursorMode(static_cast<CursorMode>(glfwGetInputMode(window->handle, GLFW_CURSOR))) {
		// TODO: implement

		// initialize the cursor position
		glfwGetCursorPos(window->handle, &cursorPos.x, &cursorPos.y);
		prevCursorPos = cursorPos;

		// set the window's input callbacks
		glfwSetKeyCallback(window->handle, key_callback);
		glfwSetCharCallback(window->handle, text_input_callback);
		glfwSetMouseButtonCallback(window->handle, mouse_button_callback);
		glfwSetScrollCallback(window->handle, scroll_callback);

		// use raw mouse motion if it's supported
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window->handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		// Testing ModifierBits
		/*{ // TODO: remove
			std::cout << std::endl;

			std::cout << "Shift:     " << ModifierBits::SHIFT << "\n";
			std::cout << "Control:   " << ModifierBits::CONTROL << "\n";
			std::cout << "Alt:       " << ModifierBits::ALT << "\n";
			std::cout << "Super:     " << ModifierBits::SUPER << "\n";
			std::cout << "Caps Lock: " << ModifierBits::CAPS_LOCK << "\n";
			std::cout << "Num Lock:  " << ModifierBits::NUM_LOCK << "\n";
			std::cout << std::endl;

			ModifierBits test = (ModifierBits::SHIFT | ModifierBits::CAPS_LOCK);
			std::cout << "modifiers: " << test << "\n";
			test[ModifierBits::SHIFT] = false;
			std::cout << "modifiers[SHIFT] = false" << "\n";
			std::cout << "modifiers: " << test << "\n";
			test[ModifierBits::CONTROL] = true;
			std::cout << "modifiers[CONTROL] = true" << "\n";
			std::cout << "modifiers: " << test << "\n";
			test[ModifierBits::SHIFT | ModifierBits::NUM_LOCK] = true;
			std::cout << "modifiers[SHIFT | NUM_LOCK] = true" << "\n";
			std::cout << "modifiers: " << test << "\n";
			test = ModifierBits::SUPER;
			std::cout << "modifiers = SUPER" << "\n";
			std::cout << "modifiers: " << test << "\n";
			test = ModifierBits{};
			std::cout << "modifiers = {}" << "\n";
			std::cout << "modifiers: " << test << "\n";
			std::cout << std::endl;

			std::cout << "SHIFT | CAPS_LOCK                              = " << (ModifierBits::SHIFT | ModifierBits::CAPS_LOCK) << "\n";
			std::cout << "(SHIFT | CAPS_LOCK)[SHIFT]                     = " << (ModifierBits::SHIFT | ModifierBits::CAPS_LOCK)[ModifierBits::SHIFT] << "\n";
			std::cout << "(SHIFT | CAPS_LOCK)[CAPS_LOCK]                 = " << (ModifierBits::SHIFT | ModifierBits::CAPS_LOCK)[ModifierBits::CAPS_LOCK] << "\n";
			std::cout << "(SHIFT | CAPS_LOCK)[NUM_LOCK]                  = " << (ModifierBits::SHIFT | ModifierBits::CAPS_LOCK)[ModifierBits::NUM_LOCK] << "\n";
			std::cout << "(SHIFT | CAPS_LOCK)[SHIFT | CAPS_LOCK]         = " << (ModifierBits::SHIFT | ModifierBits::CAPS_LOCK)[ModifierBits::SHIFT | ModifierBits::CAPS_LOCK] << "\n";
			std::cout << "(SHIFT | CAPS_LOCK)[SHIFT | SUPER | CAPS_LOCK] = " << (ModifierBits::SHIFT | ModifierBits::CAPS_LOCK)[ModifierBits::SHIFT | ModifierBits::SUPER | ModifierBits::CAPS_LOCK] << "\n";

			std::cout << std::endl;
		}*/
	}

	void InputManager::input() {
		// reset mouse scroll offset
		mouseScrollOffset = { 0, 0 };

		prevCursorMode = getCursorMode();

		glfwPollEvents();

		// update the cursor position
		prevCursorPos = cursorPos;
		glfwGetCursorPos(window->handle, &cursorPos.x, &cursorPos.y);

		// update key bindings
		handleKeyBindingsInput();
	}

	void InputManager::update() {
		
	}


	// INPUT CALLBACKS

	void key_callback(GLFWwindow* windowPtr, int32_t keyCode, int32_t scancode, int32_t actionInt, int32_t modsInt) {
		//const Window* window = (const Window*) glfwGetWindowUserPointer(windowPtr);
		KeyAction action = static_cast<KeyAction>(actionInt);
		KeyType keyType = (keyCode == static_cast<int>(Keys::KEY_UNKNOWN)) ? KeyType::KEY_SCANCODE : KeyType::KEY_NAMED;
		ModifierBits modifiers(modsInt);
		Key key(keyType, (keyType == KeyType::KEY_NAMED) ? keyCode : scancode);
		KeyInput keyInput{ key, modifiers };

		/*switch (action) {
		case KeyAction::PRESS:   std::cout << "Key pressed with modifiers:  " << modifiers << " " << modsInt << std::endl; break;
		case KeyAction::RELEASE: std::cout << "Key released with modifiers: " << modifiers << " " << modsInt << std::endl; break;
		case KeyAction::REPEAT:  std::cout << "Key repeated with modifiers: " << modifiers << " " << modsInt << std::endl; break;
		}*/

		if (action == KeyAction::PRESS) {
			auto keyBind = findKeyBind(keyInput);
			if (keyBind) {
				keyBind->handlePress(keyInput);
			}
		} else if (action == KeyAction::RELEASE) {
			auto keyBindList = getKeyBindList(key);
			if (keyBindList) {
				for (auto& keyBind : *keyBindList) {
					keyBind->handleRelease(keyInput);
				}
			}
		}
	}

	void mouse_button_callback(GLFWwindow* windowPtr, int32_t button, int32_t actionInt, int32_t modsInt) {
		ModifierBits modifiers(modsInt);
		KeyAction action = static_cast<KeyAction>(actionInt);
		Key key(KeyType::MOUSE, button);
		KeyInput keyInput { key, modifiers };

		/*switch (action) {
		case KeyAction::PRESS:   std::cout << "Mouse clicked with modifiers:  " << modifiers << " " << modsInt << std::endl; break;
		case KeyAction::RELEASE: std::cout << "Mouse released with modifiers: " << modifiers << " " << modsInt << std::endl; break;
		}*/

		if (action == KeyAction::PRESS) {
			auto keyBind = findKeyBind(keyInput);
			if (keyBind) {
				keyBind->handlePress(keyInput);
			}
		} else if (action == KeyAction::RELEASE) {
			auto keyBindList = getKeyBindList(key);
			if (keyBindList) {
				for (auto& keyBind : *keyBindList) {
					keyBind->handleRelease(keyInput);
				}
			}
		}
	}

	void scroll_callback(GLFWwindow* windowPtr, double xOffset, double yOffset) {
		const Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowPtr));
		window->game->inputManager.mouseScrollOffset += glm::dvec2(xOffset, yOffset);
	}

	void text_input_callback(GLFWwindow* windowPtr, uint32_t codePoint) {
		// TODO: implement
	}

}