#pragma once

#include <utility>

#include "util/type_traits.h"
#include "Settings.h"
#include "input/key_input.h"
#include "input/KeyBind.h"
#include "util/jonk/Jonk.h"

namespace jonk {

	template<>
	struct JonkTypeCompat<eng::Settings> {
		static Jonk toJonk(const eng::Settings& settings) {
			JonkObject jonkObj {};
			jonkObj.emplace("chunk_load_radius", settings.chunkLoadRadius);
			jonkObj.emplace("ui_scale", settings.uiScale);
			jonkObj.emplace("window_size", settings.windowSize);

			{ // save keybinds to settings jonk
				using namespace eng::input;
				JonkObject keyBindsObj {};
				forEachKeyBind([&](KeyBind& keyBind) {
					if (const auto binding = keyBind.getBoundKeyInput(); binding) {
						keyBindsObj.emplace(keyBind.name, *binding);
					} else {
						keyBindsObj.emplace(keyBind.name, nullptr);
					}
				});
				jonkObj.emplace("keybinds", std::move(keyBindsObj));
			}

			return jonkObj;
		}
	};

	template<>
	struct JonkTypeCompat<eng::input::KeyInput> {
		// KeyInputs are serialized as arrays with 2-3 integers
		// the first int is the key type, the second int is the key code, the third int is the modifier keys (if present)
		// [ <0|1|2>, <int keyCode>, <uint modifiers|null>, ]
		using KeyInput = eng::input::KeyInput;
		static Jonk toJonk(const KeyInput& keyInput) {
			using namespace eng::input;
			using eng::to_underlying;
			JonkArray arr {};
			arr.emplace_back(to_underlying(keyInput.key.type));
			arr.emplace_back(keyInput.key.code);
			arr.emplace_back(static_cast<uint8_t>(keyInput.modifiers));
			return arr;
		}
		static bool validateKeyType(const Jonk& j) noexcept {
			using eng::to_underlying;
			using namespace eng::input;
			if (!j.isIntegral()) return false;
			const auto ji = j.getInt();
			return (ji == to_underlying(KeyType::KEY_NAMED)) ||
				   (ji == to_underlying(KeyType::KEY_SCANCODE)) ||
				   (ji == to_underlying(KeyType::MOUSE));
		}
		static bool validateModifiers(const Jonk& j) noexcept {
			using namespace eng::input;
			if (!j.isIntegral()) return false;
			const auto ji = j.getInt();
			constexpr auto allMods = static_cast<JonkInt>(ModifierBits::SHIFT | ModifierBits::CONTROL |
														  ModifierBits::ALT | ModifierBits::SUPER |
														  ModifierBits::CAPS_LOCK | ModifierBits::NUM_LOCK);
			return ((ji >= 0) && (ji <= allMods));
		}
		static KeyInput fromJonk(const Jonk& jonk) {
			using namespace eng::input;
			if (jonk.isArray()) {
				const JonkArray& arr = jonk.asArray();
				if ((arr.size() == 2) || (arr.size() == 3)) {
					if (validateKeyType(arr[0]) && arr[1].isIntegral()) {
						const KeyType keyType = KeyType { arr[0].get<int>() };
						const int keyCode = arr[1].get<int>();
						ModifierBits modifiers = ModifierBits::NONE;
						if ((arr.size() == 3) && !arr[2].isNull()) {
							if (validateModifiers(arr[2]))
								modifiers = ModifierBits::fromByte(arr[2].get<uint8_t>());
							else
								throw bad_jonk_cast();
						}
						return { Key(keyType, keyCode), modifiers };
					}
				}
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (!jonk.isArray())
				return false;
			const JonkArray& arr = jonk.asArray();
			if ((arr.size() != 2) && (arr.size() != 3))
				return false;
			return validateKeyType(arr[0]) && arr[1].isIntegral() &&
				   ((arr.size() == 2) || arr[2].isNull() || validateModifiers(arr[2]));
		}
	};

}