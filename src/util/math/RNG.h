#pragma once

#include <cstdint>
#include <limits>
#include <string>

/*
 *  Custom random number generator to ensure consistency across platforms
 *  https://experilous.com/1/blog/post/perfect-fast-random-floating-point-numbers
 *  http://www.azillionmonkeys.com/qed/random.html
 */

namespace eng {

	class RNG {
	public:
		using result_type = uint32_t;
		using seed_t = uint64_t;

		static constexpr size_t seed_size_bits = 48; // use 48 bit seeds
	private:
		static constexpr seed_t multiplier = 25214903917ull;
		static constexpr seed_t increment = 11;
		static constexpr seed_t modulus = (2ull << seed_size_bits);

		seed_t seed;

	public:
		[[nodiscard]] constexpr RNG(const seed_t seed) noexcept : seed(seed) {}
		[[nodiscard]] RNG(std::string_view seed) noexcept : seed(toSeed(seed)) {}
		[[nodiscard]] RNG() : seed(randomSeed()) {}

		void setSeed(const seed_t seed) noexcept {
			this->seed = seed;
		}

		[[nodiscard]] static constexpr result_type min() {
			return std::numeric_limits<result_type>::lowest();
		}
		[[nodiscard]] static constexpr result_type max() {
			return std::numeric_limits<result_type>::max();
		}

		[[nodiscard]] static uint32_t seededUint32(seed_t seed) noexcept;
		[[nodiscard]] static uint64_t seededUint64(seed_t seed) noexcept;

		inline result_type operator ()() noexcept {
			return nextUint32();
		}

		uint32_t nextUint32() noexcept;
		uint64_t nextUint64() noexcept;

		// returns a random unsigned integer between 0 (inclusive) and bound (exclusive)
		uint32_t nextUint32(uint32_t bound);
		// returns a random unsigned integer between 0 (inclusive) and bound (exclusive)
		uint64_t nextUint64(uint64_t bound);

		// returns a random float between 0 (inclusive) and 1 (exclusive)
		float nextFloat() noexcept;
		// returns a random double between 0 (inclusive) and 1 (exclusive)
		double nextDouble() noexcept;

		// generate a random seed
		[[nodiscard]] static seed_t randomSeed();

		// convert a string into a seed
		[[nodiscard]] static seed_t toSeed(std::string_view seedStr) noexcept;

	private:



	};

}