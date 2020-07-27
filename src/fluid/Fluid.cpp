#include "Fluid.h"

#include "FluidRegistry.h"
#include "world/World.h"

namespace eng {

	void Fluid::registerFluid() const {
		fluid_registry.registerObject(*this);
	}

}