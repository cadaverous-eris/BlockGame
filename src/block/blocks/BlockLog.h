#pragma once

#include <array>
#include <iostream>

#include "block/Block.h"
#include "util/direction.h"
#include "util/resources/ResourceManager.h"
#include "model/block/BlockModel.h"

namespace eng {

	class BlockLog : public Block {
	protected:

		mutable std::array<std::unique_ptr<BakedBlockModel>, axis::axes.size()> models;

	public:

		BlockLog(const std::string& name) : Block(name) {}
		
		metadata_t getDefaultMetadata() const override {
			return axis::getIndex(Axis::Y);
		}

		static Axis getAxis(const BlockState& blockState) {
			const auto metadata = blockState.getMetadata();
			if (metadata < axis::axes.size())
				return axis::fromIndex(metadata);
			return Axis::Y; // default to Y
		}

		BlockState getStateForPlacement(const World&, const glm::ivec3& blockPos, const RayCastResultF& rayCastResult/*, const EntityPlayer* const player*/) const override;

		void onResourceLoadComplete() const override {
			const BlockModel& logModel = ResourceManager::instance().getBlockModels().getModel("block/" + getName());
			const TextureAtlas& textureAtlas = ResourceManager::instance().getBlockTextures();
			models[0] = std::make_unique<BakedBlockModel>(logModel.getRotated(Axis::Z, 1).bake(textureAtlas));
			models[1] = std::make_unique<BakedBlockModel>(logModel.bake(textureAtlas));
			models[2] = std::make_unique<BakedBlockModel>(logModel.getRotated(Axis::X, 1).bake(textureAtlas));
		}

		void addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, Direction face, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const override {
			const auto metadata = blockState.getMetadata();
			if (metadata < models.size())
				models[metadata]->addFaceQuadsToBuffer(blockState, blockPos, face, layer, offset, worldView, buffer);
			else
				models[axis::getIndex(Axis::Y)]->addFaceQuadsToBuffer(blockState, blockPos, face, layer, offset, worldView, buffer); // default to Y
		}

		
	};

}