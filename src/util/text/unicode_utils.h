#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

namespace eng {

	namespace unicode {

		// unicode replacement character
		// displayed instead of characters that are missing from the display font(s)
		// replaces invalid bytes/codepoints when converting between character encodings
		inline constexpr char32_t replacement_character { 0xFFFD };
		// unicode replacement character (as utf8 bytes)
		// displayed instead of characters that are missing from the display font(s)
		// replaces invalid bytes/codepoints when converting between character encodings
		template<typename C = char>
		inline constexpr std::array<C, 3> replacement_character_utf8_bytes = { static_cast<C>(0xEF), static_cast<C>(0xBF), static_cast<C>(0xBD) }; // TODO: change to std::basic_string once it is constexpr_compliant
		

		enum class Utf8DecoderState : char32_t { accept = 0, reject = 12, };
		constexpr Utf8DecoderState decodeUtf8(Utf8DecoderState& state, char32_t& codepoint, unsigned char byte) noexcept;


		std::u32string utf8ToUtf32(std::string_view str8);
		std::u32string utf8ToUtf32(std::u8string_view str8);

		template <typename String8 = std::string>
		String8& appendUtf32CharToUtf8(String8& str8, const char32_t codepoint) {
			using char8_type = typename String8::value_type;
			using init_list_type = std::initializer_list<char8_type>;
			if (codepoint <= 0x7F) { // need 1 byte
				str8.append(1, static_cast<char8_type>(codepoint));
			} else if (codepoint <= 0x7FF) { // need 3 bytes
				str8.append(init_list_type {
					static_cast<char8_type>(0xC0 | (codepoint >> 6)),            /* 110xxxxx */
					static_cast<char8_type>(0x80 | (codepoint & 0x3F)),          /* 10xxxxxx */
				});
			} else if (codepoint <= 0xFFFF) { // need 3 bytes
				str8.append(init_list_type {
					static_cast<char8_type>(0xE0 | (codepoint >> 12)),           /* 1110xxxx */
					static_cast<char8_type>(0x80 | ((codepoint >> 6) & 0x3F)),   /* 10xxxxxx */
					static_cast<char8_type>(0x80 | (codepoint & 0x3F)),          /* 10xxxxxx */
				});
			} else if (codepoint <= 0x10FFFF) { // need 4 bytes
				str8.append(init_list_type {
					static_cast<char8_type>(0xF0 | (codepoint >> 18)),           /* 11110xxx */
					static_cast<char8_type>(0x80 | ((codepoint >> 12) & 0x3F)),  /* 10xxxxxx */
					static_cast<char8_type>(0x80 | ((codepoint >> 6) & 0x3F)),   /* 10xxxxxx */
					static_cast<char8_type>(0x80 | (codepoint & 0x3F)),          /* 10xxxxxx */
				});
			} else { // out of unicode range, use replacement character
				str8.append(replacement_character_utf8_bytes<char8_type>.begin(),
							replacement_character_utf8_bytes<char8_type>.end());
			}
			return str8;
		}

		template <typename String8 = std::string>
		String8 utf32ToUtf8(std::u32string_view str32) {
			String8 str8;
			str8.reserve(str32.length());
			for (const auto& codepoint : str32)
				appendUtf32CharToUtf8(str8, codepoint);
			return str8;
		}

	}

}