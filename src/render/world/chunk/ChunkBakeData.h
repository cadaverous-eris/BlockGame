#pragma once

#include <memory>

#include <glm/vec3.hpp>

#include "world/chunk/Chunk.h"
#include "world/chunk/ChunkData.h"
#include "render/world/chunk/RenderChunk.h"

namespace eng {

	/*
	 * Holds data used to create a mesh for a chunk:
	 * - all blockstates & fluidstates within the bounding box of the chunk, expanded 2 blocks in each direction
	 * - any extended blockstate data // TODO: implement
	 * - position of the chunk
	 */
	class ChunkBakeData {
	public:
		static constexpr size_t PADDING = 2;
		static constexpr size_t WIDTH = Chunk::WIDTH + (PADDING * 2);
		static constexpr size_t LAYER_SIZE = WIDTH * WIDTH;
		static constexpr size_t SIZE = LAYER_SIZE * WIDTH;

		using BlockData = ChunkData<BlockState, WIDTH>;
		using FluidData = ChunkData<FluidState, WIDTH>;
	private:
		std::unique_ptr<BlockData> blockData;
		std::unique_ptr<FluidData> fluidData;

		std::weak_ptr<RenderChunk> renderChunk;
		const World* world; // TODO: remove?
		ChunkCoord chunkCoord;
		glm::ivec3 blockPos;

	public:
		explicit ChunkBakeData(Chunk& chunk);

		ChunkBakeData(const ChunkBakeData&) = delete;
		ChunkBakeData(ChunkBakeData&&) = default;
		ChunkBakeData& operator =(const ChunkBakeData&) = delete;
		ChunkBakeData& operator =(ChunkBakeData&&) = default;

		// pos is relative to chunk origin
		inline BlockStateRef getBlockState(const glm::ivec3& pos) const noexcept {
			return (*blockData)[posToIndex(pos)];
		}
		// pos is relative to chunk origin
		inline FluidStateRef getFluidState(const glm::ivec3& pos) const noexcept {
			return (*fluidData)[posToIndex(pos)];
		}

		inline const std::shared_ptr<RenderChunk> getRenderChunk() const noexcept { return renderChunk.lock(); }
		inline std::shared_ptr<RenderChunk> getRenderChunk() noexcept { return renderChunk.lock(); }

		inline const World* getWorld() const noexcept { return world; } // TODO: remove?

		inline const ChunkCoord& getChunkCoord() const noexcept { return chunkCoord; }
		inline const glm::ivec3& getBlockPos() const noexcept { return blockPos; }

		inline const BlockData& getBlockData() const noexcept { return *blockData; }
		inline const FluidData& getFluidData() const noexcept { return *fluidData; }
		
		// cPos is relative to chunk origin
		static inline constexpr size_t posToIndex(const glm::ivec3& cPos) {
			return ((cPos.z + PADDING) * LAYER_SIZE) + ((cPos.y + PADDING) * WIDTH) + cPos.x + PADDING;
		}
		// (cX, cY, cZ) is relative to chunk origin
		static inline constexpr size_t posToIndex(const int cX, const int cY, const int cZ) {
			return ((cZ + PADDING) * LAYER_SIZE) + ((cY + PADDING) * WIDTH) + cX + PADDING;
		}

	};

}