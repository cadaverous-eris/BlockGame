#pragma once

#include <glm/vec3.hpp>

#include "block/BlockState.h"
#include "fluid/FluidState.h"

namespace eng {

	class ChunkBakeData;

	class MeshingWorldView {
	private:
		const ChunkBakeData* chunkBakeData;

	public:
		MeshingWorldView(ChunkBakeData& chunkBakeData) noexcept;


		// blockPos is in world coordinates
		BlockStateRef getBlockState(const glm::ivec3& blockPos) const noexcept;

		// blockPos is in world coordinates
		FluidStateRef getFluidState(const glm::ivec3& blockPos) const noexcept;

		// get whether the given blockPos can be viewed
		bool containsBlockPos(const glm::ivec3& blockPos) const noexcept;

	};

}