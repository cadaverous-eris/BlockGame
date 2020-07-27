#include "ChunkBakery.h"

#include <shared_mutex>
#include <algorithm>
#include <utility>
#include <chrono>
#include <cassert>

#include "block/Block.h"
#include "fluid/Fluid.h"
#include "world/chunk/Chunk.h"
#include "world/World.h"
#include "model/block/BlockModel.h"
#include "RenderChunk.h"
#include "MeshingWorldView.h"
#include "util/math/math.h"

#include <iostream> // TODO: remove

namespace eng {

	// temporary storage for blockQuads created during meshing
	thread_local static std::array<std::vector<BlockQuad>, render_layer::layers.size()> blockScratchBuffers;
	thread_local static std::array<std::vector<BlockQuad>, render_layer::layers.size()> fluidScratchBuffers;

	ChunkBakery::ChunkBakery(const size_t scratchBufferInitSize) : scratchBufferInitSize(scratchBufferInitSize) {
		taskQueue.reserve(12);
		const auto hardwareThreads = std::thread::hardware_concurrency();
		const auto numThreads = 3;//vmax(static_cast<int>(hardwareThreads) - 3, 1); // TODO: change how thread count is calculated
		for (int i = 0; i < numThreads; i++)
			threads.emplace_back(ChunkBakery::runBakerThread, this);
	}

	ChunkBakery::~ChunkBakery() {
		{
			std::scoped_lock<std::mutex> lock { taskQueueMutex };
			taskQueue.clear();
		}
		destroyed = true;
		condVar.notify_all();
		for (auto& thread : threads) {
			if (thread.joinable()) thread.join();
		}
	}

	void ChunkBakery::enqueueTask(Chunk& chunk, const MeshingPriority priority, const bool fluidOnly) {
		std::scoped_lock<std::mutex> writerLock { taskQueueMutex };

		//std::cout << "chunk bakery task queue size: " << taskQueue.size() << '\n';

		// old, priority-ignoring version
		/*auto existingIt = std::find_if(taskQueue.begin(), taskQueue.end(), [&chunk](const auto& task) -> bool {
			return chunk.chunkCoord == task.getChunkCoord();
		});
		if (existingIt != taskQueue.end()) {
			//existingIt->chunkData = ChunkBakeData { chunk };
			(*existingIt).chunkData = ChunkBakeData { chunk };
			(*existingIt).fluidOnly = fluidOnly;
			//condVar.notify_one(); // TODO: notify condition variable?
		} else {
			taskQueue.emplace_back(chunk, priority, fluidOnly);
			condVar.notify_one();
		}*/

		auto existingIt = std::find_if(taskQueue.begin(), taskQueue.end(), [&chunk](const MeshingTask& task) -> bool {
			return chunk.getChunkCoord() == task.getChunkCoord();
		});
		if (existingIt != taskQueue.end()) {
			if (existingIt->priority <= priority) {
				existingIt->chunkData = ChunkBakeData { chunk };
				existingIt->fluidOnly &= fluidOnly;
				//condVar.notify_one(); // TODO: notify condition variable?
				return;
			}
			taskQueue.erase(existingIt);
		}
		auto insertionIt = taskQueue.end();
		for (auto it = taskQueue.begin(); it != taskQueue.end(); it++) {
			// set insertionIt to the position of the first element with a higher priority
			if (it->priority > priority) {
				insertionIt = it;
				break;
			}
		}
		taskQueue.emplace(insertionIt, chunk, priority, fluidOnly);
		condVar.notify_one();
	}

	size_t ChunkBakery::queuedTasks() const {
		std::scoped_lock<std::mutex> writerLock { taskQueueMutex };
		return taskQueue.size();
	}

	void ChunkBakery::runBakerThread(ChunkBakery* const bakery) {
		for (auto& buf : blockScratchBuffers)
			buf.reserve(bakery->scratchBufferInitSize);
		for (auto& buf : fluidScratchBuffers)
			buf.reserve(bakery->scratchBufferInitSize);
		while (!bakery->destroyed) {
			std::unique_lock<std::mutex> lock(bakery->taskQueueMutex);
			if (bakery->taskQueue.empty())
				bakery->condVar.wait_for(lock, std::chrono::milliseconds(1000));
			if (!bakery->taskQueue.empty()) {
				const bool fluidOnly = bakery->taskQueue.front().fluidOnly;
				ChunkBakeData chunkBakeData = std::move(bakery->taskQueue.front().chunkData);
				bakery->taskQueue.erase(bakery->taskQueue.begin());
				lock.unlock();
				bakeChunk(chunkBakeData, fluidOnly);
			}
		}
	}

	void ChunkBakery::bakeChunk(ChunkBakeData& chunkData, bool fluidOnly) {
		if (std::shared_ptr<RenderChunk> renderChunk = chunkData.getRenderChunk(); renderChunk) {
			const MeshingWorldView worldView(chunkData);

			for (size_t j = 0; j < Chunk::SIZE; j++) {
				const glm::ivec3 pos = Chunk::indexToPos(j);
				const auto index = ChunkBakeData::posToIndex(pos);

				if (!fluidOnly) { // Block
					const BlockState& blockState = chunkData.getBlockData()[index];
					BlockRef block = blockState.getBlock();
					if (block.hasModel(blockState)) {
						const glm::ivec3 worldPos = pos + chunkData.getBlockPos();

						// face quads
						for (const Direction face : direction::directions) {
							const glm::ivec3 nPos = offsetVector(pos, face);
							const auto& nState = chunkData.getBlockData()[ChunkBakeData::posToIndex(nPos)];
							const bool cullFace = nState.getBlock().canCullAdjacentFace(nState, getOpposite(face), blockState);
							if (!cullFace)
								for (const auto layer : render_layer::layers) {
									const auto layerIndex = render_layer::getIndex(layer);
									block.addFaceQuadsToBuffer(blockState, worldPos, face, layer, pos, worldView, blockScratchBuffers[layerIndex]);
								}
						}
						// non-face quads
						for (const auto layer : render_layer::layers) {
							const auto layerIndex = render_layer::getIndex(layer);
							block.addFaceQuadsToBuffer(blockState, worldPos, Direction::UNDEFINED, layer, pos, worldView, blockScratchBuffers[layerIndex]);
						}
					}
				}

				{ // Fluid
					const FluidState& fluidState = chunkData.getFluidData()[index];
					FluidRef fluid = fluidState.getFluid();
					if (!fluid.isNullFluid(fluidState)) {
						const glm::ivec3 worldPos = pos + chunkData.getBlockPos();
						for (auto layer : render_layer::layers) {
							const auto layerIndex = render_layer::getIndex(layer);
							fluid.addQuadsToBuffer(fluidState, worldPos, layer, pos, worldView, fluidScratchBuffers[layerIndex]);
						}
					}
				}
			}

			std::scoped_lock<std::mutex> meshLock { renderChunk->getMeshMutex() };
			ChunkMesh& chunkMesh = renderChunk->getMesh();
			for (const RenderLayer layer : render_layer::layers) {
				const auto layerIndex = render_layer::getIndex(layer);
				if (fluidOnly) {
					const auto blockQuadCount = chunkMesh.blockQuadCounts[layerIndex];
					const auto totalQuads = blockQuadCount + fluidScratchBuffers[layerIndex].size();
					if (totalQuads > 0) {
						if (chunkMesh.layerQuads[layerIndex].capacity() == 0) {
							chunkMesh.layerQuads[layerIndex] = ChunkMesh::getPooledQuadList(totalQuads);
						} else {
							chunkMesh.layerQuads[layerIndex].resize(blockQuadCount);
						}
						chunkMesh.layerQuads[layerIndex].insert(
							chunkMesh.layerQuads[layerIndex].end(),
							fluidScratchBuffers[layerIndex].begin(),
							fluidScratchBuffers[layerIndex].end());
					} else {
						ChunkMesh::poolQuadList(chunkMesh.layerQuads[layerIndex]);
					}
				} else {
					const auto blockQuadCount = blockScratchBuffers[layerIndex].size();
					const auto totalQuads = blockQuadCount + fluidScratchBuffers[layerIndex].size();
					chunkMesh.blockQuadCounts[layerIndex] = blockQuadCount;
					if (totalQuads > 0) {
						if (chunkMesh.layerQuads[layerIndex].capacity() == 0) {
							chunkMesh.layerQuads[layerIndex] = ChunkMesh::getPooledQuadList(totalQuads);
						} else {
							chunkMesh.layerQuads[layerIndex].clear();
						}
						chunkMesh.layerQuads[layerIndex].insert(
							chunkMesh.layerQuads[layerIndex].begin(),
							blockScratchBuffers[layerIndex].begin(),
							blockScratchBuffers[layerIndex].end());
						chunkMesh.layerQuads[layerIndex].insert(
							chunkMesh.layerQuads[layerIndex].end(),
							fluidScratchBuffers[layerIndex].begin(),
							fluidScratchBuffers[layerIndex].end());
					} else {
						ChunkMesh::poolQuadList(chunkMesh.layerQuads[layerIndex]);
					}
				}
				blockScratchBuffers[layerIndex].clear();
				fluidScratchBuffers[layerIndex].clear();
			}
			renderChunk->markDirty();
		}
	}

	ChunkBakery::MeshingTask::MeshingTask(Chunk& chunk, const MeshingPriority& priority, const bool fluidOnly) :
			chunkData(chunk), priority(priority), fluidOnly(fluidOnly) {}
	ChunkBakery::MeshingTask::MeshingTask(ChunkBakeData&& chunkData, const MeshingPriority& priority, const bool fluidOnly) :
			chunkData(std::move(chunkData)), priority(priority), fluidOnly(fluidOnly) {}

	ChunkBakery::MeshingTask::MeshingTask(ChunkBakery::MeshingTask&& b) :
			chunkData(std::move(b.chunkData)), priority(b.priority), fluidOnly(b.fluidOnly) {}
	ChunkBakery::MeshingTask& ChunkBakery::MeshingTask::operator =(ChunkBakery::MeshingTask&& b) {
		if (this != &b) {
			chunkData = std::move(b.chunkData);
			priority = b.priority;
			fluidOnly = b.fluidOnly;
		}
		return *this;
	}

	inline const ChunkCoord& ChunkBakery::MeshingTask::getChunkCoord() const noexcept {
		return chunkData.getChunkCoord();
	}

}