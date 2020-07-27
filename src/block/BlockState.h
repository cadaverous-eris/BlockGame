#pragma once

#include <cstdint>

#include <glm/vec3.hpp>

#include "util/direction.h"
#include "util/math/AxisAlignedBox.h"

namespace eng {
	
	class Block;
	// alias for convenience since Blocks are almost always used as const references
	using BlockRef = const Block&;

	class World;
	class Fluid;
	class FluidState;

	class BlockState {
	public:
		using id_t = uint16_t;
		using metadata_t = uint64_t; // only lower 48 bits are used

		using state_data_t = uint64_t;
	private:
		state_data_t id : 16;
		state_data_t metadata : 48;
	public:
		BlockState(const id_t id, const metadata_t metadata) noexcept : id(id), metadata(metadata) {}
		BlockState(BlockRef block, const metadata_t metadata) noexcept;
		BlockState(BlockRef block);
		BlockState();

		BlockRef getBlock() const;
		inline id_t getBlockId() const noexcept { return static_cast<id_t>(id); }
		inline metadata_t getMetadata() const noexcept { return static_cast<metadata_t>(metadata); }

		inline BlockState withMetadata(const metadata_t metadata) const noexcept {
			return { static_cast<id_t>(id), static_cast<metadata_t>(metadata) };
		}

		inline bool operator ==(const BlockState& b) const noexcept {
			return (id == b.id) && (metadata == b.metadata); // check equality by block id && metadata
		}
		inline bool operator !=(const BlockState& b) const noexcept {
			return (id != b.id) || (metadata != b.metadata); // check equality by block id && metadata
		}

		bool isEmpty() const noexcept;
		inline operator bool() const noexcept { return !isEmpty(); }

		// TODO: more wrapper functions for Block methods that take a BlockState???

		bool isAir() const;
		bool isFullOpaqueCube() const;
		bool isFullCube() const;
		bool isSideSolid(Direction side) const;
		AxisAlignedBoxF getSelectionBox(const World& world, const glm::ivec3& blockPos) const;
		bool isReplaceable() const;
		void onBlockUpdate(World& world, const glm::ivec3& blockPos, Direction srcDirection) const;
		bool canCullAdjacentFace(const Direction face, const BlockState& adjacentBlockState) const;
		bool canCullAdjacentFace(const Direction face, const FluidState& adjacentFluidState) const;

		float getFluidCapactity() const;
		bool canFluidFlowThroughFace(Direction blockFace, const Fluid& fluid) const;

	};

	// alias for convenience since const BlockState& function parameters are so commonly used
	using BlockStateRef = const BlockState&;

}