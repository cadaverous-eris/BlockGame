#include "RNG.h"

#include <random>
//#include <bit>

#include <glm/ext/scalar_integer.hpp>

namespace eng {

	static float asFloat(uint32_t i) noexcept {
		union PunFloat {
			uint32_t i;
			float f;
		} const pun = { i };
		return pun.f;
		//return std::bit_cast<float>(i);
	}
	static double asDouble(uint64_t i) noexcept {
		union PunDouble {
			uint64_t i;
			double f;
		} const pun = { i };
		return pun.f;
		//return std::bit_cast<double>(i);
	}

	// FNV-1a hashing http://www.isthe.com/chongo/tech/comp/fnv/
	static constexpr RNG::seed_t fnv1a(const unsigned char* str, const size_t size) noexcept {
		constexpr RNG::seed_t base_hash = 14695981039346656037ull;
		constexpr RNG::seed_t hash_multiplier = 1099511628211ull;
		RNG::seed_t hash = base_hash;
		for (size_t i = 0; i < size; i++)
			hash = (hash ^ str[i]) * hash_multiplier;
		return hash >> (64 - RNG::seed_size_bits);
	}

	// custom string hashing to ensure that the results will be the same between 32-bit and 64-bit systems
	template<typename CharT, typename Traits>
	static constexpr RNG::seed_t stringHash(std::basic_string_view<CharT, Traits> str) noexcept {
		const auto strPtr = reinterpret_cast<const unsigned char*>(str.data());
		const auto size = str.size() * sizeof(CharT);
		return fnv1a(strPtr, size);
	}

	uint32_t RNG::seededUint32(seed_t seed) noexcept {
		seed = (seed * multiplier + increment) & (modulus - 1);
		return static_cast<uint32_t>(seed >> (seed_size_bits - 32));
	}
	uint64_t RNG::seededUint64(seed_t seed) noexcept {
		uint64_t r;
		seed = (seed * multiplier + increment) & (modulus - 1);
		r = (seed << (64 - seed_size_bits)) & 0xFFFFFFFF00000000ull;
		seed = (seed * multiplier + increment) & (modulus - 1);
		return r | ((seed >> (seed_size_bits - 32)) & 0x00000000FFFFFFFFull);
	}

	uint32_t RNG::nextUint32() noexcept {
		seed = (seed * multiplier + increment) & (modulus - 1);
		return static_cast<uint32_t>(seed >> (seed_size_bits - 32));
	}
	uint64_t RNG::nextUint64() noexcept {
		return (static_cast<uint64_t>(nextUint32()) << 32) | static_cast<uint64_t>(nextUint32());
	}

	int32_t RNG::nextInt32() noexcept {
		return static_cast<int32_t>(nextUint32() & 0x7FFFFFFFu);
	}
	int64_t RNG::nextInt64() noexcept {
		return (static_cast<int64_t>(nextInt32()) << 32) | static_cast<int64_t>(nextUint32());
	}

	uint32_t RNG::nextUint32(uint32_t bound) {
		if (bound == 0) return nextUint32();
		if (bound == 1) return 0;
		if (glm::isPowerOfTwo(bound)) {
			return nextUint32() & (bound - 1);
		}
		constexpr uint32_t maxRand = std::numeric_limits<uint32_t>::max();
		const auto discardRange = maxRand % bound;
		uint32_t r;
		do {
			r = nextUint32();
		} while (r > (maxRand - discardRange));
		return r % bound;
	}
	uint64_t RNG::nextUint64(uint64_t bound) { // uniform distribution
		if (bound == 0) return nextUint64();
		if (bound == 1) return 0;
		if (glm::isPowerOfTwo(bound)) {
			return nextUint64() & (bound - 1);
		}
		constexpr uint64_t maxRand = std::numeric_limits<uint64_t>::max();
		const auto discardRange = maxRand % bound;
		uint64_t r;
		do {
			r = nextUint64();
		} while (r > (maxRand - discardRange));
		return r % bound;
	}

	uint32_t RNG::nextUint32(uint32_t min, uint32_t bound) {
		if ((bound <= min) || (bound - min == 1)) return min;
		return nextUint32(bound - min) + min;
	}
	uint64_t RNG::nextUint64(uint64_t min, uint64_t bound) {
		if ((bound <= min) || (bound - min == 1)) return min;
		return nextUint64(bound - min) + min;
	}

	float RNG::nextFloat() noexcept {
		return asFloat(0x3F800000u | (nextUint32() >> 9)) - 1.0f;
	}
	double RNG::nextDouble() noexcept {
		return asDouble(0x3FF0000000000000ull | (nextUint64() >> 12)) - 1.0;
	}

	RNG::seed_t RNG::randomSeed() {
		std::random_device r {};
		return (static_cast<RNG::seed_t>(r()) << 32) | static_cast<RNG::seed_t>(r());
	}

	RNG::seed_t RNG::toSeed(std::string_view seedStr) noexcept {
		return stringHash(seedStr);
	}
	RNG::seed_t RNG::toSeed(std::u8string_view seedStr) noexcept {
		return stringHash(seedStr);
	}

}