#pragma once

#include <array>
#include <functional>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/gtc/integer.hpp>

#include "chunk_consts.h"
#include "ChunkData.h"
#include "util/direction.h"
#include "util/math/math.h"
#include "block/BlockState.h"
#include "fluid/FluidState.h"
#include "world/BlockLight.h"
#include "render/world/chunk/RenderChunk.h"
#include "util/math/AxisAlignedBox.h"
#include "ChunkCoord.h"
#include "render/world/chunk/MeshingPriority.h"

namespace eng {

	class World;
	class ChunkBakery;

	class Chunk {
	public:
		static constexpr size_t WIDTH = chunk_width; // width, depth, and height of a chunk in blocks
		static constexpr size_t LAYER_SIZE = chunk_layer_size; // number of blocks per 2d slice of the chunk
		static constexpr size_t SIZE = chunk_volume; // total number of blocks in a chunk
		static inline constexpr size_t LOG2_WIDTH = chunk_log2_width;

		using BlockData = ChunkData<BlockState, WIDTH>;
		using FluidData = ChunkData<FluidState, WIDTH>;
		using LightData = ChunkData<BlockLight, WIDTH>;
	private:
		BlockData blockData;
		FluidData fluidData;
		//LightData lightData;

		std::shared_ptr<RenderChunk> renderChunk;

		World* world;
		ChunkCoord chunkCoord;
		glm::ivec3 blockPos;
		AxisAlignedBox<int> boundingBox;

	public:

		Chunk(World* const world, const ChunkCoord& coord);

		inline World* getWorld() noexcept { return world; }
		inline const World* getWorld() const noexcept { return world; }
		inline const ChunkCoord& getChunkCoord() const noexcept { return chunkCoord; }
		inline const glm::ivec3& getBlockPos() const noexcept { return blockPos; }
		inline const AxisAlignedBox<int>& getBoundingBox() const noexcept { return boundingBox; }


		inline bool containsBlockPos(const glm::ivec3& blockPos) const noexcept;

		// returns the blockstate at a given world position
		// pos is a block pos in world coordinates
		BlockStateRef getBlockState(const glm::ivec3& pos) const;
		// pos is a block pos in world coordinates
		void setBlockState(const glm::ivec3& blockPos, BlockStateRef blockState, bool remesh = true, bool updateNeighbors = true, bool scheduleFluidUpdate = true, MeshingPriority meshingPriority = MeshingPriority::BlockUpdate);

		// returns the fluidstate at a given world position
		// pos is a block pos in world coordinates
		FluidStateRef getFluidState(const glm::ivec3& pos) const;
		// pos is a block pos in world coordinates
		void setFluidState(const glm::ivec3& pos, FluidStateRef fluidState, bool remesh = true, bool updateNeighbors = true, bool scheduleUpdate = true, MeshingPriority meshingPriority = MeshingPriority::FluidUpdate);

		inline const BlockData& getBlockData() const noexcept { return blockData; }
		inline const FluidData& getFluidData() const noexcept { return fluidData; }
		//inline const LightData& getLightData() const noexcept { return lightData; }

		inline const std::shared_ptr<RenderChunk> getRenderChunk() const noexcept { return renderChunk; }
		inline std::shared_ptr<RenderChunk> getRenderChunk() noexcept { return renderChunk; }
		inline const RenderChunk* getRawRenderChunk() const noexcept { return renderChunk.get(); }
		inline RenderChunk* getRawRenderChunk() noexcept { return renderChunk.get(); }

		void updateMesh(MeshingPriority meshingPriority = MeshingPriority::BlockUpdate, bool onlyFluid = false);

		// cPos is relative to chunk origin
		static inline constexpr size_t posToIndex(const glm::ivec3& cPos) {
			return (cPos.z * LAYER_SIZE) + (cPos.y * WIDTH) + cPos.x;
			//return ((cPos.z & (WIDTH - 1)) << (LOG2_WIDTH * 2)) | ((cPos.y & (WIDTH - 1)) << LOG2_WIDTH) | (cPos.x & (WIDTH - 1));
		}
		// (cX, cY, cZ) is relative to chunk origin
		static inline constexpr size_t posToIndex(const size_t cX, const size_t cY, const size_t cZ) {
			return (cZ * LAYER_SIZE) + (cY * WIDTH) + cX;
			//return ((cZ & (WIDTH - 1)) << (LOG2_WIDTH * 2)) | ((cY & (WIDTH - 1)) << LOG2_WIDTH) | (cX & (WIDTH - 1));
		}
		static inline constexpr glm::ivec3 indexToPos(const size_t index) {
			//const int z = index / LAYER_SIZE;
			//const int y = (index - (z * LAYER_SIZE)) / WIDTH;
			//const int x = index - ((z * LAYER_SIZE) + (y * WIDTH));
			//const int z = index / LAYER_SIZE;
			//const int y = (index - (z * LAYER_SIZE)) / WIDTH;
			//return { x, y, z };
			//return { index % WIDTH, (index % LAYER_SIZE) / WIDTH, index / LAYER_SIZE };
			return {
				index & (WIDTH - 1),
				(index >> LOG2_WIDTH) & (WIDTH - 1),
				(index >> (LOG2_WIDTH * 2)) & (WIDTH - 1),
			};
		}

	private:

		void generate();

	};

}