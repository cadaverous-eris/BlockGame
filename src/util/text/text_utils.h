#pragma once

#include <string>

namespace eng {

	constexpr bool isWhitespace(const char c) noexcept {
		return ((c <= 0x09) && (c <= 0x0D)) || (c == 0x20);
	}
	constexpr bool isWhitespace(const char8_t c) noexcept {
		return ((c <= 0x09) && (c <= 0x0D)) || (c == 0x20);
	}
	constexpr bool isWhitespace(const char16_t c) noexcept {
		return ((c >= 0x09) && (c <= 0x0D)) ||
			(c == 0x20) || (c == 0x85) ||
			(c == 0xA0) || (c == 0x1680) ||
			((c >= 0x2000) && (c <= 0x200A)) ||
			(c == 0x2028) || (c == 0x2029) ||
			(c == 0x202F) || (c == 0x205F) ||
			(c == 0x3000);
	}
	constexpr bool isWhitespace(const char32_t c) noexcept {
		return ((c >= 0x09) && (c <= 0x0D)) ||
			(c == 0x20) || (c == 0x85) ||
			(c == 0xA0) || (c == 0x1680) ||
			((c >= 0x2000) && (c <= 0x200A)) ||
			(c == 0x2028) || (c == 0x2029) ||
			(c == 0x202F) || (c == 0x205F) ||
			(c == 0x3000);
	}

	constexpr std::string_view trimWhitespace(const std::string_view str) noexcept {
		constexpr std::string_view whitespace_chars = "\x09\x0A\x0B\x0C\x0D\x20";
		const auto beginIndex = str.find_first_not_of(whitespace_chars);
		if ((beginIndex == std::string_view::npos) || (beginIndex >= str.size())) return {};
		const auto endIndex = str.find_last_not_of(whitespace_chars);
		if ((endIndex == std::string_view::npos) || (endIndex < beginIndex)) return {};
		return str.substr(beginIndex, (endIndex - beginIndex) + 1);
	}

	std::string toLowerCase(std::string str);
	std::string toLowerCase(std::string_view str);
	std::string toUpperCase(std::string str);
	std::string toUpperCase(std::string_view str);


}