#include "ChunkCoord.h"

#include "Chunk.h"

namespace eng {

	ChunkCoord ChunkCoord::fromBlockPos(const glm::ivec3& blockPos) noexcept {
		return ChunkCoord(blockPos.x >> Chunk::LOG2_WIDTH, blockPos.y >> Chunk::LOG2_WIDTH, blockPos.z >> Chunk::LOG2_WIDTH);
	}

	// returns the block position of the chunk coordinate
	glm::ivec3 ChunkCoord::toBlockPos(const ChunkCoord& chunkCoord) noexcept {
		return chunkCoord.getBlockPos();
	}

	// returns the block position of the chunk coordinate
	glm::ivec3 ChunkCoord::getBlockPos() const noexcept {
		return { x << Chunk::LOG2_WIDTH, y << Chunk::LOG2_WIDTH, z << Chunk::LOG2_WIDTH };
	}

	// returns a block position relative to the chunk coordinate
	// (subtracts the block position of this chunk coordinate from the provided block position)
	glm::ivec3 ChunkCoord::getRelativeBlockPos(const glm::ivec3& blockPos) const noexcept {
		return { blockPos.x - (x << Chunk::LOG2_WIDTH), blockPos.y - (y << Chunk::LOG2_WIDTH), blockPos.z - (z << Chunk::LOG2_WIDTH) };
	}

	ChunkCoord ChunkCoord::offset(const Direction d) const noexcept {
		const glm::ivec3 offset = direction::toVector<glm::ivec3>(d);
		return { x + offset.x, y + offset.y, z + offset.z };
	}

}