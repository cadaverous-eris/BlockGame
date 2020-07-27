#pragma once

#include <type_traits>
#include <limits>
#include <utility>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/transform.hpp>

#include "util/math/math.h"
#include "util/direction.h"
#include "Ray.h"

namespace eng {

	template<typename T = float>
	struct AxisAlignedBox {
		using vec_type = glm::vec<3, T>;

		vec_type min { 0, 0, 0 };
		vec_type max { 0, 0, 0 };

		constexpr AxisAlignedBox(const vec_type& min, const vec_type& max) noexcept : min(min), max(max) {}
		constexpr AxisAlignedBox(const vec_type& size) : max(size) {}
		constexpr AxisAlignedBox() = default;

		constexpr AxisAlignedBox(const AxisAlignedBox&) = default;
		constexpr AxisAlignedBox(AxisAlignedBox&&) = default;
		constexpr AxisAlignedBox& operator =(const AxisAlignedBox&) = default;
		constexpr AxisAlignedBox& operator =(AxisAlignedBox&&) = default;

		template<typename U>
		explicit AxisAlignedBox(const std::enable_if_t<!std::is_same_v<T, U> && std::is_convertible_v<glm::vec<3, U>, vec_type>, AxisAlignedBox<U>>& b) :
				min(static_cast<vec_type>(b.min)), max(static_cast<vec_type>(b.max)) {}

		// TODO: union, scaling etc

		vec_type getCenter() const {
			return (min + max) / T(2);
		}

		T getVolume() const {
			return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
		}

		template<typename U>
		bool contains(const glm::vec<3, U>& p) const {
			return (static_cast<bool>(*this) &&
					(p.x >= min.x && p.x <= max.x) &&
					(p.y >= min.y && p.y <= max.y) &&
					(p.z >= min.z && p.z <= max.z));
		}
		template<typename U>
		bool contains(const AxisAlignedBox<U>& b) const {
			return (static_cast<bool>(b) && contains(b.min) && contains(b.max));
		}
		template<typename U>
		bool intersects(const AxisAlignedBox<U>& b) const {
			return (static_cast<bool>(b) &&
					static_cast<bool>(*this) &&
					(min.x < b.max.x && max.x > b.min.x) &&
					(min.y < b.max.y && max.y > b.min.y) &&
					(min.z < b.max.z && max.z > b.min.z));
		}

		operator bool() const {
			return min != max;
		}

		bool operator ==(const AxisAlignedBox<T>& b) const {
			return (min == b.min) && (max == b.max);
		}
		bool operator !=(const AxisAlignedBox<T>& b) const {
			return (min != b.min) || (max != b.max);
		}

		AxisAlignedBox<T> operator +() const { return *this; }
		AxisAlignedBox<T> operator -() const { return { max, min }; }

		// offset bounding box
		template<typename U>
		auto operator +=(const glm::vec<3, U>& offset) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, vec_type>, AxisAlignedBox<T>&> {
			const auto o = static_cast<vec_type>(offset);
			min += o;
			max += o;
			return *this;
		}
		// offset bounding box
		template<typename U>
		auto operator -=(const glm::vec<3, U>& offset) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, vec_type>, AxisAlignedBox<T>&> {
			const auto o = static_cast<vec_type>(offset);
			min -= o;
			max -= o;
			return *this;
		}

		// scale bounding box
		template<typename U>
		auto operator *=(const glm::vec<3, U>& scale) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, vec_type>, AxisAlignedBox<T>&> {
			const auto s = static_cast<vec_type>(scale);
			min *= s;
			max *= s;
			return *this;
		}
		// scale bounding box
		template<typename U>
		auto operator *=(const U scale) ->
				std::enable_if_t<std::is_convertible_v<U, T>, AxisAlignedBox<T>&> {
			const auto s = static_cast<T>(scale);
			min *= s;
			max *= s;
			return *this;
		}
		// scale bounding box
		template<typename U>
		auto operator /=(const glm::vec<3, U>& scale) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, vec_type>, AxisAlignedBox<T>&> {
			const auto s = static_cast<vec_type>(scale);
			min /= s;
			max /= s;
			return *this;
		}
		// scale bounding box
		template<typename U>
		auto operator /=(const U scale) ->
				std::enable_if_t<std::is_convertible_v<U, T>, AxisAlignedBox<T>&> {
			const auto s = static_cast<T>(scale);
			min /= s;
			max /= s;
			return *this;
		}
		
		// union of 2 bounding boxes
		template<typename U>
		auto operator |=(const AxisAlignedBox<U>& b) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, vec_type>, AxisAlignedBox<T>&> {
			min = glm::min(min, static_cast<vec_type>(b.min));
			max = glm::max(max, static_cast<vec_type>(b.max));
			return *this;
		}
		// intersection of 2 bounding boxes
		template<typename U>
		auto operator &=(const AxisAlignedBox<U>& b) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, vec_type>, AxisAlignedBox<T>&> {
			min = glm::max(min, static_cast<vec_type>(b.min));
			max = glm::min(max, static_cast<vec_type>(b.max));
			return *this;
		}

		template<typename U>
		auto getExpanded(const U d) const -> AxisAlignedBox<std::common_type_t<U, T>> {
			using ret_val_type = std::common_type_t<U, T>;
			using ret_vec_type = glm::vec<3, ret_val_type>;
			return {
				static_cast<glm::vec<3, ret_val_type>>(min) - static_cast<ret_val_type>(d),
				static_cast<glm::vec<3, ret_val_type>>(max) + static_cast<ret_val_type>(d),
			};
		}
		template<typename U>
		auto getExpanded(const glm::vec<3, U>& d) const -> AxisAlignedBox<std::common_type_t<U, T>> {
			using ret_vec_type = glm::vec<3, std::common_type_t<U, T>>;
			return {
				static_cast<ret_vec_type>(min) - static_cast<ret_vec_type>(d),
				static_cast<ret_vec_type>(max) + static_cast<ret_vec_type>(d),
			};
		}
		template<typename U>
		auto getContracted(const U d) const -> AxisAlignedBox<std::common_type_t<U, T>> {
			return getExpanded(-d);
		}
		template<typename U>
		auto getContracted(const glm::vec<3, U>& d) const -> AxisAlignedBox<std::common_type_t<U, T>> {
			return getExpanded(-d);
		}

		template<typename U = T, std::enable_if_t<std::numeric_limits<U>::is_iec559, int> = 0>
		AxisAlignedBox<U> getRotated(const Axis axis, int rotations, const vec_type& origin) const {
			rotations = rotations & 3;
			const glm::mat<4, 4, T> matrix = glm::translate(glm::rotate(
				glm::translate(glm::mat<4, 4, T>(1), origin),
				glm::radians(rotations * T{90}),
				direction::toVector<vec_type>(axis)
			), -origin);
			vec_type rMin(matrix * glm::vec<4, T>(min, 1));
			vec_type rMax(matrix * glm::vec<4, T>(max, 1));
			for (const auto& a : axis::axes) {
				const auto ra = direction::rotateAround(a, axis, RotationDirection::CW, rotations);
				const auto ai = axis::getIndex(a);
				const auto rai = axis::getIndex(ra);
				if ((min[ai] < max[ai]) != (rMin[rai] < rMax[rai]))
					std::swap(rMin[rai], rMax[rai]);
			}
			return { rMin, rMax };
		}

		vec_type getNegativeVert(const glm::vec3& normal) const {
			return {
				(normal.x < 0) ? max.x : min.x,
				(normal.y < 0) ? max.y : min.y,
				(normal.z < 0) ? max.z : min.z,
			};
		}
		vec_type getNegativeVert(const glm::dvec3& normal) const {
			return {
				(normal.x < 0) ? max.x : min.x,
				(normal.y < 0) ? max.y : min.y,
				(normal.z < 0) ? max.z : min.z,
			};
		}
		vec_type getPositiveVert(const glm::vec3& normal) const noexcept {
			return {
				(normal.x > 0) ? max.x : min.x,
				(normal.y > 0) ? max.y : min.y,
				(normal.z > 0) ? max.z : min.z,
			};
		}
		vec_type getPositiveVert(const glm::dvec3 normal) const {
			return {
				(normal.x > 0) ? max.x : min.x,
				(normal.y > 0) ? max.y : min.y,
				(normal.z > 0) ? max.z : min.z,
			};
		}

		template<typename U>
		bool intersectRay(const Ray<U>& ray) const {
			using vec = typename Ray<U>::vec3_t;

			const vec& origin = ray.getOrigin();
			const U range = ray.getLength();
			if (!(*this) || (range <= 0)) return false;
			if (contains(origin)) return true;

			const vec& inverseDir = ray.getInverseDir();

			vec t0 = (static_cast<vec>(min) - origin);
			vec t1 = (static_cast<vec>(max) - origin);
			t0[0] = (t0[0] == U(0)) ? 0 : t0[0] * inverseDir[0];
			t1[0] = (t1[0] == U(0)) ? 0 : t1[0] * inverseDir[0];
			t0[1] = (t0[1] == U(0)) ? 0 : t0[1] * inverseDir[1];
			t1[1] = (t1[1] == U(0)) ? 0 : t1[1] * inverseDir[1];
			t0[2] = (t0[2] == U(0)) ? 0 : t0[2] * inverseDir[2];
			t1[2] = (t1[2] == U(0)) ? 0 : t1[2] * inverseDir[2];

			const vec tComponentMins = glm::min(t0, t1);
			const vec tComponentMaxs = glm::max(t0, t1);

			const U tMin = vmax(tComponentMins[0], tComponentMins[1], tComponentMins[2]);
			const U tMax = vmin(range, tComponentMaxs[0], tComponentMaxs[1], tComponentMaxs[2]);
			return (tMin <= tMax) && (tMax >= U(0));
		}

		template<typename U>
		bool intersectRay(const Ray<U>& ray, U& distance) const {
			using vec = typename Ray<U>::vec3_t;

			const U range = ray.getLength();
			if (!(*this) || (range <= 0)) return false;

			const vec& origin = ray.getOrigin();
			const vec& inverseDir = ray.getInverseDir();

			vec t0 = (static_cast<vec>(min) - origin);
			vec t1 = (static_cast<vec>(max) - origin);
			t0[0] = (t0[0] == U(0)) ? 0 : t0[0] * inverseDir[0];
			t1[0] = (t1[0] == U(0)) ? 0 : t1[0] * inverseDir[0];
			t0[1] = (t0[1] == U(0)) ? 0 : t0[1] * inverseDir[1];
			t1[1] = (t1[1] == U(0)) ? 0 : t1[1] * inverseDir[1];
			t0[2] = (t0[2] == U(0)) ? 0 : t0[2] * inverseDir[2];
			t1[2] = (t1[2] == U(0)) ? 0 : t1[2] * inverseDir[2];

			const vec tComponentMins = glm::min(t0, t1);
			const vec tComponentMaxs = glm::max(t0, t1);

			const U tMin = vmax(tComponentMins[0], tComponentMins[1], tComponentMins[2]);
			const U tMax = vmin(tComponentMaxs[0], tComponentMaxs[1], tComponentMaxs[2]);
			if ((tMin > tMax) || (tMax < U(0)) || (tMin > range)) return false;

			distance = (tMin < 0) ? tMax : tMin;
			return true;
		}

		template<typename U>
		Direction intersectRayFace(const Ray<U>& ray, U& distance) const {
			using vec = typename Ray<U>::vec3_t;

			const U range = ray.getLength();
			if (!(*this) || (range <= 0)) return Direction::UNDEFINED;

			const vec& origin = ray.getOrigin();
			const vec& inverseDir = ray.getInverseDir();

			vec t0 = (static_cast<vec>(min) - origin);
			vec t1 = (static_cast<vec>(max) - origin);
			t0[0] = (t0[0] == U(0)) ? 0 : t0[0] * inverseDir[0];
			t1[0] = (t1[0] == U(0)) ? 0 : t1[0] * inverseDir[0];
			t0[1] = (t0[1] == U(0)) ? 0 : t0[1] * inverseDir[1];
			t1[1] = (t1[1] == U(0)) ? 0 : t1[1] * inverseDir[1];
			t0[2] = (t0[2] == U(0)) ? 0 : t0[2] * inverseDir[2];
			t1[2] = (t1[2] == U(0)) ? 0 : t1[2] * inverseDir[2];

			const vec tComponentMins = glm::min(t0, t1);
			const vec tComponentMaxs = glm::max(t0, t1);

			const U tMin = vmax(tComponentMins[0], tComponentMins[1], tComponentMins[2]);
			const U tMax = vmin(tComponentMaxs[0], tComponentMaxs[1], tComponentMaxs[2]);
			if ((tMin > tMax) || (tMax < U(0)) || (tMin > range)) return Direction::UNDEFINED;

			distance = (tMin < 0) ? tMax : tMin;
			if (distance == t0.y) return Direction::DOWN; // -y -> DOWN
			if (distance == t1.y) return Direction::UP; // +y -> UP
			if (distance == t0.z) return Direction::NORTH; // -z -> NORTH
			if (distance == t1.z) return Direction::SOUTH; // +z -> SOUTH
			if (distance == t0.x) return Direction::WEST; // -x -> WEST
			if (distance == t1.x) return Direction::EAST; // +x -> EAST
			return Direction::UNDEFINED;
		}

	};

	// offset bounding box
	template<typename T, typename U>
	auto operator +(AxisAlignedBox<T> lhs, const glm::vec<3, U>& offset) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, typename AxisAlignedBox<T>::vec_type>, AxisAlignedBox<T>> {
		return lhs += offset;
	}
	// offset bounding box
	template<typename T, typename U>
	auto operator -(AxisAlignedBox<T> lhs, const glm::vec<3, U>& offset) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, typename AxisAlignedBox<T>::vec_type>, AxisAlignedBox<T>> {
		return lhs -= offset;
	}

	// scale bounding box
	template<typename T, typename U>
	auto operator *(AxisAlignedBox<T> lhs, const glm::vec<3, U>& scale) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, typename AxisAlignedBox<T>::vec_type>, AxisAlignedBox<T>> {
		return lhs *= scale;
	}
	// scale bounding box
	template<typename T, typename U>
	auto operator *(AxisAlignedBox<T> lhs, const U scale) ->
			std::enable_if_t<std::is_convertible_v<U, T>, AxisAlignedBox<T>> {
		return lhs *= scale;
	}
	// scale bounding box
	template<typename T, typename U>
	auto operator /(AxisAlignedBox<T> lhs, const glm::vec<3, U>& scale) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, typename AxisAlignedBox<T>::vec_type>, AxisAlignedBox<T>> {
		return lhs /= scale;
	}
	// scale bounding box
	template<typename T, typename U>
	auto operator /(AxisAlignedBox<T> lhs, const U scale) ->
			std::enable_if_t<std::is_convertible_v<U, T>, AxisAlignedBox<T>> {
		return lhs /= scale;
	}

	// union of 2 bounding boxes
	template<typename T, typename U>
	auto operator |(AxisAlignedBox<T> lhs, const AxisAlignedBox<U>& rhs) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, typename AxisAlignedBox<T>::vec_type>, AxisAlignedBox<T>> {
		return lhs |= rhs;
	}
	// intersection of 2 bounding boxes
	template<typename T, typename U>
	auto operator &(AxisAlignedBox<T> lhs, const AxisAlignedBox<U>& rhs) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<3, U>, typename AxisAlignedBox<T>::vec_type>, AxisAlignedBox<T>> {
		return lhs &= rhs;
	}

	using AxisAlignedBoxF = AxisAlignedBox<float>;
	using AxisAlignedBoxD = AxisAlignedBox<double>;
	using AxisAlignedBoxI8 = AxisAlignedBox<int8_t>;
	using AxisAlignedBoxI16 = AxisAlignedBox<int16_t>;
	using AxisAlignedBoxI32 = AxisAlignedBox<int32_t>;
	using AxisAlignedBoxI64 = AxisAlignedBox<int64_t>;
	using AxisAlignedBoxI = AxisAlignedBoxI32;

}