#pragma once

#include "block/Block.h"

namespace eng {

	Color getLeavesColor(const glm::ivec3& blockPos);

	class BlockLeaves : public BasicBlock {
	public:

		BlockLeaves(const std::string& name) : BasicBlock(name) {}

		bool isFullOpaqueCube(const BlockState& blockState) const override {
			return false;
		}

		bool isFullCube(BlockStateRef blockState) const override {
			return true;
		}

		bool isSideSolid(BlockStateRef blockState, Direction side) const override {
			return false;
		}

		float getFluidCapactity(BlockStateRef blockState) const override {
			return 0.75f;
		}

		Color getBlockColor(const MeshingWorldView worldView, const BlockState& blockState, const glm::ivec3& blockPos, const uint8_t colorIndex) const override {
			return getLeavesColor(blockPos);
		}
		
	};

}