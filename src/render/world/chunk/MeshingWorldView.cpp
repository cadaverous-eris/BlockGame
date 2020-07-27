#include "MeshingWorldView.h"

#include "block/BlockRegistry.h"
#include "fluid/FluidRegistry.h"
#include "ChunkBakeData.h"

namespace eng {

	MeshingWorldView::MeshingWorldView(ChunkBakeData& chunkBakeData) noexcept : chunkBakeData(&chunkBakeData) {}

	BlockStateRef MeshingWorldView::getBlockState(const glm::ivec3& blockPos) const noexcept {
		if (containsBlockPos(blockPos)) {
			return chunkBakeData->getBlockState(blockPos - chunkBakeData->getBlockPos());
		}
		return blocks::empty_blockstate;
	}

	FluidStateRef MeshingWorldView::getFluidState(const glm::ivec3& blockPos) const noexcept {
		if (containsBlockPos(blockPos)) {
			return chunkBakeData->getFluidState(blockPos - chunkBakeData->getBlockPos());
		}
		return fluids::empty_fluidstate;
	}

	bool MeshingWorldView::containsBlockPos(const glm::ivec3& blockPos) const noexcept {
		return (
			(blockPos.x >= (chunkBakeData->getBlockPos().x - static_cast<int>(ChunkBakeData::PADDING))) &&
			(blockPos.x < (chunkBakeData->getBlockPos().x + static_cast<int>(ChunkBakeData::WIDTH - ChunkBakeData::PADDING))) &&
			(blockPos.y >= (chunkBakeData->getBlockPos().y - static_cast<int>(ChunkBakeData::PADDING))) &&
			(blockPos.y < (chunkBakeData->getBlockPos().y + static_cast<int>(ChunkBakeData::WIDTH - ChunkBakeData::PADDING))) &&
			(blockPos.z >= (chunkBakeData->getBlockPos().z - static_cast<int>(ChunkBakeData::PADDING))) &&
			(blockPos.z < (chunkBakeData->getBlockPos().z + static_cast<int>(ChunkBakeData::WIDTH - ChunkBakeData::PADDING)))
		);
	}


}