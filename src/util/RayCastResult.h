#pragma once

#include <limits>

#include <glm/vec3.hpp>

#include "util/direction.h"

namespace eng {

	enum class RayCastResultType { Block, Fluid, Entity, None };

	template<typename T = float>
	struct RayCastResult {
		static_assert(std::numeric_limits<T>::is_iec559, "RayCastResult only accepts floating-point template arguments");

		using HitType = RayCastResultType;
		using vec_type = glm::vec<3, T>;

		HitType type = HitType::None;
		vec_type position { 0, 0, 0 }; // point of intersection
		glm::ivec3 blockPos { 0, 0, 0 }; // blockPos of the block or fluid that was hit
		Direction face = Direction::UNDEFINED; // the face of the block or fluid that was hit
		//Entity* entity = nullptr; // entity that was hit // TODO: add
		T distance = std::numeric_limits<T>::infinity(); // distance to the point of intersection

		// true if anything was hit
		inline operator bool() const { return type != HitType::None; }

		// returns true if a block was hit
		inline bool hitBlock() const { return type == HitType::Block; }
		// returns true if a fluid was hit
		inline bool hitFluid() const { return type == HitType::Fluid; }
		// returns true if an entity was hit
		inline bool hitEntity() const { return type == HitType::Entity; }

		// returns the RayCastResult with the smaller distance value
		inline RayCastResult operator &(const RayCastResult& b) const noexcept {
			return (b.distance < distance) ? b : *this;
		}
		RayCastResult& operator &=(const RayCastResult& b) noexcept {
			if (b.distance < distance) *this = b;
			return *this;
		}
	};

	using RayCastResultF = RayCastResult<float>;
	using RayCastResultD = RayCastResult<double>;

}