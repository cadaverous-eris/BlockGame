#include "KeyBind.h"

#include <array>
#include <map>
#include <unordered_map>
#include <cassert>

#include "InputManager.h"

namespace eng::input {

	// owns all KeyBind instances
	std::vector<std::unique_ptr<KeyBind>> keyBindList {};
	// maps of keys to vectors of bound keybinds
	static std::array<std::unordered_map<int, std::vector<KeyBind*>>, 3/* 1 for each KeyType */> bindingMaps;
	// map of KeyBind names to KeyBind*s
	static std::map<std::string, KeyBind*> keyBindsNameMap {};
	// list of all rebindable KeyBinds
	std::vector<KeyBind*> rebindableKeyBinds {};

	namespace keybinds {
		// movement keys
		KeyBind* const MOVE_FORWARD = makeKeyBind("move_forward", { Keys::KEY_W });
		KeyBind* const MOVE_BACKWARD = makeKeyBind("move_backward", { Keys::KEY_S });
		KeyBind* const MOVE_LEFT = makeKeyBind("move_left", { Keys::KEY_A });
		KeyBind* const MOVE_RIGHT = makeKeyBind("move_right", { Keys::KEY_D });
		KeyBind* const JUMP = makeKeyBind("jump", { Keys::KEY_SPACE });
		KeyBind* const SNEAK = makeKeyBind("sneak", { Keys::KEY_SHIFT });
		KeyBind* const PLACE_BLOCK = makeKeyBind("place_block", { MouseButtons::MOUSE_RIGHT });
		KeyBind* const BREAK_BLOCK = makeKeyBind("break_block", { MouseButtons::MOUSE_LEFT });
		// misc. keys
		KeyBind* const INVENTORY = makeKeyBind("inventory", { Keys::KEY_E });
		KeyBind* const PAUSE = makeKeyBind("pause", { Keys::KEY_ESCAPE });
		KeyBind* const MAXIMIZE_WINDOW = makeKeyBind("maximize", { Keys::KEY_F11 });
		KeyBind* const TAKE_SCREENSHOT = makeKeyBind("take_screenshot", { Keys::KEY_F2 });
		// debugging keys
		KeyBind* const PRINT_CAMERA = makeKeyBind("print_camera", { Keys::KEY_ENTER });
		KeyBind* const DEBUG_PLACE_FLUID = makeKeyBind("place_fluid", { Keys::KEY_F });
		KeyBind* const DEBUG_BREAK_FLUID = makeKeyBind("break_fluid", { Keys::KEY_G });
	}

	static KeyBind* initKeyBind(KeyBind* keyBind) {
		assert(!keyBind->name.empty());
		const auto mapEmplaceResult = keyBindsNameMap.try_emplace(keyBind->name, keyBind);
		[[maybe_unused]] const auto duplicateKeyBindName = !mapEmplaceResult.second;
		assert(!duplicateKeyBindName);
		if (keyBind->rebindable) rebindableKeyBinds.push_back(keyBind);
		resetBinding(*keyBind);
		return keyBind;
	}

	KeyBind* makeKeyBind(std::string&& name, KeyInput&& defaultKeyInput) {
		return initKeyBind(keyBindList.emplace_back(makeKeyBindPtr(std::move(name), std::move(defaultKeyInput))).get());
	}
	KeyBind* makeKeyBind(std::string&& name) {
		return initKeyBind(keyBindList.emplace_back(makeKeyBindPtr(std::move(name))).get());
	}
	KeyBind* makeNonRebindableKeyBind(std::string&& name, KeyInput&& keyInput) {
		return initKeyBind(keyBindList.emplace_back(makeKeyBindPtr(std::move(name), std::move(keyInput), false)).get());
	}

	std::unique_ptr<KeyBind> makeKeyBindPtr(std::string&& name, KeyInput&& defaultKeyInput, bool rebindable) {
		return std::unique_ptr<KeyBind>(new KeyBind(std::move(name), std::move(defaultKeyInput), rebindable));
	}
	std::unique_ptr<KeyBind> makeKeyBindPtr(std::string&& name, KeyInput&& defaultKeyInput) {
		return std::unique_ptr<KeyBind>(new KeyBind(std::move(name), std::move(defaultKeyInput)));
	}
	std::unique_ptr<KeyBind> makeKeyBindPtr(std::string&& name) {
		return std::unique_ptr<KeyBind>(new KeyBind(std::move(name)));
	}


	KeyBind::KeyBind(std::string&& name, KeyInput&& defaultKeyInput, bool rebindable) : name(std::move(name)), defaultKeyInput(std::move(defaultKeyInput)), rebindable(rebindable) {}
	KeyBind::KeyBind(std::string&& name) : name(std::move(name)) {}

	void KeyBind::handleInput() {
		prevPressed = pressed;
		released = false;
	}

	void KeyBind::handlePress(const KeyInput& input, const bool callListners) {
		pressed = true;
		if (callListners) {
			for (auto [ cbHandle, callback ] : pressHandlers) callback(input);
		}
	}

	void KeyBind::handleRelease(const KeyInput& input, const bool callListners) {
		released = true;
		if (pressed && callListners) {
			for (auto [cbHandle, callback] : releaseHandlers) callback(input);
		}
		pressed = false;
	}

	void KeyBind::resetState() noexcept {
		pressed = false;
		prevPressed = false;
		released = false;
	}

	void KeyBind::removePressHandler(size_t handlerId) noexcept {
		pressHandlers.erase(
			std::remove_if(pressHandlers.begin(), pressHandlers.end(), [&](const auto& cbPair) noexcept { return handlerId == cbPair.first; }),
			pressHandlers.end()
		);
	}
	void KeyBind::removeReleaseHandler(size_t handlerId) noexcept {
		releaseHandlers.erase(
			std::remove_if(releaseHandlers.begin(), releaseHandlers.end(), [&](const auto& cbPair) noexcept { return handlerId == cbPair.first; }),
			releaseHandlers.end()
		);
	}

	bool KeyBind::matchesKeyInput(const KeyInput& keyInput) const noexcept {
		return this->keyInput && (this->keyInput->key == keyInput.key) && keyInput.modifiers[this->keyInput->modifiers];
	}


	// returns a pointer to the vector of KeyBinds for the given Key, or nullptr if no KeyBinds are bound to the given Key
	std::vector<KeyBind*>* getKeyBindList(const Key& key) {
		auto& bindingMap = bindingMaps[static_cast<int>(key.type)];
		if (bindingMap.empty()) return nullptr;
		auto bindings = bindingMap.find(key.code);
		if (bindings == bindingMap.end()) return nullptr;
		return &(bindings->second);
	}

	// returns a pointer to the vector of KeyBinds for the given Key
	std::vector<KeyBind*>* getOrCreateKeyBindList(const Key& key) {
		auto& bindingMap = bindingMaps[static_cast<int>(key.type)];
		return &bindingMap[key.code];
	}

	KeyBind* findKeyBind(const KeyInput& keyInput) {
		auto keyBinds = getKeyBindList(keyInput.key);
		if (!keyBinds) return nullptr;
		auto match = std::find_if(std::begin(*keyBinds), std::end(*keyBinds), [&](KeyBind* const keyBind) {
			return (keyBind->keyInput && keyInput.modifiers[keyBind->keyInput->modifiers]);
		});
		if (match == std::end(*keyBinds)) return nullptr;
		return *match;
	}

	static KeyInput removeExtraModifiers(KeyInput keyInput) {
		ModifierBits extraMods = ModifierBits::CAPS_LOCK | ModifierBits::NUM_LOCK;
		if ((keyInput.key == Keys::KEY_LEFT_SHIFT) || (keyInput.key == Keys::KEY_RIGHT_SHIFT))
			extraMods |= ModifierBits::SHIFT;
		else if ((keyInput.key == Keys::KEY_LEFT_CONTROL) || (keyInput.key == Keys::KEY_RIGHT_CONTROL))
			extraMods |= ModifierBits::CONTROL;
		else if ((keyInput.key == Keys::KEY_LEFT_ALT) || (keyInput.key == Keys::KEY_RIGHT_ALT))
			extraMods |= ModifierBits::ALT;
		else if ((keyInput.key == Keys::KEY_LEFT_SUPER) || (keyInput.key == Keys::KEY_RIGHT_SUPER))
			extraMods |= ModifierBits::SUPER;
		keyInput.modifiers &= ~extraMods;
		return keyInput;
	}

	void bind(KeyBind& keyBind, const KeyInput& keyInputIn) {
		const KeyInput keyInput = removeExtraModifiers(keyInputIn);

		// do nothing if keyBind is already bound to keyInput
		if (keyBind.keyInput && (*(keyBind.keyInput) == keyInput)) return;

		unbind(keyBind);
		auto keyBinds = getOrCreateKeyBindList(keyInput.key);
		if (!keyBinds) return;

		keyBinds->push_back(&keyBind);
		keyBind.keyInput = keyInput;
		std::sort(std::begin(*keyBinds), std::end(*keyBinds), [&](KeyBind* const a, KeyBind* const b) {
			return a->keyInput->modifiers > b->keyInput->modifiers;
		});
	}

	void unbind(KeyBind& keyBind) {
		if (keyBind.keyInput) {
			auto keyBinds = getKeyBindList(keyBind.keyInput->key);
			if (!keyBinds) return;

			keyBinds->erase(
				std::remove(std::begin(*keyBinds), std::end(*keyBinds), &keyBind),
				std::end(*keyBinds)
			);
			keyBind.keyInput = std::nullopt;
		}
	}

	void resetBinding(KeyBind& keyBind) {
		if (keyBind.defaultKeyInput)
			bind(keyBind, *keyBind.defaultKeyInput);
	}

	void handleKeyBindingsInput() {
		for (auto& keyBind : keyBindList)
			keyBind->handleInput();
	}

	void resetKeyBindStates() {
		for (auto& keyBind : keyBindList)
			keyBind->resetState();
	}

	bool isConflict(const KeyBind& a, const KeyBind& b) {
		return a.keyInput && b.keyInput && (a.keyInput == b.keyInput);
	}

	bool isShadow(const KeyBind& a, const KeyBind& b) {
		if (!a.keyInput || !b.keyInput) return false;
		if (a.keyInput->key != b.keyInput->key) return false;
		if (a.keyInput->modifiers == b.keyInput->modifiers) return false;
		return (a.keyInput->modifiers[b.keyInput->modifiers] || b.keyInput->modifiers[a.keyInput->modifiers]);
	}

	KeyBind* findKeyBind(const std::string& name) {
		if (auto it = keyBindsNameMap.find(name); it != keyBindsNameMap.end()) {
			return it->second;
		}
		return nullptr;
	}

}