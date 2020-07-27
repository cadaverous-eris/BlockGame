#include "BlockLeaves.h"

#include "util/math/NoiseGen.h"

namespace eng {

	static NoiseGen colorNoise1 { RNG::randomSeed(), { 43.0, 1.0, 1.0, 1 } };
	static NoiseGen colorNoise2 { RNG::randomSeed(), { 33.0, 1.0, 1.0, 1 } };
	static NoiseGen colorNoise3 { RNG::randomSeed(), { 23.0, 1.0, 1.0, 1 } };

	Color getLeavesColor(const glm::ivec3& blockPos) {
		return Color::hsl {
			static_cast<float>(colorNoise1.getNoise(blockPos)) * 360.0f,
			glm::mix(0.2f, 0.65f, static_cast<float>(colorNoise2.getNoise(blockPos))),
			glm::mix(0.4f, 0.8f, static_cast<float>(colorNoise3.getNoise(blockPos))),
		};
	}

}