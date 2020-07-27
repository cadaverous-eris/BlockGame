#pragma once

#include "block/Block.h"

namespace eng {

	class BlockGlass : public BasicBlock {
	public:
		
		BlockGlass(const std::string& name) : BasicBlock(name) {}

		bool isFullOpaqueCube(const BlockState& blockState) const override {
			return false;
		}

		bool isFullCube(BlockStateRef blockState) const override {
			return true;
		}

		bool canCullAdjacentFace(const BlockState& blockState, Direction face, const BlockState& adjacentBlockState) const override {
			return adjacentBlockState.getBlock() == *this;
		}


	};

}