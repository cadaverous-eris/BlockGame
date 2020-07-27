#include "ChunkBakeData.h"

#include <array>
#include <cstring>
#include <algorithm>
#include <utility>

#include "world/World.h"

namespace eng {

	ChunkBakeData::ChunkBakeData(Chunk& chunk) :
			blockData(std::make_unique<BlockData>()),
			fluidData(std::make_unique<FluidData>()),
			renderChunk(chunk.getRenderChunk()),
			world(chunk.getWorld()),
			chunkCoord(chunk.getChunkCoord()),
			blockPos(chunk.getBlockPos()) {
		constexpr std::array<int, 3> copyStarts { -static_cast<int>(PADDING), 0, Chunk::WIDTH };
		constexpr std::array<size_t, 3> copyLengths { PADDING, Chunk::WIDTH, PADDING };

		const std::array<const Chunk*, 27> chunkNeighborhood {
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1, -1, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1,  0, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1,  1, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1, -1,  0))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1,  0,  0))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1,  1,  0))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1, -1,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1,  0,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3(-1,  1,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0, -1, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0,  0, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0,  1, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0, -1,  0))),
			&chunk,
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0,  1,  0))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0, -1,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0,  0,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 0,  1,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1, -1, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1,  0, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1,  1, -1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1, -1,  0))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1,  0,  0))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1,  1,  0))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1, -1,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1,  0,  1))),
			world->getChunk(ChunkCoord(chunkCoord + glm::ivec3( 1,  1,  1))),
		};

		for (int ix = 0; ix < 3; ix++) {
			const int x = copyStarts[ix];
			for (int z = -static_cast<int>(PADDING); z < static_cast<int>(Chunk::WIDTH + PADDING); z++) {
				for (int y = -static_cast<int>(PADDING); y < static_cast<int>(Chunk::WIDTH + PADDING); y++) {
					//const glm::ivec3 cOff { ix - 1, ((y + Chunk::WIDTH) / Chunk::WIDTH) - 1, ((z + Chunk::WIDTH) / Chunk::WIDTH) - 1 };
					const int iz = ((z + Chunk::WIDTH) / Chunk::WIDTH);
					const int iy = ((y + Chunk::WIDTH) / Chunk::WIDTH);
					const auto cOff = (ix * 9) + (iz * 3) + iy;
					const Chunk* const c = chunkNeighborhood[cOff];
					if (c) {
						const auto meshDataI = posToIndex(x, y, z);
						const auto chunkI = Chunk::posToIndex((x + Chunk::WIDTH) % Chunk::WIDTH, (y + Chunk::WIDTH) % Chunk::WIDTH, (z + Chunk::WIDTH) % Chunk::WIDTH);
						const auto copyLenBlock = copyLengths[ix] * sizeof(BlockState);
						const auto copyLenFluid = copyLengths[ix] * sizeof(FluidState);
						std::memcpy(blockData->data() + meshDataI, c->getBlockData().data() + chunkI, copyLenBlock);
						std::memcpy(fluidData->data() + meshDataI, c->getFluidData().data() + chunkI, copyLenFluid);
					}
				}
			}
		}
	}


}