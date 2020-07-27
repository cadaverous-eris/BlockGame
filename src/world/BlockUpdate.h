#pragma once

#include <glm/vec3.hpp>

#include "block/BlockState.h"
#include "util/direction.h"

namespace eng {

	struct BlockUpdate {
		glm::ivec3 blockPos = { 0, 0, 0 };
		Direction srcDirection = Direction::UNDEFINED;

		BlockUpdate() = default;
		BlockUpdate(const glm::ivec3& blockPos, const Direction& srcDirection) noexcept :
				blockPos(blockPos), srcDirection(srcDirection) {}

		static inline constexpr Direction mergeDirections(const Direction dir1, const Direction dir2) noexcept {
			return (dir1 == dir2) ? dir1 : Direction::UNDEFINED;
		}
	};

	struct ScheduledBlockUpdate {
		BlockUpdate blockUpdate;
		uint16_t delay;

		ScheduledBlockUpdate() = default;
		ScheduledBlockUpdate(const BlockUpdate& blockUpdate, const uint16_t delay) noexcept :
				blockUpdate(blockUpdate), delay(delay) {}
	};

	using FluidUpdate = BlockUpdate;
	using ScheduledFluidUpdate = ScheduledBlockUpdate;

}