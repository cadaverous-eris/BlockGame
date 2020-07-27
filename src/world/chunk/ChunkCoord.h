#pragma once

#include <functional>

#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>

#include "util/direction.h"
#include "util/math/math.h"

namespace eng {

	struct ChunkCoord : glm::ivec3 {
		static ChunkCoord fromBlockPos(const glm::ivec3& blockPos) noexcept;
		constexpr ChunkCoord() noexcept : glm::ivec3(0, 0, 0) {}
		constexpr ChunkCoord(const int x, const int y, const int z) noexcept : glm::ivec3(x, y, z) {}
		constexpr explicit ChunkCoord(const glm::ivec3& vec) noexcept : glm::ivec3(vec) {}
		// returns the block position of the chunk coordinate
		static glm::ivec3 toBlockPos(const ChunkCoord& chunkCoord) noexcept;
		// returns the block position of the chunk coordinate
		glm::ivec3 getBlockPos() const noexcept;
		// returns a block position relative to the chunk coordinate
		// (subtracts the block position of this chunk coordinate from the provided block position)
		glm::ivec3 getRelativeBlockPos(const glm::ivec3& blockPos) const noexcept;
		ChunkCoord offset(const Direction) const noexcept;
	};

}

namespace std {
	// ChunkCoord hasher
	template<> struct hash<eng::ChunkCoord> : hash<glm::ivec3> {};
}