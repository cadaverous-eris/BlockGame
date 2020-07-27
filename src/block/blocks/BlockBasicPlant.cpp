#include "BlockBasicPlant.h"

#include "block/BlockRegistry.h"
#include "world/World.h"

namespace eng {

	AxisAlignedBoxF BlockBasicPlant::getSelectionBox(const World& world, const BlockState& blockState, const glm::ivec3& blockPos) const {
		return plant_aabb;
	}

	bool BlockBasicPlant::isValidSoil(const World& world, const glm::ivec3& soilPos, const BlockState& soilState) const {
		BlockRef soil = soilState.getBlock();
		return (soil == blocks::dirt) || (soil == blocks::grass) || (soil == blocks::gravel) || (soil == blocks::sand);
	}

	bool BlockBasicPlant::canPlaceBlock(const World& world, const glm::ivec3& blockPos, const Direction face) const {
		const auto soilPos = direction::offsetVector(blockPos, Direction::DOWN);
		const BlockState soilState = world.getBlockState(soilPos);
		return isValidSoil(world, soilPos, soilState);
	}

	void BlockBasicPlant::onBlockUpdate(World& world, const BlockState& blockState, const glm::ivec3& blockPos, const Direction srcDir) const {
		const auto soilPos = direction::offsetVector(blockPos, Direction::DOWN);
		if (world.isBlockLoaded(soilPos)) {
			const BlockState& soilState = world.getBlockState(soilPos);
			if (!isValidSoil(world, soilPos, soilState))
				world.setBlockState(blockPos, {}, true, true, true);
		}
	}

}