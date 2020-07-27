#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "RNG.h"

namespace eng {

	class NoiseGen {
	public:
		struct NoiseParams {
			double frequency { 83.0 };
			double amplitude { 1.0 };
			double persistence { 0.4 };
			int octaves { 4 };
		};
	private:
		NoiseParams defaultParams;
		glm::ivec2 period2d { 0, 0 };
		glm::ivec3 period3d { 0, 0, 0 };
		
		RNG::seed_t seed;

	public:
		[[nodiscard]] NoiseGen(RNG::seed_t seed, const NoiseParams& params) noexcept;

		NoiseGen& setPeriod2D(const glm::ivec2& period) noexcept {
			period2d = period;
			return *this;
		}
		NoiseGen& setPeriod3D(const glm::ivec3& period) noexcept {
			period3d = period;
			return *this;
		}

		[[nodiscard]] inline double getNoise(const glm::ivec2& pos, double freq, double amp, double persist) const {
			return getNoise(pos, seed, { freq, amp, persist, defaultParams.octaves }, period2d);
		}
		[[nodiscard]] inline double getNoise(const glm::ivec2& pos) const {
			return getNoise(pos, seed, defaultParams, period2d);
		}
		[[nodiscard]] inline double getNoise(const glm::ivec3& pos, double freq, double amp, double persist) const {
			return getNoise(pos, seed, { freq, amp, persist, defaultParams.octaves }, period3d);
		}
		[[nodiscard]] inline double getNoise(const glm::ivec3& pos) const {
			return getNoise(pos, seed, defaultParams, period3d);
		}

		[[nodiscard]] static double getNoise(const glm::ivec2& pos, RNG::seed_t seed, const NoiseParams& params, const glm::ivec2& period = { 0, 0 });
		[[nodiscard]] static double getNoise(const glm::ivec3& pos, RNG::seed_t seed, const NoiseParams& params, const glm::ivec3& period = { 0, 0, 0 });

	};
	
}
