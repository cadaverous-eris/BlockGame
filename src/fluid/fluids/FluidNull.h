#pragma once

#include "fluid/Fluid.h"
#include "block/Block.h"

namespace eng {

	class FluidNull : public Fluid {
	public:

		FluidNull() : Fluid("null") {}

		bool isNullFluid(FluidStateRef fluidState) const override {
			return true;
		}

		AxisAlignedBoxF getBoundingBox(const World& world, FluidStateRef fluidState, const glm::ivec3& blockPos) const override {
			return Block::null_aabb;
		}

		uint8_t getAmount(FluidStateRef fluidState) const override {
			return 0;
		}

		void onFluidUpdate(World& world, FluidStateRef fluidState, const glm::ivec3& blockPos, const Direction srcDirection) const {
		}

		void addQuadsToBuffer(FluidStateRef fluidState, const glm::ivec3& blockPos, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const override {}


		void addTextureNamesToSet(std::set<std::string>& textureNameSet) const override {}

	};

}