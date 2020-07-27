#include "FluidState.h"

#include "Fluid.h"
#include "FluidRegistry.h"

namespace eng {

	FluidState::FluidState(FluidRef fluid, const metadata_t metadata) noexcept :
			FluidState(fluid.getId(), metadata) {}
	FluidState::FluidState(FluidRef fluid) :
			FluidState(fluid.getId(), fluid.getDefaultMetadata()) {}
	FluidState::FluidState() : FluidState(fluids::null_fluid) {}

	FluidRef FluidState::getFluid() const {
		return fluid_registry[static_cast<id_t>(id)];
	}
	
	bool FluidState::isEmpty() const noexcept {
		return (id == fluids::null_fluid.getId());
	}


	FiniteFluidState::FiniteFluidState(FluidRef fluid, amount_t amount, flow_t flow, const metadata_t metadata) noexcept :
			FiniteFluidState(fluid.getId(), amount, flow, metadata) {}
	FiniteFluidState::FiniteFluidState(FluidRef fluid, amount_t amount, const metadata_t metadata) noexcept :
			FiniteFluidState(fluid.getId(), amount, FluidFlow::Still, metadata) {}
	FiniteFluidState::FiniteFluidState(FluidRef fluid, amount_t amount, flow_t flow) :
			FiniteFluidState(fluid.getId(), amount, flow, fluid.getDefaultMetadata()) {}
	FiniteFluidState::FiniteFluidState(FluidRef fluid, amount_t amount) :
			FiniteFluidState(fluid.getId(), amount, FluidFlow::Still, fluid.getDefaultMetadata()) {}
	FiniteFluidState::FiniteFluidState() : FiniteFluidState(fluids::null_fluid, 0) {}

	FluidRef FiniteFluidState::getFluid() const {
		return fluid_registry[static_cast<id_t>(id)];
	}

	bool FiniteFluidState::isEmpty() const noexcept {
		return (id == fluids::null_fluid.getId());
	}


	InfiniteFluidState::InfiniteFluidState(FluidRef fluid, flow_t flow, const metadata_t metadata) noexcept :
			InfiniteFluidState(fluid.getId(), flow, metadata) {}
	InfiniteFluidState::InfiniteFluidState(FluidRef fluid, const metadata_t metadata) noexcept :
			InfiniteFluidState(fluid.getId(), FluidFlow::Still, metadata) {}
	InfiniteFluidState::InfiniteFluidState(FluidRef fluid, flow_t flow) :
			InfiniteFluidState(fluid.getId(), flow, fluid.getDefaultMetadata()) {}
	InfiniteFluidState::InfiniteFluidState(FluidRef fluid) :
			InfiniteFluidState(fluid.getId(), FluidFlow::Still, fluid.getDefaultMetadata()) {}
	InfiniteFluidState::InfiniteFluidState() : InfiniteFluidState(fluids::null_fluid) {}

	FluidRef InfiniteFluidState::getFluid() const {
		return fluid_registry[static_cast<id_t>(id)];
	}

	bool InfiniteFluidState::isEmpty() const noexcept {
		return (id == fluids::null_fluid.getId());
	}

}