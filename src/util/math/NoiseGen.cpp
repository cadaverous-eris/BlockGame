#include "NoiseGen.h"

#include <array>
#include <limits>
#include <type_traits>

#include <glm/common.hpp>
#include <glm/geometric.hpp>

namespace eng {

	template<typename T>
	static constexpr std::array<glm::dvec2, 8> vecPool2d {
		glm::vec<2, T> {  0.9238795042037964,   0.3826834559440613  },
		glm::vec<2, T> {  0.3826834261417389,   0.9238795042037964  },
		glm::vec<2, T> { -0.3826833963394165,   0.9238795638084412  },
		glm::vec<2, T> { -0.9238795042037964,   0.38268348574638367 },
		glm::vec<2, T> { -0.9238795042037964,  -0.3826834261417389  },
		glm::vec<2, T> { -0.38268357515335083, -0.9238795042037964  },
		glm::vec<2, T> {  0.3826836049556732,  -0.9238794445991516  },
		glm::vec<2, T> {  0.9238795638084412,  -0.3826834261417389  },
	};
	template<typename T>
	static constexpr std::array<glm::dvec3, 12> vecPool3d {
		glm::vec<3, T> {  1.0,  1.0,  0.0 },
		glm::vec<3, T> { -1.0,  1.0,  0.0 },
		glm::vec<3, T> {  1.0, -1.0,  0.0 },
		glm::vec<3, T> { -1.0, -1.0,  0.0 },
		glm::vec<3, T> {  1.0,  0.0,  1.0 },
		glm::vec<3, T> { -1.0,  0.0,  1.0 },
		glm::vec<3, T> {  1.0,  0.0, -1.0 },
		glm::vec<3, T> { -1.0,  0.0, -1.0 },
		glm::vec<3, T> {  0.0,  1.0,  1.0 },
		glm::vec<3, T> {  0.0, -1.0,  1.0 },
		glm::vec<3, T> {  0.0,  1.0, -1.0 },
		glm::vec<3, T> {  0.0, -1.0, -1.0 },
	};

	NoiseGen::NoiseGen(const RNG::seed_t seed, const NoiseParams& params) noexcept :
			defaultParams(params), seed(seed) {}


	template<typename T>
	static inline typename std::enable_if_t<std::is_arithmetic_v<T> && std::is_signed_v<T>, T> signum(const T val) noexcept {
		return static_cast<T>(T(0) < val) - static_cast<T>(val < T(0));
	}
	template<typename T>
	static inline typename std::enable_if_t<std::is_arithmetic_v<T> && std::is_unsigned_v<T>, T> signum(const T val) noexcept {
		return static_cast<T>(T(0) < val);
	}

	static RNG::seed_t hash(const RNG::seed_t seed, const glm::ivec2& pos) noexcept {
		RNG::seed_t hash = 0;
		hash ^= seed + (0x9e3779b9 + (hash << 6) + (hash >> 2));
		hash ^= pos.x + (0x9e3779b9 + (hash << 6) + (hash >> 2));
		hash ^= pos.y + (0x9e3779b9 + (hash << 6) + (hash >> 2));
		return hash;
		/*uint32_t hash = 80238287u;
		hash = (hash << 4) ^ (hash >> 28) ^ ((static_cast<uint32_t>(seed >> 32) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ ((static_cast<uint32_t>(seed) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ (((signum(pos.y) * 512 + 512) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ (((signum(pos.x) * 512 + 512) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ ((pos.x * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ ((pos.y * 5449) % 130651);
		return hash % 75327403u;*/
	}
	static RNG::seed_t hash(const RNG::seed_t seed, const glm::ivec3& pos) noexcept {
		RNG::seed_t hash = 0;
		hash ^= seed + (0x9e3779b9 + (hash << 6) + (hash >> 2));
		hash ^= pos.x + (0x9e3779b9 + (hash << 6) + (hash >> 2));
		hash ^= pos.y + (0x9e3779b9 + (hash << 6) + (hash >> 2));
		hash ^= pos.z + (0x9e3779b9 + (hash << 6) + (hash >> 2));
		return hash;
		/*uint32_t hash = 80238287u;
		hash = (hash << 4) ^ (hash >> 28) ^ ((static_cast<uint32_t>(seed >> 32) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ ((static_cast<uint32_t>(seed) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ (((signum(pos.z) * 512 + 512) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ (((signum(pos.y) * 512 + 512) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ (((signum(pos.x) * 512 + 512) * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ ((pos.x * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ ((pos.y * 5449) % 130651);
		hash = (hash << 4) ^ (hash >> 28) ^ ((pos.z * 5449) % 130651);
		return hash % 75327403u;*/
	}
	
	template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, int> = 0>
	static inline T smoothstep(const T t) noexcept { // 6t^5 - 15t^4 + 10t^3
		return t * t * t * ((t * ((t * T{6}) - T{15})) + T{10});
	}

	template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, int> = 0>
	static inline glm::vec<2, T> getGradient(const glm::ivec2& pos, const RNG::seed_t seed) {
		return vecPool2d<T>[RNG(hash(seed, pos)).nextUint32(vecPool2d<T>.size())];
	}
	template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, int> = 0>
	static inline glm::vec<3, T> getGradient(const glm::ivec3& pos, const RNG::seed_t seed) {
		return vecPool3d<T>[RNG(hash(seed, pos)).nextUint32(vecPool3d<T>.size())];
	}

	template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, int> = 0>
	static T getOctave(const glm::ivec2& pos, const T frequency, const RNG::seed_t seed) {
		using tvec2 = glm::vec<2, T>;
		using glm::ivec2;
		const tvec2 s { static_cast<tvec2>(pos) / frequency }; // scaled position
		const ivec2 p { glm::floor(s) }; // floor(scaled position)
		const tvec2 f { glm::fract(s) }; // fract(scaled position)
		const T x1 = glm::mix(
			(glm::dot(getGradient<T>(p, seed), f) + T{1}) / T{2},
			(glm::dot(getGradient<T>(p + ivec2(1, 0), seed), f - tvec2(1, 0)) + T{1}) / T{2},
			smoothstep(f.x)
		);
		const T x2 = glm::mix(
			(glm::dot(getGradient<T>(p + ivec2(0, 1), seed), f - tvec2(0, 1)) + T{1}) / T{2},
			(glm::dot(getGradient<T>(p + ivec2(1, 1), seed), f - tvec2(1, 1)) + T{1}) / T{2},
			smoothstep(f.x)
		);
		return glm::mix(x1, x2, smoothstep(f.y));
	}
	template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, int> = 0>
	static T getOctave(const glm::ivec3& pos, const T frequency, const RNG::seed_t seed) {
		using tvec3 = glm::vec<3, T>;
		using glm::ivec3;
		const tvec3 s { static_cast<tvec3>(pos) / frequency }; // scaled position
		const ivec3 p { glm::floor(s) }; // floor(scaled position)
		const tvec3 f { glm::fract(s) }; // fract(scaled position)
		const T x1 = glm::mix(
			(glm::dot(getGradient<T>(p, seed), f) + T{1}) / T{2},
			(glm::dot(getGradient<T>(p + ivec3(1, 0, 0), seed), f - tvec3(1, 0, 0)) + T{1}) / T{2},
			smoothstep(f.x)
		);
		const T x2 = glm::mix(
			(glm::dot(getGradient<T>(p + ivec3(0, 1, 0), seed), f - tvec3(0, 1, 0)) + T{1}) / T{2},
			(glm::dot(getGradient<T>(p + ivec3(1, 1, 0), seed), f - tvec3(1, 1, 0)) + T{1}) / T{2},
			smoothstep(f.x)
		);
		const T x3 = glm::mix(
			(glm::dot(getGradient<T>(p + ivec3(0, 0, 1), seed), f - tvec3(0, 0, 1)) + T{1}) / T{2},
			(glm::dot(getGradient<T>(p + ivec3(1, 0, 1), seed), f - tvec3(1, 0, 1)) + T{1}) / T{2},
			smoothstep(f.x)
		);
		const T x4 = glm::mix(
			(glm::dot(getGradient<T>(p + ivec3(0, 1, 1), seed), f - tvec3(0, 1, 1)) + T{1}) / T{2},
			(glm::dot(getGradient<T>(p + ivec3(1, 1, 1), seed), f - tvec3(1, 1, 1)) + T{1}) / T{2},
			smoothstep(f.x)
		);
		const T y1 = glm::mix(x1, x2, smoothstep(f.y));
		const T y2 = glm::mix(x3, x4, smoothstep(f.y));
		return glm::mix(y1, y2, smoothstep(f.z));
	}

	double NoiseGen::getNoise(const glm::ivec2& pos, const RNG::seed_t seed, const NoiseParams& params, const glm::ivec2& period) {
		double max = 0;
		double noise = 0;
		double freq = params.frequency;
		double amp = params.amplitude;
		const glm::ivec2 p = (period != glm::ivec2(0, 0)) ? (((pos % period) + period) % period) : pos;
		for (int i = 0; i < params.octaves; i++) {
			noise += getOctave(p, freq, seed) * amp;
			max += amp;
			amp *= params.persistence;
			freq *= 0.5;
		}
		return noise / max;
	}
	double NoiseGen::getNoise(const glm::ivec3& pos, const RNG::seed_t seed, const NoiseParams& params, const glm::ivec3& period) {
		double max = 0;
		double noise = 0;
		double freq = params.frequency;
		double amp = params.amplitude;
		const glm::ivec3 p = (period != glm::ivec3(0, 0, 0)) ? (((pos % period) + period) % period) : pos;
		for (int i = 0; i < params.octaves; i++) {
			noise += getOctave(p, freq, seed) * amp;
			max += amp;
			amp *= params.persistence;
			freq *= 0.5;
		}
		return noise / max;
	}

}