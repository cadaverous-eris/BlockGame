#pragma once

#include "block/Block.h"

namespace eng {

	class BlockAir final : public Block {
	public:

		BlockAir(const std::string& name) : Block(name, {}) {}

		bool isAir(const BlockState& blockState) const override {
			return true;
		}

		bool isFullOpaqueCube(const BlockState& blockState) const override {
			return false;
		}

		bool isFullCube(BlockStateRef blockState) const override {
			return false;
		}

		bool isSideSolid(BlockStateRef blockState, Direction side) const override {
			return false;
		}

		AxisAlignedBoxF getSelectionBox(const World& world, BlockStateRef blockState, const glm::ivec3& blockPos) const override {
			return null_aabb;
		}

		bool isReplaceable(BlockStateRef blockState) const override {
			return true;
		}

		float getFluidCapactity(BlockStateRef blockState) const override {
			return 1.0f;
		}

		bool canFluidFlowThroughFace(BlockStateRef blockState, Direction blockFace, const Fluid& fluid) const override {
			return true;
		}

		bool canCullAdjacentFace(BlockStateRef blockState, const Direction face, BlockStateRef adjacentBlockState) const override {
			return false;
		}
		bool canCullAdjacentFace(BlockStateRef blockState, const Direction face, const FluidState& adjacentFluidState) const override {
			return false;
		}

		bool hasModel(BlockStateRef blockState) const override {
			return false;
		}

		void addFaceQuadsToBuffer(BlockStateRef blockState, const glm::ivec3& blockPos, Direction face, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const override {}

	};

}