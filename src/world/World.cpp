#include "World.h"

#include <unordered_set>

#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include "util/direction.h"
#include "util/math/AxisAlignedBox.h"
#include "util/math/math.h"
#include "block/BlockRegistry.h"
#include "fluid/FluidRegistry.h"
#include "render/world/WorldRenderer.h"
#include "render/world/chunk/ChunkBakery.h"

#include <iostream> // TODO: remove

namespace eng {

	static constexpr size_t max_fluid_updates = 512; // TODO: tune value

	// TODO: dynamically determine number of chunks to load each tick
	size_t World::maxChunkLoads { 3 }; // maximum number of chunks to load per tick


	World::World(RNG::seed_t seed) :
			seed(seed),
			//terrainGenNoise(seed, { 53.0, 1.0, 0.8, 3 }) {
			terrainGenNoise(seed, { 123.0, 1.0, 0.6, 4 }) {
		const auto loadingAreaDim = static_cast<size_t>(std::ceil((unloading_dist * 2.0f) / Chunk::WIDTH));
		loadedChunks.reserve(loadingAreaDim * loadingAreaDim * loadingAreaDim);
	}

	void World::update() {
		const ChunkCoord playerChunkCoord = ChunkCoord::fromBlockPos(static_cast<glm::ivec3>(player->getPosition()));

		std::vector<ChunkCoord> toLoad {};
		std::vector<ChunkCoord> toUnload {};

		constexpr size_t meshingQueueLimit = 8;
		const size_t meshingQueueSize = (worldRenderer) ? worldRenderer->getChunkBakery().queuedTasks() : 0;
		if ((meshingQueueSize < meshingQueueLimit)/* && ((ticks % 1) == 0)*/) {
			const auto loadLimit = std::min(meshingQueueLimit - meshingQueueSize, maxChunkLoads);
			const int chunkLoadRadius = (static_cast<size_t>(loading_dist) / Chunk::WIDTH);
			for (int lr = 0; lr <= chunkLoadRadius; lr++) {
				for (int cz = -lr; cz <= lr; cz++) {
					for (int cx = (cz == lr || cz == -lr) ? 0 : lr; glm::abs(cx) <= lr; cx = -cx + static_cast<int>(cx <= 0)) {
						for (int cy = 0; glm::abs(cy) <= chunkLoadRadius; cy = -cy + static_cast<int>(cy <= 0)) {
							const ChunkCoord chunkCoord { playerChunkCoord.x + cx, playerChunkCoord.y + cy, playerChunkCoord.z + cz };
							if (!isChunkLoaded(chunkCoord) && canLoadChunk(chunkCoord)) {
								toLoad.push_back(chunkCoord);
								if (toLoad.size() >= loadLimit)
									goto endloadloop;
							}
						}
					}
				}
			}
		endloadloop:
			;
		}

		doFluidUpdates();

		doBlockUpdates();

		for (auto it = loadedChunks.begin(); it != loadedChunks.end(); it++) {
			const auto& [chunkCoord, chunk] = *it;

			// unload the chunk if it's out of range
			if (shouldUnloadChunk(chunkCoord)) {
				toUnload.push_back(chunkCoord);
				continue;
			}

			// TODO: handle chunk updates

			// TODO: random block ticks
		}

		// TODO: tick entities

		// TODO: tick tile entities

		// load the chunks that were scheduled for loading
		for (const auto& chunkCoord : toLoad)
			loadChunk(chunkCoord);

		// unload chunks that were scheduled for unloading
		for (const auto& chunkCoord : toUnload)
			unloadChunk(chunkCoord);

		// remesh chunks
		if (worldRenderer) {
			ChunkBakery& chunkBakery = worldRenderer->getChunkBakery();
			for (const auto& [chunkCoord, meshingPriority] : dirtyChunks) {
				if (auto chunk = getChunk(chunkCoord); chunk)
					chunkBakery.enqueueTask(*chunk, meshingPriority.priority, meshingPriority.fluidOnly);
			}
		}
		dirtyChunks.clear();

		ticks++;
	}



	const BlockState& World::getBlockState(const glm::ivec3& blockPos) const {
		if (const Chunk* const chunk = getChunk(ChunkCoord::fromBlockPos(blockPos)); chunk) {
			return chunk->getBlockState(blockPos);
		}
		return blocks::empty_blockstate;
	}
	bool World::setBlockState(const glm::ivec3& blockPos, BlockStateRef blockState, const bool remesh, const bool updateNeighbors, bool scheduleFluidUpdate, const MeshingPriority meshingPriority) {
		if (Chunk* const chunk = getChunk(ChunkCoord::fromBlockPos(blockPos)); chunk) {
			chunk->setBlockState(blockPos, blockState, remesh, updateNeighbors, scheduleFluidUpdate, meshingPriority);
			return true;
		}
		return false;
	}

	FluidStateRef World::getFluidState(const glm::ivec3& blockPos) const {
		if (const Chunk* const chunk = getChunk(ChunkCoord::fromBlockPos(blockPos)); chunk) {
			return chunk->getFluidState(blockPos);
		}
		return fluids::empty_fluidstate;
	}
	bool World::setFluidState(const glm::ivec3& blockPos, FluidStateRef fluidState, bool remesh, bool updateNeighbors, bool scheduleUpdate, MeshingPriority meshingPriority) {
		if (Chunk* const chunk = getChunk(ChunkCoord::fromBlockPos(blockPos)); chunk) {
			chunk->setFluidState(blockPos, fluidState, remesh, updateNeighbors, scheduleUpdate, meshingPriority);
			return true;
		}
		return false;
	}

	bool World::isChunkLoaded(const ChunkCoord& chunkCoord) const {
		return loadedChunks.find(chunkCoord) != loadedChunks.end();
	}
	Chunk* World::getChunk(const ChunkCoord& chunkCoord) {
		auto it = loadedChunks.find(chunkCoord);
		return (it == loadedChunks.end()) ? nullptr : &(it->second);
	}
	const Chunk* World::getChunk(const ChunkCoord& chunkCoord) const {
		const auto it = loadedChunks.find(chunkCoord);
		return (it == loadedChunks.end()) ? nullptr : &(it->second);
	}

	int World::getTerrainHeight(const int x, const int z) const {
		const auto noise = terrainGenNoise.getNoise({ x, z });
		//const auto terrainHeight = static_cast<int>(noise * 188) - 94;
		const auto terrainHeight = static_cast<int>(noise * 512) - 128;

		return terrainHeight;
	}

	RayCastResultF World::rayCast(const RayF& ray, RayCastMask mask) const {
		float closestDist = ray.getLength();
		glm::ivec3 hitBlockPos {};
		Direction hitFace = Direction::UNDEFINED;
		RayCastResultType hitType = RayCastResultType::None;
		bool hasHit = false;

		for (const auto& [chunkCoord, chunk] : loadedChunks) {
			if (!chunk.getBoundingBox().intersectRay(ray)) continue;

			const Chunk::BlockData& chunkBlockData = chunk.getBlockData();
			const Chunk::FluidData& chunkFluidData = chunk.getFluidData();

			if ((mask & (RayCastMask::Blocks | RayCastMask::Fluids)) != RayCastMask::None) {
				for (size_t i = 0; i < Chunk::SIZE; i++) {
					// TODO: skip blocks & fluids outside of the aabb from origin to (origin + (direction * range))?
					const auto blockPos = chunk.getBlockPos() + Chunk::indexToPos(i);
					{ // Blocks
						const auto& blockState = chunkBlockData[i];
						const auto box = blockState.getBlock().getSelectionBox(*this, blockState, blockPos) + blockPos;
						float dist;
						const Direction face = box.intersectRayFace(ray, dist);
						if ((face != Direction::UNDEFINED) && (dist <= closestDist)) {
							closestDist = dist;
							hitBlockPos = blockPos;
							hitFace = face;
							hitType = RayCastResultType::Block;
							hasHit = true;
						}
					}
					if ((mask & RayCastMask::Fluids) != RayCastMask::None) { // Fluids
						const auto& fluidState = chunkFluidData[i];
						const auto box = fluidState.getFluid().getBoundingBox(*this, fluidState, blockPos) + blockPos;

						float dist;
						const Direction face = box.intersectRayFace(ray, dist);
						if ((face != Direction::UNDEFINED) && (dist <= closestDist)) {
							closestDist = dist;
							hitBlockPos = blockPos;
							hitFace = face;
							hitType = RayCastResultType::Fluid;
							hasHit = true;
						}
					}
				}
			}
		}
		/*if (hasHit) {
			constexpr auto eps = std::numeric_limits<float>::epsilon();
			const auto calculatedDistance = glm::distance(ray.getOrigin(), ray.getOrigin() + (ray.getDir() * closestDist));
			if (epsNotEqual(calculatedDistance, closestDist))
				std::cout << calculatedDistance << " != " << closestDist << "\n";
		}*/
		if (hasHit) return {
			hitType,
			ray.project(closestDist),
			hitBlockPos,
			hitFace,
			closestDist,
		};

		return {}; // no result
	}

	void World::loadChunk(const ChunkCoord& chunkCoord) {
		loadedChunks.try_emplace(chunkCoord, this, chunkCoord);
		scheduleChunkRemesh(chunkCoord, MeshingPriority::ChunkLoad);
		// schedule neighbor chunks for re-meshing
		for (const Direction d : direction::directions)
			scheduleChunkRemesh(chunkCoord.offset(d), MeshingPriority::ChunkLoad);
	}
	void World::unloadChunk(const ChunkCoord& chunkCoord) {
		// schedule neighbor chunks for re-meshing
		for (const Direction d : direction::directions)
			scheduleChunkRemesh(chunkCoord.offset(d), MeshingPriority::ChunkUnload);
		loadedChunks.erase(chunkCoord);
	}

	bool World::canLoadChunk(const ChunkCoord& chunkCoord) const {
		const glm::ivec3 chunkCenter { ChunkCoord::toBlockPos(chunkCoord) + glm::ivec3(Chunk::WIDTH / 2, Chunk::WIDTH / 2, Chunk::WIDTH / 2) };
		const auto distSqr = glm::distance2(static_cast<glm::vec3>(chunkCenter), player->getPosition());
		return distSqr <= static_cast<float>(loading_dist_sqr);
	}
	bool World::shouldUnloadChunk(const ChunkCoord& chunkCoord) const {
		const glm::vec3 chunkCenter { ChunkCoord::toBlockPos(chunkCoord) + glm::ivec3(Chunk::WIDTH / 2, Chunk::WIDTH / 2, Chunk::WIDTH / 2) };
		const auto distSqr = glm::distance2(chunkCenter, player->getPosition());
		return distSqr > static_cast<float>(unloading_dist_sqr);
	}

	void World::setChunkLoadRadius(const int loadRadius) noexcept {
		World::loading_dist = loadRadius;
		World::loading_dist_sqr = World::loading_dist * World::loading_dist;
		World::unloading_dist = World::loading_dist + 32;
		World::unloading_dist_sqr = World::unloading_dist * World::unloading_dist;
	}


	void World::scheduleChunkRemesh(const ChunkCoord& chunkCoord, const MeshingPriority meshingPriority, const bool onlyFluid) {
		if (auto it = dirtyChunks.find(chunkCoord); it != dirtyChunks.end()) {
			const auto [cc, prevPriority] = *it;
			it->second.fluidOnly &= onlyFluid;
			if (meshingPriority < prevPriority.priority)
				it->second.priority = meshingPriority;
		} else {
			dirtyChunks[chunkCoord] = { meshingPriority, onlyFluid };
		}
	}


	void World::scheduleBlockUpdate(const glm::ivec3& blockPos, const Direction srcDir, const uint16_t delay) {
		const auto [begin, end] = scheduledBlockUpdates.equal_range(blockPos);
		for (auto it = begin; it != end; it++) {
			if (it->second.delay == delay) {
				it->second.blockUpdate.srcDirection = BlockUpdate::mergeDirections(it->second.blockUpdate.srcDirection, srcDir);
				return;
			}
		}
		scheduledBlockUpdates.emplace(blockPos, ScheduledBlockUpdate({ blockPos, srcDir }, delay));
	}

	void World::doBlockUpdates() {
		currentTickBlockUpdates.clear();
		for (auto it = scheduledBlockUpdates.begin(); it != scheduledBlockUpdates.end();) {
			auto& [blockPos, scheduledUpdate] = *it;
			if (scheduledUpdate.delay == 0) {
				if (isBlockLoaded(blockPos))
					currentTickBlockUpdates.push_back(scheduledUpdate.blockUpdate);
				else
					cacheBlockUpdate(scheduledUpdate.blockUpdate);
				it = scheduledBlockUpdates.erase(it);
				continue;
			}
			scheduledUpdate.delay--;
			it++;
		}
		for (const auto& blockUpdate : currentTickBlockUpdates) {
			const BlockState& blockState = getBlockState(blockUpdate.blockPos);
			blockState.getBlock().onBlockUpdate(*this, blockState, blockUpdate.blockPos, blockUpdate.srcDirection);
		}
	}

	void World::cacheBlockUpdate(const BlockUpdate& blockUpdate) {
		const ChunkCoord chunkCoord = ChunkCoord::fromBlockPos(blockUpdate.blockPos);
		const size_t index = Chunk::posToIndex(chunkCoord.getRelativeBlockPos(blockUpdate.blockPos));
		auto& updateCache = blockUpdateCache[chunkCoord];
		if (auto it = updateCache.find(index); it != updateCache.end()) {
			it->second = BlockUpdate::mergeDirections(it->second, blockUpdate.srcDirection);
		} else {
			updateCache[index] = blockUpdate.srcDirection;
		}
	}

	void World::scheduleFluidUpdate(const glm::ivec3& blockPos, const Direction srcDir, const uint16_t delay) {
		const auto [begin, end] = scheduledFluidUpdates.equal_range(blockPos);
		for (auto it = begin; it != end; it++) {
			if (it->second.delay == delay) {
				it->second.blockUpdate.srcDirection = FluidUpdate::mergeDirections(it->second.blockUpdate.srcDirection, srcDir);
				return;
			}
		}
		scheduledFluidUpdates.emplace(blockPos, ScheduledFluidUpdate({ blockPos, srcDir }, delay));
	}

	void World::doFluidUpdates() {
		const auto prevUpdateCount = currentTickFluidUpdates.size();
		currentTickFluidUpdates.clear();
		for (auto it = scheduledFluidUpdates.begin(); it != scheduledFluidUpdates.end();) {
			auto& [blockPos, scheduledUpdate] = *it;
			if (scheduledUpdate.delay == 0) {
				if (isBlockLoaded(blockPos)) {
					if (getFluidState(blockPos).isEmpty()) {
					} else if (currentTickFluidUpdates.size() < max_fluid_updates) {
						currentTickFluidUpdates.push_back(scheduledUpdate.blockUpdate);
					} else {
						it++;
						continue;
					}
				} else {
					cacheFluidUpdate(scheduledUpdate.blockUpdate);
				}
				it = scheduledFluidUpdates.erase(it);
				continue;
			}
			scheduledUpdate.delay--;
			it++;
		}
		//const auto updateCount = currentTickFluidUpdates.size();
		//if (updateCount != prevUpdateCount)
		//	std::cout << "Fluid Updates: " << updateCount << '\n';
		for (const auto& fluidUpdate : currentTickFluidUpdates) {
			const FluidState& fluidState = getFluidState(fluidUpdate.blockPos);
			fluidState.getFluid().onFluidUpdate(*this, fluidState, fluidUpdate.blockPos, fluidUpdate.srcDirection);
		}
	}

	void World::cacheFluidUpdate(const FluidUpdate& fluidUpdate) {
		const ChunkCoord chunkCoord = ChunkCoord::fromBlockPos(fluidUpdate.blockPos);
		const size_t index = Chunk::posToIndex(chunkCoord.getRelativeBlockPos(fluidUpdate.blockPos));
		auto& updateCache = fluidUpdateCache[chunkCoord];
		if (auto it = updateCache.find(index); it != updateCache.end()) {
			it->second = BlockUpdate::mergeDirections(it->second, fluidUpdate.srcDirection);
		} else {
			updateCache[index] = fluidUpdate.srcDirection;
		}
	}

}