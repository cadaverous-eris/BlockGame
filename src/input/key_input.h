#pragma once

#include <iostream>

#include <GLFW/glfw3.h>

namespace eng::input {

	enum class KeyAction {
		PRESS   = GLFW_PRESS,
		RELEASE = GLFW_RELEASE,
		REPEAT  = GLFW_REPEAT,
	};
	enum class KeyType {
		KEY_NAMED    = 0, // keyboard keys that are named by glfw
		KEY_SCANCODE = 1, // keyboard keys that aren't named by glfw
		MOUSE        = 2, // mouse buttons
	};

	enum class Keys {
		KEY_UNKNOWN       = GLFW_KEY_UNKNOWN,
		KEY_SPACE         = GLFW_KEY_SPACE,
		KEY_APOSTROPHE    = GLFW_KEY_APOSTROPHE,
		KEY_COMMA         = GLFW_KEY_COMMA,
		KEY_MINUS         = GLFW_KEY_MINUS,
		KEY_PERIOD        = GLFW_KEY_PERIOD,
		KEY_SLASH         = GLFW_KEY_SLASH,
		KEY_0             = GLFW_KEY_0,
		KEY_1             = GLFW_KEY_1,
		KEY_2             = GLFW_KEY_2,
		KEY_3             = GLFW_KEY_3,
		KEY_4             = GLFW_KEY_4,
		KEY_5             = GLFW_KEY_5,
		KEY_6             = GLFW_KEY_6,
		KEY_7             = GLFW_KEY_7,
		KEY_8             = GLFW_KEY_8,
		KEY_9             = GLFW_KEY_9,
		KEY_SEMICOLON     = GLFW_KEY_SEMICOLON,
		KEY_EQUAL         = GLFW_KEY_EQUAL,
		KEY_A             = GLFW_KEY_A,
		KEY_B             = GLFW_KEY_B,
		KEY_C             = GLFW_KEY_C,
		KEY_D             = GLFW_KEY_D,
		KEY_E             = GLFW_KEY_E,
		KEY_F             = GLFW_KEY_F,
		KEY_G             = GLFW_KEY_G,
		KEY_H             = GLFW_KEY_H,
		KEY_I             = GLFW_KEY_I,
		KEY_J             = GLFW_KEY_J,
		KEY_K             = GLFW_KEY_K,
		KEY_L             = GLFW_KEY_L,
		KEY_M             = GLFW_KEY_M,
		KEY_N             = GLFW_KEY_N,
		KEY_O             = GLFW_KEY_O,
		KEY_P             = GLFW_KEY_P,
		KEY_Q             = GLFW_KEY_Q,
		KEY_R             = GLFW_KEY_R,
		KEY_S             = GLFW_KEY_S,
		KEY_T             = GLFW_KEY_T,
		KEY_U             = GLFW_KEY_U,
		KEY_V             = GLFW_KEY_V,
		KEY_W             = GLFW_KEY_W,
		KEY_X             = GLFW_KEY_X,
		KEY_Y             = GLFW_KEY_Y,
		KEY_Z             = GLFW_KEY_Z,
		KEY_LEFT_BRACKET  = GLFW_KEY_LEFT_BRACKET,
		KEY_BACKSLASH     = GLFW_KEY_BACKSLASH,
		KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
		KEY_GRAVE_ACCENT  = GLFW_KEY_GRAVE_ACCENT,
		KEY_WORLD_1       = GLFW_KEY_WORLD_1,
		KEY_WORLD_2       = GLFW_KEY_WORLD_2,
		KEY_ESCAPE        = GLFW_KEY_ESCAPE,
		KEY_ENTER         = GLFW_KEY_ENTER,
		KEY_TAB           = GLFW_KEY_TAB,
		KEY_BACKSPACE     = GLFW_KEY_BACKSPACE,
		KEY_INSERT        = GLFW_KEY_INSERT,
		KEY_DELETE        = GLFW_KEY_DELETE,
		KEY_RIGHT         = GLFW_KEY_RIGHT,
		KEY_LEFT          = GLFW_KEY_LEFT,
		KEY_DOWN          = GLFW_KEY_DOWN,
		KEY_UP            = GLFW_KEY_UP,
		KEY_PAGE_UP       = GLFW_KEY_PAGE_UP,
		KEY_PAGE_DOWN     = GLFW_KEY_PAGE_DOWN,
		KEY_HOME          = GLFW_KEY_HOME,
		KEY_END           = GLFW_KEY_END,
		KEY_CAPS_LOCK     = GLFW_KEY_CAPS_LOCK,
		KEY_SCROLL_LOCK   = GLFW_KEY_SCROLL_LOCK,
		KEY_NUM_LOCK      = GLFW_KEY_NUM_LOCK,
		KEY_PRINT_SCREEN  = GLFW_KEY_PRINT_SCREEN,
		KEY_PAUSE         = GLFW_KEY_PAUSE,
		KEY_F1            = GLFW_KEY_F1,
		KEY_F2            = GLFW_KEY_F2,
		KEY_F3            = GLFW_KEY_F3,
		KEY_F4            = GLFW_KEY_F4,
		KEY_F5            = GLFW_KEY_F5,
		KEY_F6            = GLFW_KEY_F6,
		KEY_F7            = GLFW_KEY_F7,
		KEY_F8            = GLFW_KEY_F8,
		KEY_F9            = GLFW_KEY_F9,
		KEY_F10           = GLFW_KEY_F10,
		KEY_F11           = GLFW_KEY_F11,
		KEY_F12           = GLFW_KEY_F12,
		KEY_F13           = GLFW_KEY_F13,
		KEY_F14           = GLFW_KEY_F14,
		KEY_F15           = GLFW_KEY_F15,
		KEY_F16           = GLFW_KEY_F16,
		KEY_F17           = GLFW_KEY_F17,
		KEY_F18           = GLFW_KEY_F18,
		KEY_F19           = GLFW_KEY_F19,
		KEY_F20           = GLFW_KEY_F20,
		KEY_F21           = GLFW_KEY_F21,
		KEY_F22           = GLFW_KEY_F22,
		KEY_F23           = GLFW_KEY_F23,
		KEY_F24           = GLFW_KEY_F24,
		KEY_F25           = GLFW_KEY_F25,
		KEY_KP_0          = GLFW_KEY_KP_0,
		KEY_KP_1          = GLFW_KEY_KP_1,
		KEY_KP_2          = GLFW_KEY_KP_2,
		KEY_KP_3          = GLFW_KEY_KP_3,
		KEY_KP_4          = GLFW_KEY_KP_4,
		KEY_KP_5          = GLFW_KEY_KP_5,
		KEY_KP_6          = GLFW_KEY_KP_6,
		KEY_KP_7          = GLFW_KEY_KP_7,
		KEY_KP_8          = GLFW_KEY_KP_8,
		KEY_KP_9          = GLFW_KEY_KP_9,
		KEY_KP_DECIMAL    = GLFW_KEY_KP_DECIMAL,
		KEY_KP_DIVIDE     = GLFW_KEY_KP_DIVIDE,
		KEY_KP_MULTIPLY   = GLFW_KEY_KP_MULTIPLY,
		KEY_KP_SUBTRACT   = GLFW_KEY_KP_SUBTRACT,
		KEY_KP_ADD        = GLFW_KEY_KP_ADD,
		KEY_KP_ENTER      = GLFW_KEY_KP_ENTER,
		KEY_KP_EQUAL      = GLFW_KEY_KP_EQUAL,
		KEY_LEFT_SHIFT    = GLFW_KEY_LEFT_SHIFT,
		KEY_LEFT_CONTROL  = GLFW_KEY_LEFT_CONTROL,
		KEY_LEFT_ALT      = GLFW_KEY_LEFT_ALT,
		KEY_LEFT_SUPER    = GLFW_KEY_LEFT_SUPER,
		KEY_RIGHT_SHIFT   = GLFW_KEY_RIGHT_SHIFT,
		KEY_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
		KEY_RIGHT_ALT     = GLFW_KEY_RIGHT_ALT,
		KEY_RIGHT_SUPER   = GLFW_KEY_RIGHT_SUPER,
		KEY_MENU          = GLFW_KEY_MENU,
		KEY_SHIFT         = GLFW_KEY_LEFT_SHIFT,
		KEY_CONTROL       = GLFW_KEY_LEFT_CONTROL,
		KEY_ALT           = GLFW_KEY_LEFT_ALT,
		KEY_SUPER         = GLFW_KEY_LEFT_SUPER,
	};
	enum class MouseButtons {
		MOUSE_1      = GLFW_MOUSE_BUTTON_1,
		MOUSE_2      = GLFW_MOUSE_BUTTON_2,
		MOUSE_3      = GLFW_MOUSE_BUTTON_3,
		MOUSE_4      = GLFW_MOUSE_BUTTON_4,
		MOUSE_5      = GLFW_MOUSE_BUTTON_5,
		MOUSE_6      = GLFW_MOUSE_BUTTON_6,
		MOUSE_7      = GLFW_MOUSE_BUTTON_7,
		MOUSE_8      = GLFW_MOUSE_BUTTON_8,
		MOUSE_LEFT   = GLFW_MOUSE_BUTTON_LEFT,
		MOUSE_RIGHT  = GLFW_MOUSE_BUTTON_RIGHT,
		MOUSE_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
	};

	// Modifier Keys for key and mouse button events
	class ModifierBits {
		friend void key_callback(GLFWwindow*, int32_t, int32_t, int32_t, int32_t);
		friend void mouse_button_callback(GLFWwindow*, int32_t, int32_t, int32_t);
		
		class BitsRef {
			friend ModifierBits;
		private:
			ModifierBits* modifierBits; // ModifierBits object this is referencing
			const ModifierBits* bitMask; // the specific bits in modifierBits that this references
			BitsRef(ModifierBits& modifierBits, const ModifierBits& bitMask) noexcept : modifierBits(&modifierBits), bitMask(&bitMask) {};
			BitsRef(const BitsRef&) = default;
			BitsRef(BitsRef&&) = default;
			BitsRef& operator =(const BitsRef&) = default;
			BitsRef& operator =(BitsRef&&) = default;
		public:
			operator bool() const noexcept { return ((*modifierBits) & (*bitMask)) == (*bitMask); } // implicitly cast to bool
			bool operator =(const bool& rhs) noexcept {
				if (rhs) (*modifierBits) |= (*bitMask); // set all bits from bitMask in modifierBits to true
				else (*modifierBits) &= ~(*bitMask); // set all bits from bitMask in modifierBits to false
				return rhs;
			}
		};
	public:
		static const ModifierBits NONE, SHIFT, CONTROL, ALT, SUPER, CAPS_LOCK, NUM_LOCK;

	private:
		uint8_t bits; // TODO: use a bitfield

		constexpr explicit ModifierBits(uint8_t bits) noexcept : bits(bits & 0b00111111) {}

		constexpr void clearInsignificantBits() noexcept { bits &= 0b00111111; }

	public:
		constexpr ModifierBits() noexcept : bits(0) {};

		constexpr size_t count() const noexcept {
			uint8_t b = bits;
			b = (b & 0b111111) - ((b & 0b101010) >> 1);
			b = (b & 0b110011) + ((b & 0b001100) >> 2);
			return (((b + (b >> 4)) & 0b1111) * 0b0001);
		}

		constexpr inline operator bool() const noexcept { return (bits & 0b00111111) > 0; }

		constexpr ModifierBits operator |(const ModifierBits& rhs) const noexcept { return ModifierBits(bits | rhs.bits); }
		constexpr ModifierBits operator &(const ModifierBits& rhs) const noexcept { return ModifierBits(bits & rhs.bits); }
		constexpr ModifierBits operator ^(const ModifierBits& rhs) const noexcept { return ModifierBits(bits ^ rhs.bits); }
		constexpr ModifierBits operator ~() const noexcept { return ModifierBits(~bits); }
		ModifierBits& operator |=(const ModifierBits& rhs) noexcept {
			bits = (bits | rhs.bits) & 0b00111111;
			return *this;
		}
		ModifierBits& operator &=(const ModifierBits& rhs) noexcept {
			bits = (bits & rhs.bits) & 0b00111111;
			return *this;
		}
		ModifierBits& operator ^=(const ModifierBits& rhs) noexcept {
			bits = (bits ^ rhs.bits) & 0b00111111;
			return *this;
		}
		constexpr bool operator ==(const ModifierBits& rhs) const noexcept { return (bits & 0b00111111) == (rhs.bits & 0b00111111); }
		constexpr bool operator !=(const ModifierBits& rhs) const noexcept { return (bits & 0b00111111) != (rhs.bits & 0b00111111); }
		constexpr bool operator <(const ModifierBits& rhs) const noexcept {
			if ((*this) == rhs) return false;
			const auto countDif = static_cast<int>(count()) - static_cast<int>(rhs.count()); // < 0 if this has fewer active bits than rhs
			if (countDif < 0) return true; // more modifiers -> more specific
			if (countDif == 0) return (bits < rhs.bits); // num_lock > caps_lock > super > alt > control > shift
			return false;
		}
		constexpr bool operator >(const ModifierBits& rhs) const noexcept {
			if ((*this) == rhs) return false;
			const auto countDif = static_cast<int>(count()) - static_cast<int>(rhs.count()); // > 0 if this has more active bits than rhs
			if (countDif > 0) return true; // more modifiers -> more specific
			if (countDif == 0) return (bits > rhs.bits); // num_lock > caps_lock > super > alt > control > shift
			return false;
		}
		constexpr bool operator <=(const ModifierBits& rhs) const noexcept { return ((*this) == rhs) || ((*this) < rhs); }
		constexpr bool operator >=(const ModifierBits& rhs) const noexcept { return ((*this) == rhs) || ((*this) > rhs); }

		BitsRef operator [](const ModifierBits& i) noexcept { return BitsRef(*this, i); }
		constexpr bool operator [](const ModifierBits& i) const noexcept { return (bits & i.bits) == (i.bits); }

		inline constexpr explicit operator uint8_t() const noexcept { return bits & 0b00111111; }
		inline constexpr explicit operator int8_t() const noexcept { return bits & 0b00111111; }
		inline constexpr explicit operator uint16_t() const noexcept { return bits & 0b00111111; }
		inline constexpr explicit operator int16_t() const noexcept { return bits & 0b00111111; }
		inline constexpr explicit operator uint32_t() const noexcept { return bits & 0b00111111; }
		inline constexpr explicit operator int32_t() const noexcept { return bits & 0b00111111; }
		inline constexpr explicit operator uint64_t() const noexcept { return bits & 0b00111111; }
		inline constexpr explicit operator int64_t() const noexcept { return bits & 0b00111111; }

		static inline constexpr ModifierBits fromByte(const uint8_t bits) noexcept { return ModifierBits(bits); }
	};

	std::ostream& operator <<(std::ostream& stream, const ModifierBits& modifierBits);

	inline constexpr const ModifierBits ModifierBits::NONE {};
	inline constexpr const ModifierBits ModifierBits::SHIFT { GLFW_MOD_SHIFT };
	inline constexpr const ModifierBits ModifierBits::CONTROL { GLFW_MOD_CONTROL };
	inline constexpr const ModifierBits ModifierBits::ALT { GLFW_MOD_ALT };
	inline constexpr const ModifierBits ModifierBits::SUPER { GLFW_MOD_SUPER };
	inline constexpr const ModifierBits ModifierBits::CAPS_LOCK { GLFW_MOD_CAPS_LOCK };
	inline constexpr const ModifierBits ModifierBits::NUM_LOCK { GLFW_MOD_NUM_LOCK };

	// a key (named or unnamed) or mouse button
	struct Key {
		KeyType type;
		int code;

		constexpr Key(KeyType type, int code) noexcept : type(type), code(code) {}
		constexpr Key(Keys namedKey) noexcept : type(KeyType::KEY_NAMED), code(static_cast<int>(namedKey)) {}
		constexpr Key(MouseButtons mouseButton) noexcept : type(KeyType::MOUSE), code(static_cast<int>(mouseButton)) {}

		constexpr bool operator ==(const Key& rhs) const noexcept { return (type == rhs.type) && (code == rhs.code); }
		constexpr bool operator !=(const Key& rhs) const noexcept { return (type != rhs.type) || (code != rhs.code); }
	};

	// a key and modifiers
	struct KeyInput {
		Key key;
		ModifierBits modifiers = ModifierBits::NONE;

		constexpr bool operator ==(const KeyInput& rhs) const noexcept { return (key == rhs.key) && (modifiers == rhs.modifiers); }
		constexpr bool operator !=(const KeyInput& rhs) const noexcept { return (key != rhs.key) || (modifiers != rhs.modifiers); }
	};

}

