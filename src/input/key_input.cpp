#include "key_input.h"


#include <fmt/core.h>

#include "util/type_traits.h"

namespace eng::input {

	static bool isPrintableNamedKey(const Keys& namedKey) {
		constexpr bool isKeypadPrintable = false; // TODO: change to true?
		const auto id = to_underlying(namedKey);
		if (((id >= to_underlying(Keys::KEY_0)) && (id <= to_underlying(Keys::KEY_9))) ||
			((id >= to_underlying(Keys::KEY_A)) && (id <= to_underlying(Keys::KEY_Z)))) return true;
		if ((id >= to_underlying(Keys::KEY_KP_0)) && (id <= to_underlying(Keys::KEY_KP_9))) return isKeypadPrintable;
		switch (namedKey) {
			case Keys::KEY_APOSTROPHE:
			case Keys::KEY_COMMA:
			case Keys::KEY_MINUS:
			case Keys::KEY_PERIOD:
			case Keys::KEY_SLASH:
			case Keys::KEY_SEMICOLON:
			case Keys::KEY_EQUAL:
			case Keys::KEY_LEFT_BRACKET:
			case Keys::KEY_RIGHT_BRACKET:
			case Keys::KEY_BACKSLASH:
			case Keys::KEY_WORLD_1:
			case Keys::KEY_WORLD_2:
				return true;
			case Keys::KEY_KP_DECIMAL:
			case Keys::KEY_KP_DIVIDE:
			case Keys::KEY_KP_MULTIPLY:
			case Keys::KEY_KP_SUBTRACT:
			case Keys::KEY_KP_ADD:
			case Keys::KEY_KP_EQUAL:
				return isKeypadPrintable;
			default:
				return false;
		}
	}

	std::string to_string(const Keys& namedKey) {
		if (namedKey == Keys::KEY_UNKNOWN) return "Unknown Key";
		// TODO: use glfwGetKeyName(to_underlying(namedKey), -1)? figure out what to do about symbols and keypad keys
		//if (isPrintableNamedKey(namedKey)) {
		//	const char* keyName = glfwGetKeyName(to_underlying(namedKey), -1);
		//	if (keyName && (keyName[0] != '\0'))
		//		return std::string(keyName);
		//}
		switch (namedKey) {
			case Keys::KEY_SPACE: return "SPACE";
			case Keys::KEY_APOSTROPHE: return "APOSTROPHE";
			case Keys::KEY_COMMA: return "COMMA";
			case Keys::KEY_MINUS: return "MINUS";
			case Keys::KEY_PERIOD: return "PERIOD";
			case Keys::KEY_SLASH: return "SLASH";
			case Keys::KEY_0: return "0";
			case Keys::KEY_1: return "1";
			case Keys::KEY_2: return "2";
			case Keys::KEY_3: return "3";
			case Keys::KEY_4: return "4";
			case Keys::KEY_5: return "5";
			case Keys::KEY_6: return "6";
			case Keys::KEY_7: return "7";
			case Keys::KEY_8: return "8";
			case Keys::KEY_9: return "9";
			case Keys::KEY_SEMICOLON: return "SEMICOLON";
			case Keys::KEY_EQUAL: return "EQUALS";
			case Keys::KEY_A: return "A";
			case Keys::KEY_B: return "B";
			case Keys::KEY_C: return "C";
			case Keys::KEY_D: return "D";
			case Keys::KEY_E: return "E";
			case Keys::KEY_F: return "F";
			case Keys::KEY_G: return "G";
			case Keys::KEY_H: return "H";
			case Keys::KEY_I: return "I";
			case Keys::KEY_J: return "J";
			case Keys::KEY_K: return "K";
			case Keys::KEY_L: return "L";
			case Keys::KEY_M: return "M";
			case Keys::KEY_N: return "N";
			case Keys::KEY_O: return "O";
			case Keys::KEY_P: return "P";
			case Keys::KEY_Q: return "Q";
			case Keys::KEY_R: return "R";
			case Keys::KEY_S: return "S";
			case Keys::KEY_T: return "T";
			case Keys::KEY_U: return "U";
			case Keys::KEY_V: return "V";
			case Keys::KEY_W: return "W";
			case Keys::KEY_X: return "X";
			case Keys::KEY_Y: return "Y";
			case Keys::KEY_Z: return "Z";
			case Keys::KEY_LEFT_BRACKET: return "LEFT BRACKET";
			case Keys::KEY_BACKSLASH: return "BACKSLASH";
			case Keys::KEY_RIGHT_BRACKET: return "RIGHT BRACKET";
			case Keys::KEY_GRAVE_ACCENT: return "GRAVE";
			case Keys::KEY_WORLD_1: return "WORLD 1";
			case Keys::KEY_WORLD_2: return "WORLD 2";
			case Keys::KEY_ESCAPE: return "ESCAPE";
			case Keys::KEY_ENTER: return "ENTER";
			case Keys::KEY_TAB: return "TAB";
			case Keys::KEY_BACKSPACE: return "BACKSPACE";
			case Keys::KEY_INSERT: return "INSERT";
			case Keys::KEY_DELETE: return "DELETE";
			case Keys::KEY_RIGHT: return "RIGHT";
			case Keys::KEY_LEFT: return "LEFT";
			case Keys::KEY_DOWN: return "DOWN";
			case Keys::KEY_UP: return "UP";
			case Keys::KEY_PAGE_UP: return "PAGE UP";
			case Keys::KEY_PAGE_DOWN: return "PAGE DOWN";
			case Keys::KEY_HOME: return "HOME";
			case Keys::KEY_END: return "END";
			case Keys::KEY_CAPS_LOCK: return "CAPS LOCK";
			case Keys::KEY_SCROLL_LOCK: return "SCROLL LOCK";
			case Keys::KEY_NUM_LOCK: return "NUM LOCK";
			case Keys::KEY_PRINT_SCREEN: return "PRINT SCREEN";
			case Keys::KEY_PAUSE: return "PAUSE";
			case Keys::KEY_F1: return "F1";
			case Keys::KEY_F2: return "F2";
			case Keys::KEY_F3: return "F3";
			case Keys::KEY_F4: return "F4";
			case Keys::KEY_F5: return "F5";
			case Keys::KEY_F6: return "F6";
			case Keys::KEY_F7: return "F7";
			case Keys::KEY_F8: return "F8";
			case Keys::KEY_F9: return "F9";
			case Keys::KEY_F10: return "F10";
			case Keys::KEY_F11: return "F11";
			case Keys::KEY_F12: return "F12";
			case Keys::KEY_F13: return "F13";
			case Keys::KEY_F14: return "F14";
			case Keys::KEY_F15: return "F15";
			case Keys::KEY_F16: return "F16";
			case Keys::KEY_F17: return "F17";
			case Keys::KEY_F18: return "F18";
			case Keys::KEY_F19: return "F19";
			case Keys::KEY_F20: return "F20";
			case Keys::KEY_F21: return "F22";
			case Keys::KEY_F22: return "F23";
			case Keys::KEY_F23: return "F24";
			case Keys::KEY_F24: return "F25";
			case Keys::KEY_F25: return "F26";
			case Keys::KEY_KP_0: return "KEYPAD 0";
			case Keys::KEY_KP_1: return "KEYPAD 1";
			case Keys::KEY_KP_2: return "KEYPAD 2";
			case Keys::KEY_KP_3: return "KEYPAD 3";
			case Keys::KEY_KP_4: return "KEYPAD 4";
			case Keys::KEY_KP_5: return "KEYPAD 5";
			case Keys::KEY_KP_6: return "KEYPAD 6";
			case Keys::KEY_KP_7: return "KEYPAD 7";
			case Keys::KEY_KP_8: return "KEYPAD 8";
			case Keys::KEY_KP_9: return "KEYPAD 9";
			case Keys::KEY_KP_DECIMAL: return "KEYPAD DECIMAL";
			case Keys::KEY_KP_DIVIDE: return "KEYPAD DIVIDE";
			case Keys::KEY_KP_MULTIPLY: return "KEYPAD MULTIPLY";
			case Keys::KEY_KP_SUBTRACT: return "KEYPAD MINUS";
			case Keys::KEY_KP_ADD: return "KEYPAD PLUS";
			case Keys::KEY_KP_ENTER: return "KEYPAD ENTER";
			case Keys::KEY_KP_EQUAL: return "KEYPAD EQUALS";
			case Keys::KEY_LEFT_SHIFT: return "LEFT SHIFT";
			case Keys::KEY_LEFT_CONTROL: return "LEFT CONTROL";
			case Keys::KEY_LEFT_ALT: return "LEFT ALT";
			case Keys::KEY_LEFT_SUPER: return "LEFT SUPER";
			case Keys::KEY_RIGHT_SHIFT: return "RIGHT SHIFT";
			case Keys::KEY_RIGHT_CONTROL: return "RIGHT CONTROL";
			case Keys::KEY_RIGHT_ALT: return "RIGHT ALT";
			case Keys::KEY_RIGHT_SUPER: return "RIGHT SUPER";
			case Keys::KEY_MENU: return "MENU";
			default: break;
		}
		const auto scanCode = glfwGetKeyScancode(to_underlying(namedKey));
		return (scanCode != -1) ? fmt::format("Key {}", scanCode) : std::string("Unknown Key");
	}
	std::ostream& operator <<(std::ostream& stream, const Keys& namedKey) {
		return stream << to_string(namedKey);
	}

	std::string to_string(const MouseButtons& mouseButton) {
		switch (mouseButton) {
		case MouseButtons::MOUSE_LEFT:
			return "Left Click";
		case MouseButtons::MOUSE_RIGHT:
			return "Right Click";
		case MouseButtons::MOUSE_MIDDLE:
			return "Middle Click";
		default:
			return fmt::format("Mouse {}", to_underlying(mouseButton) + 1);
		}
	}
	std::ostream& operator <<(std::ostream& stream, const MouseButtons& mouseButton);

	std::string to_string(const ModifierBits& modifierBits) {
		using namespace std::string_literals;
		if (!modifierBits) return "No Modifiers"s; // TODO: remove?
		const bool shift = modifierBits[ModifierBits::SHIFT];
		const bool control = modifierBits[ModifierBits::CONTROL];
		const bool alt = modifierBits[ModifierBits::ALT];
		const bool super = modifierBits[ModifierBits::SUPER];
		const bool capsLock = modifierBits[ModifierBits::CAPS_LOCK];
		const bool numLock = modifierBits[ModifierBits::NUM_LOCK];
		std::string str {};
		bool firstKeyName = true;
		if (shift) { str += "Shift"s;											firstKeyName = false; }
		if (control) { str += (firstKeyName ? "Control"s : " + Control"s);		firstKeyName = false; }
		if (alt) { str += (firstKeyName ? "Alt"s : " + Alt"s);					firstKeyName = false; }
		if (super) { str += (firstKeyName ? "Super"s : " + Super"s);			firstKeyName = false; }
		if (capsLock) { str += (firstKeyName ? "Caps Lock"s : " + Caps Lock"s);	firstKeyName = false; }
		if (numLock) { str += (firstKeyName ? "Num Lock"s : " + Num Lock"s);						  }
		return str;
	}
	std::ostream& operator <<(std::ostream& stream, const ModifierBits& modifierBits) {
		using namespace std::string_literals;
		if (!modifierBits) return stream << "No Modifiers"; // TODO: remove?
		const bool shift = modifierBits[ModifierBits::SHIFT];
		const bool control = modifierBits[ModifierBits::CONTROL];
		const bool alt = modifierBits[ModifierBits::ALT];
		const bool super = modifierBits[ModifierBits::SUPER];
		const bool capsLock = modifierBits[ModifierBits::CAPS_LOCK];
		const bool numLock = modifierBits[ModifierBits::NUM_LOCK];
		//stream << (shift ? '1' : '0') << (control ? '1' : '0') << (alt ? '1' : '0')
		//		 << (super ? '1' : '0') << (capsLock ? '1' : '0') << (numLock ? '1' : '0') << ' ';
		bool firstKeyName = true;
		if (shift) { stream << "Shift"s;											firstKeyName = false; }
		if (control) { stream << (firstKeyName ? "Control"s : " + Control"s);		firstKeyName = false; }
		if (alt) { stream << (firstKeyName ? "Alt"s : " + Alt"s);					firstKeyName = false; }
		if (super) { stream << (firstKeyName ? "Super"s : " + Super"s);				firstKeyName = false; }
		if (capsLock) { stream << (firstKeyName ? "Caps Lock"s : " + Caps Lock"s);	firstKeyName = false; }
		if (numLock) { stream << (firstKeyName ? "Num Lock"s : " + Num Lock"s);							  }
		return stream;
	}

	std::string to_string(const Key& key) {
		switch (key.type) {
			case KeyType::KEY_NAMED:
				return to_string(Keys{ key.code });
			case KeyType::MOUSE:
				return to_string(MouseButtons{ key.code });
			case KeyType::KEY_SCANCODE:
				// TODO: use glfwGetKeyName(GLFW_KEY_UNKNOWN, key.code)?
				return fmt::format("Key {}", key.code);
		}
		return "Invalid Key";
	}
	std::ostream& operator <<(std::ostream& stream, const Key& key) {
		return stream << to_string(key);
	}

	std::string to_string(const KeyInput& keyInput) {
		auto str = to_string(keyInput.key);
		if (keyInput.modifiers)
			str += " + " + to_string(keyInput.modifiers);
		return str;
	}
	std::ostream& operator <<(std::ostream& stream, const KeyInput& keyInput) {
		return stream << to_string(keyInput);
	}

}