#pragma once

#include <array>

#include "block/Block.h"

namespace eng {

	class BlockSlab : public Block {
	public:
		struct SlabState {
			static constexpr Direction getOrientationDir(BlockState::metadata_t m) noexcept {
				m = m & 0b1111;
				if (m < 6u) return direction::fromIndex(m);
				if (m < 9u) return Direction::UNDEFINED;
				return direction::fromIndex(0);
			}
			static constexpr Axis getOrientationAxis(BlockState::metadata_t m) noexcept {
				m = m & 0b1111;
				return ((m >= 6u) && (m < 9u)) ? axis::fromIndex(m - 6u) : Axis::UNDEFINED;
			}
			static constexpr bool isDoubleSlab(const BlockState::metadata_t m) noexcept {
				return ((m & 0b10000) > 0);
			}
			BlockState::id_t blockId;
			Direction orientationDir = Direction::DOWN;
			Axis orientationAxis = Axis::UNDEFINED;
			bool doubleSlab = false;
			SlabState(BlockRef slab, const Direction orientationDir, const bool doubleSlab) noexcept :
					blockId(slab.getId()),
					orientationDir((orientationDir != Direction::UNDEFINED) ? orientationDir : Direction::DOWN),
					orientationAxis(Axis::UNDEFINED),
					doubleSlab(doubleSlab) {}
			SlabState(BlockRef slab, const Axis orientationAxis, const bool doubleSlab) noexcept :
					blockId(slab.getId()),
					orientationDir((orientationAxis == Axis::UNDEFINED) ? Direction::DOWN : Direction::UNDEFINED),
					orientationAxis(orientationAxis),
					doubleSlab(doubleSlab) {}
			SlabState(const BlockState& blockState) noexcept :
					blockId(blockState.getBlockId()),
					orientationDir(getOrientationDir(blockState.getMetadata())),
					orientationAxis(getOrientationAxis(blockState.getMetadata())),
					doubleSlab(isDoubleSlab(blockState.getMetadata())) {}
			operator BlockState() const noexcept {
				BlockState::metadata_t m = 0;
				if (orientationDir != Direction::UNDEFINED) m = direction::getIndex(orientationDir);
				else if (orientationAxis != Axis::UNDEFINED) m = axis::getIndex(orientationAxis) + 6u;
				if (doubleSlab) m |= 0b10000;
				return { blockId, m };
			}
		};
	protected:
		mutable std::array<std::unique_ptr<BakedBlockModel>, 9> models;

		const BlockState fullBlockState;
		BlockRef fullBlock;

	public:

		static const AxisAlignedBoxF slab_down_aabb;
		static const AxisAlignedBoxF slab_up_aabb;
		static const AxisAlignedBoxF slab_north_aabb;
		static const AxisAlignedBoxF slab_south_aabb;
		static const AxisAlignedBoxF slab_west_aabb;
		static const AxisAlignedBoxF slab_east_aabb;
		static const AxisAlignedBoxF slab_y_aabb;
		static const AxisAlignedBoxF slab_x_aabb;
		static const AxisAlignedBoxF slab_z_aabb;

		BlockSlab(const std::string& name, const BlockState& fullBlockState) :
			Block(name, { "block/" + name + "_down", "block/" + name + "_y" }),
			fullBlockState(fullBlockState), fullBlock(fullBlockState.getBlock()) {}
		BlockSlab(const BlockState& fullBlockState) :
			BlockSlab(fullBlockState.getBlock().getName() + "_slab", fullBlockState) {}
		BlockSlab(const std::string& name, BlockRef fullBlock) :
				BlockSlab(name, fullBlock.createBlockState()) {}
		BlockSlab(BlockRef fullBlock) :
				BlockSlab(fullBlock.getName() + "_slab", fullBlock.createBlockState()) {}



		bool isFullOpaqueCube(const BlockState& blockState) const override {
			return SlabState(blockState).doubleSlab && fullBlock.isFullOpaqueCube(fullBlockState);
		}

		bool isFullCube(BlockStateRef blockState) const override {
			return SlabState(blockState).doubleSlab && fullBlock.isFullCube(fullBlockState);
		}

		bool isSideSolid(BlockStateRef blockState, Direction side) const override;

		BlockState getStateForPlacement(const World& world, const glm::ivec3& blockPos, const RayCastResultF& rayCastResult/*, const EntityPlayer* const player*/) const override;

		AxisAlignedBoxF getSelectionBox(const World& world, const BlockState& blockState, const glm::ivec3& blockPos) const override;

		bool canCullAdjacentFace(BlockStateRef blockState, Direction face, BlockStateRef adjacentBlockState) const override;
		bool canCullAdjacentFace(BlockStateRef blockState, Direction face, const FluidState& adjacentFluidState) const override;

		float getFluidCapactity(BlockStateRef blockState) const override;

		bool canFluidFlowThroughFace(BlockStateRef blockState, Direction blockFace, const Fluid& fluid) const override;

		Color getBlockColor(const MeshingWorldView worldView, const BlockState& blockState, const glm::ivec3& blockPos, const uint8_t colorIndex) const override {
			return fullBlock.getBlockColor(worldView, fullBlockState, blockPos, colorIndex);
		}

		void onResourceLoadComplete() const override;

		void addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, Direction face, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const override;

	};

}