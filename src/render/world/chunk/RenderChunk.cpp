#include "RenderChunk.h"

#include "model/block/BlockQuad.h"

namespace eng {

	void RenderChunk::init() {
		for (auto layer : render_layer::layers) {
			const size_t layerIndex = render_layer::getIndex(layer);
			// Block VAO and VBO setup
			blockVAOs[layerIndex].bind();
			blockVBOs[layerIndex].setData(nullptr, 0, VertexBuffer::DrawHint::DYNAMIC);
			blockVAOs[layerIndex].setVertexFormat(BlockQuad::format);
		}
	}

	RenderChunk::RenderChunk(const ChunkCoord& chunkCoord, const glm::ivec3& blockPos) :
			chunkCoord(chunkCoord), blockPos(blockPos) {
		init();
	}

	RenderChunk::RenderChunk(const ChunkCoord& chunkCoord) :
			chunkCoord(chunkCoord), blockPos(chunkCoord.getBlockPos()) {
		init();
	}

	void RenderChunk::syncMesh() const {
		std::scoped_lock<std::mutex> meshLock { meshMutex };
		for (auto layer : render_layer::layers) {
			const size_t layerIndex = render_layer::getIndex(layer);
			const auto& meshQuads = mesh.getQuads(layer);
			blockVAOs[layerIndex].bind();
			blockVBOs[layerIndex].setData(std::span(meshQuads.data(), meshQuads.size()));
			//mesh.getQuads(layer) = std::vector<BlockQuad> {}; // why did I do this???
		}
		dirty = false;
	}

	void RenderChunk::preRender() const {
		if (dirty) syncMesh();
	}

	bool RenderChunk::shouldDrawLayer(const RenderLayer layer) const {
		const size_t layerIndex = render_layer::getIndex(layer);
		return (blockVBOs[layerIndex].getSize() / sizeof(BlockQuad)) > 0;
	}

	void RenderChunk::drawLayer(const RenderLayer layer) const {
		const size_t layerIndex = render_layer::getIndex(layer);
		const auto numQuads = blockVBOs[layerIndex].getSize() / sizeof(BlockQuad);
		if (numQuads > 0)
			blockVAOs[layerIndex].draw(DrawMode::POINTS, 0, numQuads);
	}

}