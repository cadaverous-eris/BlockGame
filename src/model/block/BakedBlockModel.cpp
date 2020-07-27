#include "BakedBlockModel.h"

#include <utility>

#include "block/Block.h"
#include "util/resources/TextureAtlas.h"
#include "BlockModel.h"

namespace eng {

	void BakedBlockModel::addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, const Direction face, const RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const {
		const auto listIndex = direction::getIndex(face) + (render_layer::getIndex(layer) * direction::direction_values.size());
		const auto& quadList = quadLists[listIndex];
		if (quadList.range) {
			const auto end = quadList.range.end(*this);
			for (auto it = quadList.range.begin(*this); it != end; it++) {
				buffer.push_back((*it) + offset);
			}
		}
		if (!quadList.uncoloredRanges.empty()) {
			for (const auto& [colorIndex, range] : quadList.uncoloredRanges) {
				const auto color = blockState.getBlock().getBlockColor(worldView, blockState, blockPos, colorIndex);
				const auto end = range.end(*this);
				for (auto it = range.begin(*this); it != end; it++) {
					buffer.push_back(((*it) * color) + offset);
				}
			}
		}
	}

	BakedBlockModel::BakedBlockModel(const Builder& builder) : modelQuads(), quadLists() {
		for (size_t i = 0; i < quadListsCount; i++) {
			{
				const size_t rangeStart = modelQuads.size();
				modelQuads.insert(modelQuads.end(), builder.quadLists[i].quads.begin(), builder.quadLists[i].quads.end());
				const size_t rangeSize = modelQuads.size() - rangeStart;
				quadLists[i].range = { rangeStart, rangeSize };
			}
			const auto& builderUncoloredQuads = builder.quadLists[i].uncoloredQuads;
			for (auto it = builderUncoloredQuads.begin(); it != builderUncoloredQuads.end();) {
				const uint8_t colorIndex = it->first;
				const auto endIt = builderUncoloredQuads.upper_bound(colorIndex);
				const size_t rangeStart = modelQuads.size();
				for (auto quadIt = it; quadIt != endIt; quadIt++)
					modelQuads.push_back(quadIt->second);
				const auto rangeSize = modelQuads.size() - rangeStart;
				quadLists[i].uncoloredRanges[colorIndex] = { rangeStart, rangeSize };
				it = endIt;
			}
		}
	}

	BakedBlockModel BakedBlockModel::Builder::build() const {
		return BakedBlockModel(*this);
	}

	BakedBlockModel::Builder& BakedBlockModel::Builder::addQuad(const BlockQuad& quad, const Direction face, const RenderLayer layer) & {
		const auto listIndex = direction::getIndex(face) + (render_layer::getIndex(layer) * direction::direction_values.size());
		quadLists[listIndex].quads.push_back(quad);
		return *this;
	}
	BakedBlockModel::Builder& BakedBlockModel::Builder::addColoredQuad(const BlockQuad& quad, const Direction face, const RenderLayer layer, const uint8_t colorIndex) & {
		const auto listIndex = direction::getIndex(face) + (render_layer::getIndex(layer) * direction::direction_values.size());
		quadLists[listIndex].uncoloredQuads.emplace(colorIndex, quad);
		return *this;
	}
	BakedBlockModel::Builder&& BakedBlockModel::Builder::addQuad(const BlockQuad& quad, const Direction face, const RenderLayer layer) && {
		const auto listIndex = direction::getIndex(face) + (render_layer::getIndex(layer) * direction::direction_values.size());
		quadLists[listIndex].quads.push_back(quad);
		return std::move(*this);
	}
	BakedBlockModel::Builder&& BakedBlockModel::Builder::addColoredQuad(const BlockQuad& quad, const Direction face, const RenderLayer layer, const uint8_t colorIndex) && {
		const auto listIndex = direction::getIndex(face) + (render_layer::getIndex(layer) * direction::direction_values.size());
		quadLists[listIndex].uncoloredQuads.emplace(colorIndex, quad);
		return std::move(*this);
	}


	BakedBlockModel BakedBlockModel::createMissingModel(const TextureAtlas& textureAtlas) {
		return BlockModel::missing_model.bake(textureAtlas);
	}

}