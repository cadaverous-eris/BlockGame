#include "unicode_utils.h"

namespace eng {

	namespace unicode {

		std::u32string utf8ToUtf32(std::string_view str8) {
			std::u32string str32;
			str32.reserve(str8.length());
			char32_t codepoint;
			Utf8DecoderState state = Utf8DecoderState::accept, prevState = Utf8DecoderState::accept;
			for (auto it = str8.begin(); it != str8.end(); it++) {
				switch (decodeUtf8(state, codepoint, *it)) {
				case Utf8DecoderState::accept: // valid codepoint fully decoded
					str32.push_back(codepoint);
					break;
				case Utf8DecoderState::reject: // invalid byte
					str32.push_back(replacement_character);
					state = Utf8DecoderState::accept;
					if (prevState != Utf8DecoderState::accept) it--;
					break;
				}
				prevState = state;
			}
			return str32;
		}
		std::u32string utf8ToUtf32(std::u8string_view str8) {
			return utf8ToUtf32(reinterpret_cast<const char*>(str8.data()));
		}


		// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
		// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
		static constexpr std::array<uint8_t, 364> utf8d {
				// The first part of the table maps bytes to character classes that
				// to reduce the size of the transition table and create bitmasks.
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
				10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

				// The second part is a transition table that maps a combination
				// of a state of the automaton and a character class to a state.
				 0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
				12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
				12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
				12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
				12,36,12,12,12,12,12,12,12,12,12,12,
		};
		constexpr Utf8DecoderState decodeUtf8(Utf8DecoderState& state, char32_t& codepoint, unsigned char byte) noexcept {
			char32_t type = utf8d[byte];
			codepoint = (state != Utf8DecoderState::accept) ?
				(static_cast<char32_t>(byte) & 0x3Fu) | (codepoint << 6) :
				(0xFF >> type) & static_cast<char32_t>(byte);
			state = Utf8DecoderState{ utf8d[256 + static_cast<char32_t>(state) + type] };
			return state;
		}

	}

}