#include "key_input.h"

namespace eng::input {

	std::ostream& operator <<(std::ostream& stream, const ModifierBits& modifierBits) {
		const bool shift = modifierBits[ModifierBits::SHIFT];
		const bool control = modifierBits[ModifierBits::CONTROL];
		const bool alt = modifierBits[ModifierBits::ALT];
		const bool super = modifierBits[ModifierBits::SUPER];
		const bool capsLock = modifierBits[ModifierBits::CAPS_LOCK];
		const bool numLock = modifierBits[ModifierBits::NUM_LOCK];
		stream << (shift ? 1 : 0);
		stream << (control ? 1 : 0);
		stream << (alt ? 1 : 0);
		stream << (super ? 1 : 0);
		stream << (capsLock ? 1 : 0);
		stream << (numLock ? 1 : 0);
		bool firstKeyName = true;
		if (shift) { stream << (firstKeyName ? " " : " + ") << "Shift";     firstKeyName = false; }
		if (control) { stream << (firstKeyName ? " " : " + ") << "Control";   firstKeyName = false; }
		if (alt) { stream << (firstKeyName ? " " : " + ") << "Alt";       firstKeyName = false; }
		if (super) { stream << (firstKeyName ? " " : " + ") << "Super";     firstKeyName = false; }
		if (capsLock) { stream << (firstKeyName ? " " : " + ") << "Caps Lock"; firstKeyName = false; }
		if (numLock) { stream << (firstKeyName ? " " : " + ") << "Num Lock";  /*firstKeyName = false;*/ }
		return stream;
	}

}