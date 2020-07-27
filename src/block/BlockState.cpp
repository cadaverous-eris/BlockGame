#include "BlockState.h"


#include "Block.h"
#include "BlockRegistry.h"
#include "fluid/Fluid.h"
#include "world/World.h"

namespace eng {

	BlockState::BlockState(BlockRef block, const metadata_t metadata) noexcept :
			BlockState(block.getId(), metadata) {}
	BlockState::BlockState(BlockRef block) :
			BlockState(block.getId(), block.getDefaultMetadata()) {}
	BlockState::BlockState() :
			BlockState(blocks::air.getId(), blocks::air.getDefaultMetadata()) {}

	BlockRef BlockState::getBlock() const {
		return block_registry[static_cast<id_t>(id)];
	}
	
	bool BlockState::isEmpty() const noexcept {
		return (id == blocks::air.getId());
	}

	// wrappers for block methods that take a BlockState parameter

	bool BlockState::isAir() const { return getBlock().isAir(*this); }
	bool BlockState::isFullOpaqueCube() const { return getBlock().isFullOpaqueCube(*this); }
	bool BlockState::isFullCube() const { return getBlock().isFullCube(*this); }
	bool BlockState::isSideSolid(const Direction side) const { return getBlock().isSideSolid(*this, side); }
	AxisAlignedBoxF BlockState::getSelectionBox(const World& world, const glm::ivec3& blockPos) const {
		return getBlock().getSelectionBox(world, *this, blockPos);
	}
	bool BlockState::isReplaceable() const { return getBlock().isReplaceable(*this); }
	void BlockState::onBlockUpdate(World& world, const glm::ivec3& blockPos, const Direction srcDirection) const {
		return getBlock().onBlockUpdate(world, *this, blockPos, srcDirection);
	}
	bool BlockState::canCullAdjacentFace(const Direction face, const BlockState& adjacentBlockState) const {
		return getBlock().canCullAdjacentFace(*this, face, adjacentBlockState);
	}
	bool BlockState::canCullAdjacentFace(const Direction face, const FluidState& adjacentFluidState) const {
		return getBlock().canCullAdjacentFace(*this, face, adjacentFluidState);
	}

	float BlockState::getFluidCapactity() const { return getBlock().getFluidCapactity(*this); }
	bool BlockState::canFluidFlowThroughFace(const Direction blockFace, const Fluid& fluid) const {
		return getBlock().canFluidFlowThroughFace(*this, blockFace, fluid);
	}

}