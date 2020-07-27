#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <memory>
#include <utility>
#include <stdexcept>

#include <glm/vec3.hpp>

#include "util/Registry.h"
#include "util/type_traits.h"
#include "BlockState.h"
#include "util/direction.h"
#include "util/math/AxisAlignedBox.h"
#include "util/Color.h"
#include "util/RayCastResult.h"
#include "render/world/RenderLayer.h"
#include "model/block/BlockQuad.h"
#include "render/world/chunk/MeshingWorldView.h"
#include "model/block/BakedBlockModel.h"

namespace eng {

	class World;
	class Fluid;
	class FluidState;

	class Block {
		friend class Registry<Block>;
	public:
		using id_t = BlockState::id_t;
		using metadata_t = BlockState::metadata_t;

		constexpr inline static AxisAlignedBoxF full_aabb { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
		constexpr inline static AxisAlignedBoxF null_aabb {};

	private:
		const std::string name;
		mutable id_t id;
	protected:
		std::vector<std::string> modelNames;

	public:
		Block(const std::string& name) :
				name(name),
				modelNames(std::initializer_list<std::string>{ std::string("block/") + name }) {
			registerBlock();
		}

	protected:
		Block(const std::string& name, const std::vector<std::string>& modelNames) :
				name(name), modelNames(modelNames) {
			registerBlock();
		}

	public:
		Block(const Block&) = delete;
		Block& operator =(const Block&) = delete;
		Block(Block&&) = delete;
		Block& operator =(Block&&) = delete;

		virtual ~Block() = default;
		
		id_t getId() const noexcept { return id; }
		const std::string& getName() const noexcept { return name; }

		bool operator ==(BlockRef b) const noexcept {
			return id == b.id; // check equality by block id
		}
		bool operator !=(BlockRef b) const noexcept {
			return id != b.id; // check equality by block id
		}

		BlockState createBlockState() const {
			return { id, getDefaultMetadata() };
		}
		BlockState createBlockState(const metadata_t metadata) const noexcept {
			return { id, metadata };
		}

		// BLOCK METHODS

		virtual bool isAir(BlockStateRef blockState) const {
			return false;
		}
		
		virtual metadata_t getDefaultMetadata() const {
			return 0;
		}
		
		virtual bool isFullOpaqueCube(BlockStateRef blockState) const {
			return true;
		}

		virtual bool isFullCube(BlockStateRef blockState) const {
			return isFullOpaqueCube(blockState);
		}

		virtual bool isSideSolid(BlockStateRef blockState, Direction side) const {
			return isFullCube(blockState);
		}
		
		virtual BlockState getStateForPlacement(const World& world, const glm::ivec3& blockPos, const RayCastResultF& rayCastResult/*, const EntityPlayer* const player*/) const;

		virtual AxisAlignedBoxF getSelectionBox(const World& world, BlockStateRef, const glm::ivec3& blockPos) const;

		virtual bool isReplaceable(BlockStateRef blockState) const {
			return false;
		}

		virtual bool canPlaceBlock(const World& world, const glm::ivec3& blockPos, const Direction face) const;

		virtual void onBlockUpdate(World& world, BlockStateRef blockState, const glm::ivec3& blockPos, Direction srcDirection) const;

		virtual float getFluidCapactity(BlockStateRef blockState) const {
			return isFullCube(blockState) ? 0.0f : 1.0f;//getFluidBox(blockState).getVolume();
		}
		
		virtual bool canFluidFlowThroughFace(BlockStateRef blockState, Direction blockFace, const Fluid& fluid) const {
			return !isSideSolid(blockState, blockFace);
		}

		//virtual AxisAlignedBoxF getFluidBox(BlockStateRef blockState) const {
		//	return full_aabb;
		//}

		// RENDERING-RELATED METHODS

		virtual bool canCullAdjacentFace(BlockStateRef blockState, Direction face, BlockStateRef adjacentBlockState) const {
			return isFullOpaqueCube(blockState);
		}
		virtual bool canCullAdjacentFace(BlockStateRef blockState, Direction face, const FluidState& adjacentFluidState) const;

		virtual Color getBlockColor(const MeshingWorldView worldView, BlockStateRef blockState, const glm::ivec3& blockPos, const uint8_t colorIndex) const {
			using namespace color_literals;
			return 0xFFFFFF_c;
		}

		virtual void onResourceLoadComplete() const {
		}

		virtual bool hasModel(BlockStateRef blockState) const {
			return true;
		}

		virtual void addFaceQuadsToBuffer(BlockStateRef blockState, const glm::ivec3& blockPos, Direction face, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const = 0;
		
		// TODO: change to return a std::span<std::string> instead of modifying a mutable argument
		void addModelNamesToSet(std::set<std::string>& modelNameSet) const {
			modelNameSet.insert(modelNames.begin(), modelNames.end());
		}

	private:
		
		void registerBlock() const;

		const Block& setId(const id_t id) const noexcept {
			this->id = id;
			return *this;
		}

	};

	class BasicBlock : public Block {
	protected:
		mutable std::unique_ptr<BakedBlockModel> model;

	public:

		BasicBlock(const std::string& name);

		void onResourceLoadComplete() const override;

		void addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, Direction face, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const override;

	};

}