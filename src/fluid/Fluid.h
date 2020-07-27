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
#include "FluidState.h"
#include "util/direction.h"
#include "util/math/AxisAlignedBox.h"
#include "util/Color.h"
#include "util/RayCastResult.h"
#include "render/world/RenderLayer.h"
#include "model/block/BlockQuad.h"
#include "render/world/chunk/MeshingWorldView.h"

namespace eng {

	class World;

	class Fluid {
		friend class Registry<Fluid>;
	public:
		using id_t = FluidState::id_t;
		using metadata_t = FluidState::metadata_t;

	private:
		const std::string name;
		mutable id_t id;

	public:
		Fluid(const std::string& name) : name(name) {
			registerFluid();
		}

		Fluid(const Fluid&) = delete;
		Fluid& operator =(const Fluid&) = delete;
		Fluid(Fluid&&) = delete;
		Fluid& operator =(Fluid&&) = delete;

		virtual ~Fluid() = default;

		id_t getId() const noexcept { return id; }
		const std::string& getName() const noexcept { return name; }

		bool operator ==(FluidRef b) const noexcept {
			return id == b.id; // check equality by fluid id
		}
		bool operator !=(FluidRef b) const noexcept {
			return id != b.id; // check equality by fluid id
		}

		FluidState createFluidState() const {
			return { id, getDefaultMetadata() };
		}
		FluidState createFluidState(const metadata_t metadata) const noexcept {
			return { id, metadata };
		}

		// FLUID METHODS

		virtual bool isNullFluid(FluidStateRef fluidState) const {
			return false;
		}

		virtual metadata_t getDefaultMetadata() const {
			return 0;
		}

		virtual AxisAlignedBoxF getBoundingBox(const World& world, FluidStateRef fluidState, const glm::ivec3& blockPos) const = 0;

		virtual void onFluidUpdate(World& world, FluidStateRef fluidState, const glm::ivec3& blockPos, const Direction srcDirection) const = 0;

		virtual bool isReplaceable(FluidStateRef fluidState) const {
			return true;
		}

		virtual uint8_t getAmount(FluidStateRef fluidState) const = 0;

		// RENDERING-RELATED METHODS

		virtual Color getFluidColor(const MeshingWorldView worldView, FluidStateRef fluidState, const glm::ivec3& blockPos, const uint8_t colorIndex) const {
			using namespace color_literals;
			return 0xFFFFFF_c;
		}

		virtual void onResourceLoadComplete() const {}

		virtual void addQuadsToBuffer(FluidStateRef fluidState, const glm::ivec3& blockPos, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const = 0;


		// TODO: change to return a std::span<std::string> instead of modifying a mutable argument
		virtual void addTextureNamesToSet(std::set<std::string>& textureNameSet) const = 0;

	private:

		void registerFluid() const;

		const Fluid& setId(const id_t id) const noexcept {
			this->id = id;
			return *this;
		}

	};

}