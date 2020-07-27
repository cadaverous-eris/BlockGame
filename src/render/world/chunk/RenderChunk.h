#pragma once

#include <vector>
#include <mutex>

#include "world/chunk/ChunkCoord.h"
#include "ChunkMesh.h"
#include "render/world/RenderLayer.h"
#include "render/VertexArray.h"
#include "render/VertexBuffer.h"

namespace eng {

	class RenderChunk {
		mutable bool dirty { false };
		mutable std::mutex meshMutex {};
		mutable ChunkMesh mesh;

		std::array<VertexArray, render_layer::layers.size()> blockVAOs;
		mutable std::array<VertexBuffer, render_layer::layers.size()> blockVBOs;

		eng::ChunkCoord chunkCoord;
		glm::ivec3 blockPos;

	public:

		RenderChunk(const ChunkCoord& chunkCoord, const glm::ivec3& blockPos);
		RenderChunk(const ChunkCoord& chunkCoord);

		inline ChunkMesh& getMesh() { return mesh; }

		inline std::mutex& getMeshMutex() { return meshMutex; }

		// should be called before rendering, keeps mesh updated on GPU
		void preRender() const;

		bool shouldDrawLayer(const RenderLayer layer) const;

		void drawLayer(const RenderLayer layer) const;

		inline void markDirty() const { dirty = true; }

	private:
		void init();

		void syncMesh() const;

	};

}