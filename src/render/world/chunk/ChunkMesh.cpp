#include "ChunkMesh.h"

#include <type_traits>
#include <utility>
#include <mutex>

namespace eng {

	static std::mutex poolMutex;

	// extract a quad list from the pool
	[[nodiscard]] typename ChunkMesh::quad_list ChunkMesh::getPooledQuadList(const size_t size) {
		std::scoped_lock<std::mutex> poolLock { poolMutex };
		if (size > 0 && !quadListPool.empty()) {
			// TODO: take requested size into account?
			quad_list& pooledList = quadListPool.back();
			quad_list quadList { std::move(pooledList) }; // move the last list out of the pool
			//pooledList.clear(); // return the pooled list that was moved from to a known state
			quadListPool.pop_back(); // remove the list that was moved from the pool
			return quadList;
		}
		return {};
	}
	// adds an existing quad list to the pool (replaces the argument with an empty quad list)
	void ChunkMesh::poolQuadList(quad_list& l) {
		if (l.capacity() > 0) {
			l.clear();
			std::scoped_lock<std::mutex> poolLock { poolMutex };
			quad_list& pooledList = quadListPool.emplace_back();
			std::swap(l, pooledList);
		}
	}


	ChunkMesh::ChunkMesh() noexcept {}
	ChunkMesh::ChunkMesh(const size_t opaqueQuads, const size_t cutoutQuads, const size_t transparentQuads) :
			layerQuads{ getPooledQuadList(opaqueQuads), getPooledQuadList(cutoutQuads), getPooledQuadList(transparentQuads) } {}

	ChunkMesh::~ChunkMesh() {
		for (auto& quadList : layerQuads)
			poolQuadList(quadList);
	}

	ChunkMesh::ChunkMesh(const ChunkMesh& b) : ChunkMesh(
		b.getLayerSize(render_layer::layers[0]), // get quad lists from pool
		b.getLayerSize(render_layer::layers[1]),
		b.getLayerSize(render_layer::layers[2])
	) {
		for (const auto layer : render_layer::layers) {
			auto& l = getQuads(layer), bl = b.getQuads(layer);
			l.insert(l.begin(), bl.begin(), bl.end()); // copy contents of quad lists
		}
	}
	ChunkMesh::ChunkMesh(ChunkMesh&& b) : ChunkMesh() {
		std::swap(this->layerQuads, b.layerQuads);
	}

	ChunkMesh& ChunkMesh::operator =(const ChunkMesh& b) {
		if (&b != this) {
			for (const auto layer : render_layer::layers) {
				auto& l = getQuads(layer), bl = b.getQuads(layer);
				if (bl.empty()) {
					poolQuadList(l);
				} else {
					if (l.capacity() == 0)
						l = getPooledQuadList(bl.size()); // get a quad list from the pool if necessary
					l.insert(l.begin(), bl.begin(), bl.end()); // copy contents of quad lists
				}
			}
		}
		return *this;
	}
	ChunkMesh& ChunkMesh::operator =(ChunkMesh&& b) {
		if (&b != this)
			std::swap(this->layerQuads, b.layerQuads);
		return *this;
	}
	
	void ChunkMesh::clear() {
		for (auto& l : layerQuads)
			l.clear();
	}

}