#pragma once

#include "fluid/Fluid.h"

namespace eng {

	class AtlasSprite;

	class FluidFinite : public Fluid {
	protected:
		mutable const AtlasSprite* stillTexture {};
		mutable const AtlasSprite* flowTexture {};

	public:

		FluidFinite(const std::string& name) : Fluid(name) {}


		AxisAlignedBoxF getBoundingBox(const World& world, FluidStateRef fluidState, const glm::ivec3& blockPos) const override;

		uint8_t getAmount(FluidStateRef fluidState) const override;

		void onFluidUpdate(World& world, FluidStateRef fluidState, const glm::ivec3& blockPos, const Direction srcDirection) const override;


		void onResourceLoadComplete() const override;

		void addQuadsToBuffer(FluidStateRef fluidState, const glm::ivec3& blockPos, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const override;

		virtual RenderLayer getRenderLayer() const { return RenderLayer::Transparent; }


		void addTextureNamesToSet(std::set<std::string>& textureNameSet) const override;

	};

}