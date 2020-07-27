#pragma once

#include <vector>
#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

#include "world/chunk/Chunk.h"
#include "ChunkMesh.h"
#include "ChunkBakeData.h"
#include "render/world/RenderLayer.h"
#include "MeshingPriority.h"

namespace eng {

	class ChunkBakery {
	private:
		struct MeshingTask {
			ChunkBakeData chunkData;
			MeshingPriority priority;
			bool fluidOnly;

			MeshingTask(Chunk&, const MeshingPriority&, bool fluidOnly);
			MeshingTask(ChunkBakeData&&, const MeshingPriority&, bool fluidOnly);

			MeshingTask(const MeshingTask&) = delete;
			MeshingTask& operator =(const MeshingTask&) = delete;

			MeshingTask(MeshingTask&&);
			MeshingTask& operator =(MeshingTask&&);

			const ChunkCoord& getChunkCoord() const noexcept;
		};

		mutable std::mutex taskQueueMutex {};
		std::condition_variable condVar {};
		std::atomic_bool destroyed { false };
		std::vector<MeshingTask> taskQueue {};
		std::vector<std::thread> threads {};
		size_t scratchBufferInitSize;

	public:

		explicit ChunkBakery(const size_t scratchBufferInitSize = 16384);

		ChunkBakery(const ChunkBakery&) = delete;
		ChunkBakery(ChunkBakery&&) = delete;

		ChunkBakery& operator =(const ChunkBakery&) = delete;
		ChunkBakery& operator =(ChunkBakery&&) = delete;

		~ChunkBakery();

		// TODO: add a method to enqueue multiple tasks at once???

		void enqueueTask(Chunk& chunk, const MeshingPriority priority, bool fluidOnly);

		size_t queuedTasks() const;

		static void runBakerThread(ChunkBakery* const bakery);
		static void bakeChunk(ChunkBakeData& chunkData, bool fluidOnly);
	};

}