#include "Block.h"

#include "BlockRegistry.h"
#include "util/resources/ResourceManager.h"
#include "world/World.h"
#include "fluid/FluidState.h"

namespace eng {

	void Block::registerBlock() const {
		block_registry.registerObject(*this);
	}

	BlockState Block::getStateForPlacement(const World& world, const glm::ivec3& blockPos, const RayCastResultF& rayCastResult/*, const EntityPlayer* const player*/) const {
		return createBlockState();
	}

	AxisAlignedBoxF Block::getSelectionBox(const World& world, BlockStateRef blockState, const glm::ivec3& blockPos) const {
		return full_aabb;
	}

	bool Block::canPlaceBlock(const World& world, const glm::ivec3& blockPos, const Direction face) const {
		return true;
	}

	void Block::onBlockUpdate(World& world, BlockStateRef blockState, const glm::ivec3& blockPos, const Direction srcDirection) const {
	}

	bool Block::canCullAdjacentFace(BlockStateRef blockState, const Direction face, const FluidState& adjacentFluidState) const {
		return isFullOpaqueCube(blockState);
	}


	
	BasicBlock::BasicBlock(const std::string& name) : Block(name) {}

	void BasicBlock::onResourceLoadComplete() const {
		model = std::make_unique<BakedBlockModel>(ResourceManager::instance().getBlockModels().createBakedModel("block/" + getName()));
	}

	void BasicBlock::addFaceQuadsToBuffer(BlockStateRef blockState, const glm::ivec3& blockPos, Direction face, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const {
		model->addFaceQuadsToBuffer(blockState, blockPos, face, layer, offset, worldView, buffer);
	}

}