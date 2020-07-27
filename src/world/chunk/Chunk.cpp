#include "Chunk.h"

#include <string>
#include <stdexcept>
#include <utility>

#include <glm/gtx/string_cast.hpp>

#include "world/World.h"
#include "block/BlockRegistry.h"
#include "fluid/FluidRegistry.h"
#include "model/block/BlockModel.h"
#include "util/math/RNG.h"

#include <chrono> // TODO: remove
#include <iostream> // TODO: remove

namespace eng {

	Chunk::Chunk(World* const world, const ChunkCoord& coord) :
			renderChunk(std::make_shared<RenderChunk>(coord)),
			world(world),
			chunkCoord(coord),
			blockPos(ChunkCoord::toBlockPos(coord)),
			boundingBox(blockPos, blockPos + static_cast<int>(WIDTH)) {
		generate();
	}

	inline bool Chunk::containsBlockPos(const glm::ivec3& blockPos) const noexcept {
		return chunkCoord == ChunkCoord::fromBlockPos(blockPos);
	}

	BlockStateRef Chunk::getBlockState(const glm::ivec3& pos) const {
		const auto i = posToIndex(pos - blockPos);
		if (i < 0 || i > SIZE)
			throw std::out_of_range("Chunk at ChunkCoord " + glm::to_string(static_cast<glm::ivec3>(chunkCoord)) + " does not contain BlockPos " + glm::to_string(pos));
		return blockData[i];
	}

	void Chunk::setBlockState(const glm::ivec3& blockPos, BlockStateRef blockState, const bool remesh, const bool updateNeighbors, bool scheduleFluidUpdate, const MeshingPriority meshingPriority) {
		const auto rPos = (blockPos - this->blockPos);
		const auto i = posToIndex(rPos);
		if (i < 0 || i > SIZE)
			throw std::out_of_range("Chunk at ChunkCoord " + glm::to_string(static_cast<glm::ivec3>(chunkCoord)) + " does not contain BlockPos " + glm::to_string(blockPos));
		blockData[i] = blockState;
		if (remesh) {
			updateMesh(meshingPriority);
			// schedule neighbor chunks for remeshing if the modified BlockState was on one of the edges of this chunk
			for (const Axis axis : axis::axes) {
				const auto axisIndex = getIndex(axis);
				if (rPos[axisIndex] == 0) {
					const Direction face = direction::getDirection(axis, AxisDirection::NEGATIVE);
					Chunk* const chunk = world->getChunk(chunkCoord.offset(face));
					if (chunk) chunk->updateMesh(meshingPriority);
				} else if (rPos[axisIndex] == (WIDTH - 1)) {
					const Direction face = direction::getDirection(axis, AxisDirection::POSITIVE);
					Chunk* const chunk = world->getChunk(chunkCoord.offset(face));
					if (chunk) chunk->updateMesh(meshingPriority);
				}
			}
		}
		if (scheduleFluidUpdate)
			world->scheduleFluidUpdate(blockPos, Direction::UNDEFINED);
		if (updateNeighbors) {
			for (const auto dir : direction::directions) { // schedule updates for all neighboring blocks & fluids
				world->scheduleBlockUpdate(direction::offsetVector(blockPos, dir), direction::getOpposite(dir));
				world->scheduleFluidUpdate(direction::offsetVector(blockPos, dir), direction::getOpposite(dir));
			}
		}
	}

	FluidStateRef Chunk::getFluidState(const glm::ivec3& pos) const {
		const auto i = posToIndex(pos - blockPos);
		if (i < 0 || i > SIZE)
			throw std::out_of_range("Chunk at ChunkCoord " + glm::to_string(static_cast<glm::ivec3>(chunkCoord)) + " does not contain BlockPos " + glm::to_string(pos));
		return fluidData[i];
	}

	void Chunk::setFluidState(const glm::ivec3& blockPos, FluidStateRef fluidState, const bool remesh, const bool updateNeighbors, const bool scheduleUpdate, const MeshingPriority meshingPriority) {
		const auto rPos = (blockPos - this->blockPos);
		const auto i = posToIndex(rPos);
		if (i < 0 || i > SIZE)
			throw std::out_of_range("Chunk at ChunkCoord " + glm::to_string(static_cast<glm::ivec3>(chunkCoord)) + " does not contain BlockPos " + glm::to_string(blockPos));
		fluidData[i] = fluidState;
		if (remesh) {
			updateMesh(meshingPriority, true);
			// schedule neighbor chunks for remeshing if the modified FluidState was on one of the edges of this chunk
			for (const Axis axis : axis::axes) {
				const auto axisIndex = getIndex(axis);
				if (rPos[axisIndex] == 0) {
					const Direction face = direction::getDirection(axis, AxisDirection::NEGATIVE);
					Chunk* const chunk = world->getChunk(chunkCoord.offset(face));
					if (chunk) chunk->updateMesh(meshingPriority, true);
				} else if (rPos[axisIndex] == (WIDTH - 1)) {
					const Direction face = direction::getDirection(axis, AxisDirection::POSITIVE);
					Chunk* const chunk = world->getChunk(chunkCoord.offset(face));
					if (chunk) chunk->updateMesh(meshingPriority, true);
				}
			}
		}
		if (scheduleUpdate)
			world->scheduleFluidUpdate(blockPos, Direction::UNDEFINED);
		if (updateNeighbors) {
			for (const auto dir : direction::directions) // schedule fluid updates for all neighboring blocks
				world->scheduleFluidUpdate(direction::offsetVector(blockPos, dir), direction::getOpposite(dir));
		}
	}


	

	void Chunk::generate() {
		// TODO: implement world generator
		RNG rand(world->getSeed() ^ std::hash<ChunkCoord>{}(chunkCoord));

		for (size_t z = 0; z < WIDTH; z++) {
			for (size_t x = 0; x < WIDTH; x++) {
				const auto terrainHeight = world->getTerrainHeight(x + blockPos.x, z + blockPos.z);
				for (size_t y = 0; y < WIDTH; y++) {
					const auto i = posToIndex(x, y, z);
					const int depth = -(static_cast<int>(y) + blockPos.y) +terrainHeight;

					constexpr float tallGrassChance = 0.35f;

					if (depth > 2) blockData[i] = { blocks::stone };
					else if (depth > 0) blockData[i] = { blocks::dirt };
					else if (depth == 0) blockData[i] = { blocks::grass };
					else if ((depth == -1) && (rand.nextFloat() < tallGrassChance)) blockData[i] = { blocks::tall_grass };
				}

				/*const auto isSolid = [&](const glm::ivec3& np) -> bool {
					return static_cast<int>(world->terrainGenNoise.getNoise(np + blockPos) * 128) < 51;
				};
				auto solid = isSolid({ x, WIDTH - 1, z });
				auto solidAbove1 = isSolid({ x, WIDTH + 0, z });
				auto solidAbove2 = isSolid({ x, WIDTH + 1, z });
				auto solidAbove3 = isSolid({ x, WIDTH + 2, z });
				for (int y = WIDTH - 1; y >= 0; y--) {
					const auto solidBelow = isSolid({ x, y - 1, z });
					const auto i = posToIndex(x, y, z);

					if (solid) {
						if (!solidAbove1) {
							blockData[i] = { blocks::grass };
						} else if (!solidAbove2 || !solidAbove3) {
							blockData[i] = { blocks::dirt };
						} else {
							blockData[i] = { blocks::stone };
						}
					} else {
						if (solidBelow) {
							constexpr float tallGrassChance = 0.35f;
							if (rand.nextFloat() < tallGrassChance) blockData[i] = { blocks::tall_grass };
						}
					}


					solidAbove3 = solidAbove2;
					solidAbove2 = solidAbove1;
					solidAbove1 = solid;
					solid = solidBelow;
				}*/
			}
		}
	}

	inline void Chunk::updateMesh(const MeshingPriority meshingPriority, const bool onlyFluid) {
		world->scheduleChunkRemesh(chunkCoord, meshingPriority, onlyFluid);
	}

}