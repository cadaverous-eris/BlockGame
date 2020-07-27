#pragma once

#include <cmath>
#include <vector>
#include <array>
#include <map>

#include <glm/common.hpp>

#include "BlockQuad.h"
#include "render/world/RenderLayer.h"
#include "render/world/chunk/MeshingWorldView.h"
#include "util/direction.h"

namespace eng {

	class BlockState;
	class TextureAtlas;
	
	class BlockModelLoader;

	class BakedBlockModel {
	private:
		static constexpr size_t quadListsCount = direction::direction_values.size() * render_layer::layers.size();
	public:
		struct QuadRange {
			//using iterator = typename std::vector<BlockQuad>::const_iterator;
			using iterator = const BlockQuad*;
			
			size_t rangeStart = 0, rangeLength = 0;

			inline iterator begin(const BakedBlockModel& model) const {
				//return model.modelQuads.begin() + rangeStart;
				return model.modelQuads.data() + rangeStart;
			}
			inline iterator end(const BakedBlockModel& model) const {
				//return model.modelQuads.begin() + (rangeStart + rangeLength);
				return model.modelQuads.data() + (rangeStart + rangeLength);
			}

			inline operator bool() const noexcept { return rangeLength > 0; }
		};
		struct FaceQuadList {
			QuadRange range = {}; // begin & end indices of quads for this face in this->quads;
			std::map<uint8_t, QuadRange> uncoloredRanges = {}; // map of color indices to begin & end indices of uncolored quads for this face in this->quads;
		};

		class Builder {
			friend class BakedBlockModel;
		public:
			struct BuilderFaceQuadList {
				std::vector<BlockQuad> quads;
				std::multimap<uint8_t, BlockQuad> uncoloredQuads;
			};
		private:
			std::array<BuilderFaceQuadList, quadListsCount> quadLists;
		public:
			Builder() = default;
			Builder& addQuad(const BlockQuad& quad, Direction face, RenderLayer layer) &;
			Builder& addColoredQuad(const BlockQuad& quad, Direction face, RenderLayer layer, uint8_t colorIndex) &;
			Builder&& addQuad(const BlockQuad& quad, Direction face, RenderLayer layer) &&;
			Builder&& addColoredQuad(const BlockQuad& quad, Direction face, RenderLayer layer, uint8_t colorIndex) &&;
			BakedBlockModel build() const;
		};

	private:
		std::vector<BlockQuad> modelQuads; // all quads in the model
		std::array<FaceQuadList, quadListsCount> quadLists;

	public:

		//BakedBlockModel() = default;

		explicit BakedBlockModel(const Builder&);

		void addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, const Direction face, const RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const;


		static BakedBlockModel createMissingModel(const TextureAtlas& textureAtlas);

	};

}