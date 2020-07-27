#pragma once

#include "block/Block.h"

namespace eng {

	class BlockBasicPlant : public BasicBlock {
	public:
		constexpr inline static AxisAlignedBox<float> plant_aabb {
			{ 0.14645f, 0.0f, 0.14645f },
			{ 0.85355f, 0.375f, 0.85355f }
		};

		BlockBasicPlant(const std::string& name) : BasicBlock(name) {}

		bool isFullOpaqueCube(BlockStateRef blockState) const override {
			return false;
		}

		bool isFullCube(BlockStateRef blockState) const override {
			return false;
		}

		bool isSideSolid(BlockStateRef blockState, Direction side) const override {
			return false;
		}

		AxisAlignedBoxF getSelectionBox(const World& world, BlockStateRef blockState, const glm::ivec3& blockPos) const override;

		virtual bool isValidSoil(const World& world, const glm::ivec3& soilPos, BlockStateRef soilState) const;

		bool canPlaceBlock(const World& world, const glm::ivec3& blockPos, Direction face) const override;

		void onBlockUpdate(World& world, BlockStateRef blockState, const glm::ivec3& blockPos, Direction srcDir) const override;

	};

}