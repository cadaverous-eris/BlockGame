#include "Color.h"


#include "util/text/text_utils.h" // for toLowerCase() function

namespace eng {

	static std::string hexByteToString(const uint8_t hexByte, const bool uppercase) {
		const uint8_t d0 = (hexByte >> 4) & 0xF;
		const uint8_t d1 = hexByte & 0xF;
		const char c0 = d0 + ((d0 < 0xA) ? 48 : (uppercase ? 55 : 87));
		const char c1 = d1 + ((d1 < 0xA) ? 48 : (uppercase ? 55 : 87));
		return { c0, c1 };
	}
	static std::string& appendHexByteToString(std::string& str, const uint8_t hexByte, const bool uppercase = true) {
		const uint8_t d0 = (hexByte >> 4) & 0xF;
		const uint8_t d1 = hexByte & 0xF;
		const char c0 = d0 + ((d0 < 0xA) ? '0' : ((uppercase ? 'A' : 'a') - 0xA));
		const char c1 = d1 + ((d1 < 0xA) ? '0' : ((uppercase ? 'A' : 'a') - 0xA));
		str += c0;
		str += c1;
		return str;
	}

	std::string Color::toString(std::string_view hexPrefix) const {
		std::string str(hexPrefix);
		appendHexByteToString(str, r);
		appendHexByteToString(str, g);
		appendHexByteToString(str, b);
		if (a != 0xFF) appendHexByteToString(str, a);
		return str;
	}

	std::ostream& operator <<(std::ostream& os, const Color& c) {
		const auto flags = os.flags();
		const bool up = (flags & os.uppercase);
		os << "#" << hexByteToString(c.r, up) << hexByteToString(c.g, up) << hexByteToString(c.b, up);
		if (c.a != 0xFF) os << hexByteToString(c.a, up);
		return os;
	}


	Color Color::fromString(std::string_view str, const std::string_view hexPrefix) {
		using namespace std::string_view_literals;
		constexpr auto getHexDigitValue = [](const char hd) constexpr noexcept -> uint8_t {
			if (hd >= '0' && hd <= '9') return hd - '0';
			if (hd >= 'A' && hd <= 'F') return (hd - 'A') + 0xA;
			if (hd >= 'a' && hd <= 'f') return (hd - 'a') + 0xA;
			return 0;
		};
		constexpr auto getHexByteValue1 = [getHexDigitValue](const char d1) constexpr noexcept -> uint8_t {
			const float f = glm::clamp(getHexDigitValue(d1) / 15.0f, 0.0f, 1.0f);
			return static_cast<uint8_t>(f * 0xFF);
		};
		constexpr auto getHexByteValue2 = [getHexDigitValue](const char d1, const char d2) constexpr noexcept -> uint8_t {
			return ((getHexDigitValue(d1) & 0x0F) << 4) | (getHexDigitValue(d2) & 0x0F);
		};
		if (str.starts_with(hexPrefix)) {
			const auto hexLength = str.length() - hexPrefix.length();
			const auto hexStart = hexPrefix.length();
			constexpr auto hexChars = "0123456789ABCDEFabcdef"sv;
			if (((hexLength == 3) || (hexLength == 4) || (hexLength == 6) || (hexLength == 8)) &&
				(str.find_first_not_of(hexChars, hexPrefix.length()) == std::string_view::npos)) {
				if (hexLength == 3) {
					return { getHexByteValue1(str[hexStart + 0]),
							 getHexByteValue1(str[hexStart + 1]),
							 getHexByteValue1(str[hexStart + 2]), };
				} else if (hexLength == 4) {
					return { getHexByteValue1(str[hexStart + 0]),
							 getHexByteValue1(str[hexStart + 1]),
							 getHexByteValue1(str[hexStart + 2]),
							 getHexByteValue1(str[hexStart + 3]), };
				} else if (hexLength == 6) {
					return { getHexByteValue2(str[hexStart + 0], str[hexStart + 1]),
							 getHexByteValue2(str[hexStart + 2], str[hexStart + 3]),
							 getHexByteValue2(str[hexStart + 4], str[hexStart + 5]), };
				} else if (hexLength == 8) {
					return { getHexByteValue2(str[hexStart + 0], str[hexStart + 1]),
							 getHexByteValue2(str[hexStart + 2], str[hexStart + 3]),
							 getHexByteValue2(str[hexStart + 4], str[hexStart + 5]),
							 getHexByteValue2(str[hexStart + 6], str[hexStart + 7]), };
				}
			}
		}
		if (const auto it = color::color_values.find(toLowerCase(str)); it != color::color_values.end()) {
			return it->second;
		}
		return { 0x00, 0x00, 0x00, 0xFF };
	}
	bool Color::isValidColorString(std::string_view str, const std::string_view hexPrefix) {
		using namespace std::string_view_literals;
		if (str.starts_with(hexPrefix)) {
			const auto hexLength = str.length() - hexPrefix.length();
			if ((hexLength != 3) && (hexLength != 4) && (hexLength != 6) && (hexLength != 8))
				return false;
			constexpr auto hexChars = "0123456789ABCDEFabcdef"sv;
			// return true if the string only contains hex digits after the prefix
			return (str.find_first_not_of(hexChars, hexPrefix.length()) == std::string_view::npos);
		}
		const auto it = color::color_values.find(toLowerCase(str));
		return it != color::color_values.end();
	}

}