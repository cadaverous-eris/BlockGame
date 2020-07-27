#pragma once

#include <array>

#include "BlockBasicPlant.h"
#include "block/Block.h"

namespace eng {

	/*class GrassBlockModel : public BlockModel {
	private:
		std::array<BlockQuad, 14> quads;

	public:
		GrassBlockModel(size_t texIndexD, size_t texIndexU, size_t texIndexNSWE);

		void addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, const Direction face, const RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const override;
	};*/

	Color getGrassColor(const glm::ivec3& blockPos);

	class BlockGrass : public BasicBlock {
	public:
		
		BlockGrass(const std::string& name) : BasicBlock(name) {}

		Color getBlockColor(const MeshingWorldView worldView, const BlockState& blockState, const glm::ivec3& blockPos, const uint8_t colorIndex) const override {
			return getGrassColor(blockPos);
		}

	};

	class BlockTallGrass : public BlockBasicPlant {
	public:
		
		BlockTallGrass(const std::string& name) : BlockBasicPlant(name) {}

		bool isReplaceable(BlockStateRef blockState) const override {
			return true;
		}

		Color getBlockColor(const MeshingWorldView worldView, const BlockState& blockState, const glm::ivec3& blockPos, const uint8_t colorIndex) const override {
			return getGrassColor(blockPos);
		}

	};

}