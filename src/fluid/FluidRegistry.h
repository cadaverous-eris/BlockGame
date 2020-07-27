#pragma once

#include "util/Registry.h"
#include "Fluid.h"
#include "fluids/FluidNull.h"
#include "fluids/FluidFinite.h"

namespace eng {

	inline Registry<Fluid> fluid_registry {};

	namespace fluids {
		// TODO: more block definitions

		inline const FluidNull null_fluid {};
		inline const FluidState empty_fluidstate { null_fluid };

		inline const FluidFinite water("water");

	}

}