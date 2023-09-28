#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <functional>
#include <optional>
#include <iostream>
#include <stdexcept>

#include <glm/vec2.hpp>

#include "util/math/math.h"
//#include "util/ct_string.h"

namespace eng {

    class uuid {
        uint64_t ab, cd;

		inline constexpr uuid(uint64_t ab, uint64_t cd) noexcept : ab(ab), cd(cd) {}

	public:

		//inline uuid() : uuid(v4()) {}

		// construct an zeroed-out uuid
		[[nodiscard]] explicit inline uuid() : uuid(0, 0) {}

		// generate a random uuid
		[[nodiscard]] static uuid v4();

		[[nodiscard]] static inline constexpr uuid fromUints(uint64_t ab, uint64_t cd) noexcept {
			return uuid { ab, cd };
		}
		[[nodiscard]] static inline constexpr uuid fromUints(const std::pair<uint64_t, uint64_t>& abcd) noexcept {
			return uuid { abcd.first, abcd.second };
		}
		[[nodiscard]] static inline constexpr uuid fromUints(const glm::u64vec2& abcd) noexcept {
			return uuid { abcd.x, abcd.y };
		}

		[[nodiscard]] inline constexpr std::pair<uint64_t, uint64_t> getUints() const noexcept {
			return std::pair<uint64_t, uint64_t> { ab, cd };
		}

		inline constexpr operator bool() const noexcept { return ab && cd; }

		constexpr bool operator ==(const uuid& rhs) const noexcept {
			return (ab == rhs.ab) && (cd == rhs.cd);
		}
		constexpr bool operator !=(const uuid& rhs) const noexcept {
			return (ab != rhs.ab) || (cd != rhs.cd);
		}

		[[nodiscard]] static constexpr std::optional<uuid> fromString(std::string_view str) noexcept {
            const bool hasDashes = str.size() == 36;
            if ((str.size() != 32) && !hasDashes)
				return std::nullopt;
            uint64_t ab = 0, cd = 0;
            for (size_t i = 0, digit = 0; i < str.size(); i++) {
				const char c = str[i];
				if (hasDashes && ((i == 8) || (i == 13) || (i == 18) || (i == 23))) {
					if (c != '-')
						return std::nullopt;
				} else {
					uint64_t digitVal {};
					if ((c >= '0') && (c <= '9')) {
						digitVal = static_cast<uint64_t>(c - '0');
					} else if ((c >= 'A') && (c <= 'F')) {
						digitVal = static_cast<uint64_t>(10 + (c - 'A'));
					} else if ((c >= 'a') && (c <= 'f')) {
						digitVal = static_cast<uint64_t>(10 + (c - 'a'));
					} else {
						return std::nullopt;
					}
					const uint64_t offset = (0xF - (digit & 0xF)) * 4;
					((digit < 16) ? ab : cd) |= (digitVal << offset);
					digit++;
				}
            }
            return uuid { ab, cd };
        }

        [[nodiscard]] std::string toString() const;

		inline size_t hashCode() const noexcept {
			size_t seed = 0;
			std::hash<uint64_t> hasher;
			hashCombine(seed, hasher(ab));
			hashCombine(seed, hasher(cd));
			return seed;
		}
    };

    std::ostream& operator <<(std::ostream& os, const uuid& id);


	/*template<ct_string uuidStr> requires (uuid::fromString(uuidStr).has_value())
	constexpr uuid make_uuid() noexcept {
		return *uuid::fromString(uuidStr);
	}

	template<ct_string uuidStr>
	constexpr uuid uuid_v = make_uuid<uuidStr>();*/

	/*inline namespace uuid_literals {

		constexpr uuid operator "" _uuid(const char* chars, const size_t length) noexcept {
			const std::string_view str(chars, length);
			const auto uuidParseResult = uuid::fromString(str);
			if (!uuidParseResult)
				throw std::invalid_argument(std::string("Invalid uuid literal ") + std::string(str));
			return *uuidParseResult;
		}

	}*/

}

namespace std {

	template<> struct hash<eng::uuid> {
		inline size_t operator()(const eng::uuid& uuid) const noexcept {
			return uuid.hashCode();
		}
	};

}