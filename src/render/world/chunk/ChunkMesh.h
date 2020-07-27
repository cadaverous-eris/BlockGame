#pragma once

#include <vector>
#include <array>

#include "render/world/RenderLayer.h"
#include "model/block/BlockQuad.h"


namespace eng {

	class Chunk;
	class ChunkBakery;

	class ChunkMesh {
		friend class Chunk;
		friend class ChunkBakery;
	public:
		using quad_list = std::vector<BlockQuad>;
		using layered_quad_list = std::array<quad_list, render_layer::layers.size()>;
	private:
		layered_quad_list layerQuads;
		std::array<size_t, render_layer::layers.size()> blockQuadCounts;

		static std::vector<quad_list> quadListPool;
	public:
		ChunkMesh() noexcept;
		ChunkMesh(const size_t opaqueQuads, const size_t cutoutQuads, const size_t transparentQuads);

		~ChunkMesh();

		ChunkMesh(const ChunkMesh& b);
		ChunkMesh(ChunkMesh&& b);

		ChunkMesh& operator =(const ChunkMesh& b);
		ChunkMesh& operator =(ChunkMesh&& b);

		//ChunkMesh(layered_quad_list&& layerQuads) : layerQuads(layerQuads) {}

		const quad_list& getQuads(const RenderLayer layer) const {
			return layerQuads[render_layer::getIndex(layer)];
		}
		quad_list& getQuads(const RenderLayer layer) {
			return layerQuads[render_layer::getIndex(layer)];
		}

		bool isEmpty(const RenderLayer layer) const {
			return getQuads(layer).empty();
		}
		bool isEmpty() const {
			return isEmpty(RenderLayer::Cutout) && isEmpty(RenderLayer::Opaque) && isEmpty(RenderLayer::Transparent);
		}

		size_t getLayerSize(const RenderLayer layer) const {
			return getQuads(layer).size();
		}
		size_t getLayerCapacity(const RenderLayer layer) const {
			return getQuads(layer).capacity();
		}

		void clearLayer(const RenderLayer layer) {
			getQuads(layer).clear();
		}
		void clear();

	private:
		static quad_list getPooledQuadList(const size_t size);
		static void poolQuadList(quad_list&);
	};

	inline std::vector<ChunkMesh::quad_list> ChunkMesh::quadListPool {};

}