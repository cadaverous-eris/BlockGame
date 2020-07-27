#include "BlockSlab.h"

#include <glm/vec2.hpp>
#include <glm/common.hpp>

#include "input/KeyBind.h"
#include "util/resources/ResourceManager.h"
#include "world/World.h"
#include "fluid/FluidState.h"

namespace eng {

	const AxisAlignedBoxF BlockSlab::slab_down_aabb { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.5f, 1.0f } };
	const AxisAlignedBoxF BlockSlab::slab_up_aabb = slab_down_aabb.getRotated(Axis::X, 2, { 0.5f, 0.5f, 0.5f });
	const AxisAlignedBoxF BlockSlab::slab_north_aabb = slab_down_aabb.getRotated(Axis::X, 1, { 0.5f, 0.5f, 0.5f });
	const AxisAlignedBoxF BlockSlab::slab_south_aabb = slab_down_aabb.getRotated(Axis::X, -1, { 0.5f, 0.5f, 0.5f });
	const AxisAlignedBoxF BlockSlab::slab_west_aabb = slab_down_aabb.getRotated(Axis::Z, -1, { 0.5f, 0.5f, 0.5f });
	const AxisAlignedBoxF BlockSlab::slab_east_aabb = slab_down_aabb.getRotated(Axis::Z, 1, { 0.5f, 0.5f, 0.5f });
	const AxisAlignedBoxF BlockSlab::slab_y_aabb = slab_down_aabb + glm::vec3(0.0f, 0.25f, 0.0f);
	const AxisAlignedBoxF BlockSlab::slab_x_aabb = slab_y_aabb.getRotated(Axis::Z, 1, { 0.5f, 0.5f, 0.5f });
	const AxisAlignedBoxF BlockSlab::slab_z_aabb = slab_y_aabb.getRotated(Axis::X, 1, { 0.5f, 0.5f, 0.5f });

	BlockState BlockSlab::getStateForPlacement(const World& world, const glm::ivec3& blockPos, const RayCastResultF& rayCastResult/*, const EntityPlayer* const player*/) const {
		const Direction face = rayCastResult.face;
		if (face != Direction::UNDEFINED) {
			Direction placementDir = direction::getOpposite(face);
			const Axis axis = direction::getAxis(face);

			const auto hitPos = rayCastResult.position - static_cast<glm::vec3>(rayCastResult.blockPos);
			const auto hitOffset = hitPos - glm::vec3(0.5f, 0.5f, 0.5f);
			const auto absHitOffset = glm::abs(hitOffset);

			const bool placeCenter = input::SNEAK->isPressed();

			switch (axis) {
			case Axis::X:
				if ((absHitOffset.y > 0.25f) || (absHitOffset.z > 0.25f)) {
					if (absHitOffset.y >= absHitOffset.z)
						placementDir = (hitOffset.y < 0) ? Direction::DOWN : Direction::UP;
					else
						placementDir = (hitOffset.z < 0) ? Direction::NORTH : Direction::SOUTH;
					if (placeCenter)
						placementDir = direction::rotateAround(placementDir, axis);
				}
				break;
			case Axis::Y:
				if ((absHitOffset.x > 0.25f) || (absHitOffset.z > 0.25f)) {
					if (absHitOffset.x >= absHitOffset.z)
						placementDir = (hitOffset.x < 0) ? Direction::WEST : Direction::EAST;
					else
						placementDir = (hitOffset.z < 0) ? Direction::NORTH : Direction::SOUTH;
					if (placeCenter)
						placementDir = direction::rotateAround(placementDir, axis);
				}
				break;
			case Axis::Z:
				if ((absHitOffset.x > 0.25f) || (absHitOffset.y > 0.25f)) {
					if (absHitOffset.x >= absHitOffset.y)
						placementDir = (hitOffset.x < 0) ? Direction::WEST : Direction::EAST;
					else
						placementDir = (hitOffset.y < 0) ? Direction::DOWN : Direction::UP;
					if (placeCenter)
						placementDir = direction::rotateAround(placementDir, axis);
				}
				break;
			default:
				break;
			}

			if (placeCenter) {
				return SlabState(*this, direction::getAxis(placementDir), false);
			} else {
				return SlabState(*this, placementDir, false);
			}
		}
		return createBlockState();
	}

	bool BlockSlab::isSideSolid(BlockStateRef blockState, Direction side) const {
		const SlabState slabState(blockState);
		if (slabState.doubleSlab || (slabState.orientationDir == side))
			return fullBlock.isSideSolid(fullBlockState, side);
		return false;
	}

	AxisAlignedBoxF BlockSlab::getSelectionBox(const World& world, const BlockState& blockState, const glm::ivec3& blockPos) const {
		const SlabState slabState(blockState);
		if (slabState.doubleSlab) return full_aabb;
		if (slabState.orientationDir == Direction::UP) return slab_up_aabb;
		if (slabState.orientationDir == Direction::NORTH) return slab_north_aabb;
		if (slabState.orientationDir == Direction::SOUTH) return slab_south_aabb;
		if (slabState.orientationDir == Direction::WEST) return slab_west_aabb;
		if (slabState.orientationDir == Direction::EAST) return slab_east_aabb;
		if (slabState.orientationAxis == Axis::X) return slab_x_aabb;
		if (slabState.orientationAxis == Axis::Y) return slab_y_aabb;
		if (slabState.orientationAxis == Axis::Z) return slab_z_aabb;
		return slab_down_aabb;
	}

	bool BlockSlab::canCullAdjacentFace(BlockStateRef blockState, const Direction face, BlockStateRef adjacentBlockState) const {
		const SlabState slabState(blockState);
		if (slabState.doubleSlab || (slabState.orientationDir == face))
			return fullBlock.canCullAdjacentFace(fullBlockState, face, adjacentBlockState);
		return false;
	}
	bool BlockSlab::canCullAdjacentFace(BlockStateRef blockState, const Direction face, const FluidState& adjacentFluidState) const {
		const SlabState slabState(blockState);
		if (slabState.doubleSlab || (slabState.orientationDir == face))
			return fullBlock.canCullAdjacentFace(fullBlockState, face, adjacentFluidState);
		return false;
	}

	float BlockSlab::getFluidCapactity(BlockStateRef blockState) const {
		const SlabState slabState(blockState);
		const auto fullBlockCapacity = fullBlock.getFluidCapactity(fullBlockState);
		return (slabState.doubleSlab) ? fullBlockCapacity : (0.5f + (fullBlockCapacity * 0.5f));
	}

	bool BlockSlab::canFluidFlowThroughFace(BlockStateRef blockState, const Direction blockFace, const Fluid& fluid) const {
		const SlabState slabState(blockState);
		if (slabState.doubleSlab)
			return fullBlock.canFluidFlowThroughFace(fullBlockState, blockFace, fluid);
		if (slabState.orientationAxis != Axis::UNDEFINED)
			return (direction::getAxis(blockFace) != slabState.orientationAxis);
		return (slabState.orientationDir != blockFace);
	}

	void BlockSlab::onResourceLoadComplete() const {
		const auto& textureAtlas = ResourceManager::instance().getBlockTextures();
		const auto& modelDown = ResourceManager::instance().getBlockModels().getModel("block/" + getName() + "_down");
		const auto& modelY = ResourceManager::instance().getBlockModels().getModel("block/" + getName() + "_y");
		models[0] = std::make_unique<BakedBlockModel>(modelDown.bake(textureAtlas));
		models[1] = std::make_unique<BakedBlockModel>(modelDown.getRotated(Axis::X, 2).bake(textureAtlas));
		models[2] = std::make_unique<BakedBlockModel>(modelDown.getRotated(Axis::X, 1).bake(textureAtlas));
		models[3] = std::make_unique<BakedBlockModel>(modelDown.getRotated(Axis::X, -1).bake(textureAtlas));
		models[4] = std::make_unique<BakedBlockModel>(modelDown.getRotated(Axis::Z, -1).bake(textureAtlas));
		models[5] = std::make_unique<BakedBlockModel>(modelDown.getRotated(Axis::Z, 1).bake(textureAtlas));
		models[6] = std::make_unique<BakedBlockModel>(modelY.getRotated(Axis::Z, 1).bake(textureAtlas));
		models[7] = std::make_unique<BakedBlockModel>(modelY.bake(textureAtlas));
		models[8] = std::make_unique<BakedBlockModel>(modelY.getRotated(Axis::X, 1).bake(textureAtlas));
	}

	void BlockSlab::addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, Direction face, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const {
		const SlabState slabState(blockState);
		if (slabState.doubleSlab) {
			fullBlock.addFaceQuadsToBuffer(fullBlockState, blockPos, face, layer, offset, worldView, buffer);
		} else if (slabState.orientationDir != Direction::UNDEFINED) {
			models[direction::getIndex(slabState.orientationDir)]->addFaceQuadsToBuffer(blockState, blockPos, face, layer, offset, worldView, buffer);
		} else if (slabState.orientationAxis != Axis::UNDEFINED) {
			models[axis::getIndex(slabState.orientationAxis) + 6u]->addFaceQuadsToBuffer(blockState, blockPos, face, layer, offset, worldView, buffer);
		} else {
			models[0]->addFaceQuadsToBuffer(blockState, blockPos, face, layer, offset, worldView, buffer);
		}
	}

}