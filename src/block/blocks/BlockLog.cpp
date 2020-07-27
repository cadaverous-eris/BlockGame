#include "BlockLog.h"

#include "util/resources/ResourceManager.h"
#include "world/World.h"

namespace eng {

	BlockState BlockLog::getStateForPlacement(const World&, const glm::ivec3& blockPos, const RayCastResultF& rayCastResult/*, const EntityPlayer* const player*/) const {
		const Axis axis = axis::getAxis(rayCastResult.face);
		switch (axis) {
		case Axis::X:
		case Axis::Y:
		case Axis::Z:
			return createBlockState(axis::getIndex(axis));
		default:
			return createBlockState();
		}
	}

}