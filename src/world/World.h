#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <utility>

#include <glm/vec3.hpp>

#define result_of invoke_result
#include <parallel_hashmap/phmap.h>

#include "block/Block.h"
#include "fluid/Fluid.h"
#include "chunk/Chunk.h"
#include "BlockUpdate.h"
#include "util/math/RNG.h"
#include "util/math/NoiseGen.h"
#include "util/RayCastResult.h"
#include "util/math/Ray.h"
#include "render/world/chunk/MeshingPriority.h"

#include "render/Camera.h" // TODO: remove
#include "util/type_traits.h"

namespace eng {

	enum class RayCastMask {
		None     = 0b0000,
		Blocks   = 0b0001,
		Fluids   = 0b0010,
		Entities = 0b0100,
	};
	constexpr RayCastMask operator |(const RayCastMask lhs, const RayCastMask rhs) noexcept {
		return RayCastMask{ to_underlying(lhs) | to_underlying(rhs) };
	}
	constexpr RayCastMask operator &(const RayCastMask lhs, const RayCastMask rhs) noexcept {
		return RayCastMask{ to_underlying(lhs) & to_underlying(rhs) };
	}

	class WorldRenderer;

	class World {
		friend class Chunk;
	public:
		using ChunkMap = phmap::parallel_flat_hash_map<ChunkCoord, Chunk>;//std::unordered_map<ChunkCoord, Chunk>;

	private:
		struct DirtyChunkPriority {
			MeshingPriority priority;
			bool fluidOnly = false;
		};

		uint64_t ticks = 0;
		ChunkMap loadedChunks;
		std::unordered_map<ChunkCoord, DirtyChunkPriority> dirtyChunks; // chunks that need to be remeshed, and the priority of the meshing task

		std::unordered_multimap<glm::ivec3, ScheduledBlockUpdate> scheduledBlockUpdates;
		std::vector<BlockUpdate> currentTickBlockUpdates;
		std::unordered_map<ChunkCoord, std::map<size_t, Direction>> blockUpdateCache;

		std::unordered_map<glm::ivec3, ScheduledFluidUpdate> scheduledFluidUpdates;
		std::vector<FluidUpdate> currentTickFluidUpdates;
		std::unordered_map<ChunkCoord, std::map<size_t, Direction>> fluidUpdateCache;

		WorldRenderer* worldRenderer = nullptr;

		Camera* player = nullptr; // TODO: replace with EntityPlayer*

	public:
		const RNG::seed_t seed;

	private:
		NoiseGen terrainGenNoise;
	public:
		static inline int loading_dist {};
		static inline int loading_dist_sqr {};
		static inline int unloading_dist {};
		static inline int unloading_dist_sqr {};
		static void setChunkLoadRadius(const int loadRadius) noexcept;

		static size_t maxChunkLoads; // maximum number of chunks to load per tick

		
		World(RNG::seed_t seed = RNG::randomSeed());
		World(std::string_view seed) : World(RNG::toSeed(seed)) {}


		inline RNG::seed_t getSeed() const noexcept { return seed; }

		void setPlayer(Camera* player) noexcept {
			this->player = player;
		}

		void setWorldRenderer(WorldRenderer* worldRenderer) noexcept {
			this->worldRenderer = worldRenderer;
		}

		inline const ChunkMap& getLoadedChunks() const noexcept {
			return loadedChunks;
		}

		void update();

		BlockStateRef getBlockState(const glm::ivec3& blockPos) const;
		bool setBlockState(const glm::ivec3& blockPos, BlockStateRef blockState, bool remesh = true, bool updateNeighbors = true, bool scheduleFluidUpdate = true, MeshingPriority meshingPriority = MeshingPriority::BlockUpdate);

		FluidStateRef getFluidState(const glm::ivec3& blockPos) const;
		bool setFluidState(const glm::ivec3& blockPos, FluidStateRef fluidState, bool remesh = true, bool updateNeighbors = true, bool scheduleUpdate = true, MeshingPriority meshingPriority = MeshingPriority::FluidUpdate);

		bool isChunkLoaded(const ChunkCoord&) const;
		inline bool isBlockLoaded(const glm::ivec3& blockPos) const {
			return isChunkLoaded(ChunkCoord::fromBlockPos(blockPos));
		}

		Chunk* getChunk(const ChunkCoord&);
		const Chunk* getChunk(const ChunkCoord&) const;

		RayCastResultF rayCast(const RayF& ray, RayCastMask mask) const; // TODO: take RayCastMask argument

		void scheduleBlockUpdate(const glm::ivec3& blockPos, Direction srcDir, uint16_t delay = 0);

		void scheduleFluidUpdate(const glm::ivec3& blockPos, Direction srcDir, uint16_t delay = 0);

		void scheduleChunkRemesh(const ChunkCoord& chunkCoord, const MeshingPriority meshingPriority, bool onlyFluid = false);

		int getTerrainHeight(const int x, const int z) const;

	protected:

		void doBlockUpdates();

		void doFluidUpdates();

		void loadChunk(const ChunkCoord&);
		void unloadChunk(const ChunkCoord&);

		bool canLoadChunk(const ChunkCoord&) const;
		bool shouldUnloadChunk(const ChunkCoord&) const;

		void cacheBlockUpdate(const BlockUpdate& blockUpdate);

		void cacheFluidUpdate(const FluidUpdate& fluidUpdate);

	};

}