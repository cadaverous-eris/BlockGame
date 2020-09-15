#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <memory>

#include "key_input.h"

namespace eng::input {

	class KeyBind;

	// owns all KeyBind instances
	extern std::vector<std::unique_ptr<KeyBind>> keyBindList;


	namespace keybinds {
		// movement keys
		extern KeyBind* const MOVE_FORWARD;
		extern KeyBind* const MOVE_BACKWARD;
		extern KeyBind* const MOVE_LEFT;
		extern KeyBind* const MOVE_RIGHT;
		extern KeyBind* const JUMP;
		extern KeyBind* const SNEAK;
		extern KeyBind* const PLACE_BLOCK;
		extern KeyBind* const BREAK_BLOCK;
		// misc. keys
		extern KeyBind* const INVENTORY;
		extern KeyBind* const PAUSE;
		extern KeyBind* const MAXIMIZE_WINDOW;
		extern KeyBind* const TAKE_SCREENSHOT;
		// debugging keys
		extern KeyBind* const PRINT_CAMERA;
		extern KeyBind* const DEBUG_PLACE_FLUID;
		extern KeyBind* const DEBUG_BREAK_FLUID;
	}


	KeyBind* makeKeyBind(std::string&&, KeyInput&&);
	KeyBind* makeKeyBind(std::string&&);
	void bind(KeyBind&, const KeyInput&);
	void unbind(KeyBind&);
	KeyBind* findKeyBind(const KeyInput&);
	// returns true if KeyBind a conflicts with KeyBind b
	bool isConflict(const KeyBind& a, const KeyBind& b);
	// returns true if KeyBind a shadows KeyBind b or vice versa
	bool isShadow(const KeyBind& a, const KeyBind& b);

	std::vector<KeyBind*>* getKeyBindList(const Key& key);

	void key_callback(GLFWwindow*, int32_t, int32_t, int32_t, int32_t);
	void mouse_button_callback(GLFWwindow*, int32_t, int32_t, int32_t);

	std::unique_ptr<KeyBind> makeKeyBindPtr(std::string&&, KeyInput&&);
	std::unique_ptr<KeyBind> makeKeyBindPtr(std::string&&);

	class KeyBind {
		friend std::unique_ptr<KeyBind> makeKeyBindPtr(std::string&&, KeyInput&&);
		friend std::unique_ptr<KeyBind> makeKeyBindPtr(std::string&&);
		friend void bind(KeyBind&, const KeyInput&);
		friend void unbind(KeyBind&);
		friend KeyBind* findKeyBind(const KeyInput&);
		friend bool isConflict(const KeyBind& a, const KeyBind& b);
		friend bool isShadow(const KeyBind& a, const KeyBind& b);
		friend void key_callback(GLFWwindow*, int32_t, int32_t, int32_t, int32_t);
		friend void mouse_button_callback(GLFWwindow*, int32_t, int32_t, int32_t);
	public:
		const std::string name;
		const std::optional<KeyInput> defaultKeyInput;

	private:
		std::optional<KeyInput> keyInput;

		bool pressed = false, prevPressed = false;
		bool released = false;

		std::vector<std::pair<size_t, std::function<void(const KeyInput&)>>> pressHandlers, releaseHandlers;

		size_t pressHandlerId = 1, releaseHandlerId = 1;

		explicit KeyBind(std::string&& name, KeyInput&& defaultKeyInput);
		explicit KeyBind(std::string&& name);
	public:
		~KeyBind() = default;

		KeyBind(const KeyBind&) = delete;
		KeyBind& operator =(const KeyBind&) = delete;

		KeyBind(KeyBind&& keyBind) = delete;
		KeyBind& operator =(KeyBind&&) = delete;

		inline std::optional<KeyInput> getBoundKeyInput() const noexcept { return keyInput; }

		// called from InputManager::handleInput()
		void handleInput();

		// resets the pressed/released state
		void resetState() noexcept;

		// returns true if the key is being pressed
		inline bool isPressed() const noexcept { return pressed; }
		// returns true if the key was just pressed down this input cycle
		inline bool didPress() const noexcept { return pressed && !prevPressed; }
		// returns true if the key was just released this input cycle
		inline bool didRelease() const noexcept { return prevPressed && !pressed; }

		inline bool isBound() const noexcept { return static_cast<bool>(keyInput); }

		template<typename Handler, std::enable_if_t<std::is_invocable_v<Handler, KeyInput>>* = nullptr>
		[[nodiscard]] size_t addPressHandler(Handler&& handler) {
			size_t id = pressHandlerId++;
			pressHandlers.emplace_back(id, std::forward<Handler>(handler));
			return id;
		}
		template<typename Handler, std::enable_if_t<std::is_invocable_v<Handler, KeyInput>>* = nullptr>
		[[nodiscard]] size_t addReleaseHandler(Handler && handler) {
			size_t id = releaseHandlerId++;
			releaseHandlers.emplace_back(id, std::forward<Handler>(handler));
			return id;
		}
		void removePressHandler(size_t handle) noexcept;
		void removeReleaseHandler(size_t handle) noexcept;

	private:
		void handlePress(const KeyInput& input, bool callListeners = true);
		void handleRelease(const KeyInput& input, bool callListeners = true);

	public:
		// RAII wrapper for press handlers
		class PressHandler {
			KeyBind* const keybind;
			size_t id = 0;
		public:
			template<typename Handler, std::enable_if_t<std::is_invocable_v<Handler, KeyInput>>* = nullptr>
			PressHandler(KeyBind& keybind, Handler&& handler) : keybind(&keybind) {
				id = this->keybind->addPressHandler(std::forward<Handler>(handler));
			}
			PressHandler(const PressHandler&) = delete;
			PressHandler& operator =(const PressHandler&) = delete;
			PressHandler(PressHandler&& b) noexcept : keybind(b.keybind), id(std::exchange(b.id, 0)) {}
			PressHandler& operator =(PressHandler&&) = delete;
			~PressHandler() {
				if (id > 0) keybind->removePressHandler(id);
			}
			inline size_t getId() const noexcept { return id; }
		};
		// RAII wrapper for release handlers
		class ReleaseHandler {
			KeyBind* const keybind;
			size_t id;
		public:
			template<typename Handler, std::enable_if_t<std::is_invocable_v<Handler, KeyInput>>* = nullptr>
			ReleaseHandler(KeyBind& keybind, Handler&& handler) : keybind(&keybind) {
				id = this->keybind->addReleaseHandler(std::forward<Handler>(handler));
			}
			ReleaseHandler(const ReleaseHandler&) = delete;
			ReleaseHandler& operator =(const ReleaseHandler&) = delete;
			ReleaseHandler(ReleaseHandler&& b) noexcept : keybind(b.keybind), id(std::exchange(b.id, 0)) {}
			ReleaseHandler& operator =(ReleaseHandler&&) = delete;
			~ReleaseHandler() {
				if (id > 0) keybind->removeReleaseHandler(id);
			}
			inline size_t getId() const noexcept { return id; }
		};
	};

	void handleKeyBindingsInput();

	// resets a keybind to it's default
	void resetBinding(KeyBind&);

	// resets the pressed/released state of every keybind
	void resetKeyBindStates();

	KeyBind* findKeyBind(const std::string& name);

	template<typename F, std::enable_if_t<std::is_invocable_v<F, KeyBind&>, int> = 0>
	void forEachKeyBind(const F& f) noexcept(std::is_nothrow_invocable_v<F, KeyBind&>) {
		for (auto& keyBindPtr : keyBindList)
			f(*keyBindPtr);
	}

}