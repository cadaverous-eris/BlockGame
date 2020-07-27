#pragma once

#include <limits>

#include <glm/vec3.hpp>
#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include "math.h"

namespace eng {

	template<typename T> struct AxisAlignedBox;

	template<typename T = float>
	struct Ray {
		static_assert(std::numeric_limits<T>::is_iec559, "Ray only accepts floating-point template arguments");

		using vec3_t = glm::vec<3, T>;

	private:
		vec3_t origin;
		vec3_t inverseDir; // store inverse direction instead of direction because inverse direction is more useful
		T length;

		constexpr Ray(const vec3_t& origin, const vec3_t& inverseDir, const T length) :
				origin(origin), inverseDir(inverseDir), length(length) {}
	public:

		static constexpr Ray<T> fromDir(const vec3_t& origin, const vec3_t& dir, const T length) {
			return { origin, T{1} / dir, length };
		}
		static constexpr Ray<T> fromInverseDir(const vec3_t& origin, const vec3_t& inverseDir, const T length) {
			return { origin, inverseDir, length };
		}

		Ray(const vec3_t& origin, const vec3_t& endPoint) : origin(origin) {
			vec3_t diff = endPoint - origin;
			length = glm::length(diff);
			inverseDir = length / diff;
		}
		
		inline const vec3_t& getOrigin() const { return origin; }
		inline const vec3_t& getInverseDir() const { return inverseDir; }
		inline T getLength() const { return length; }

		inline vec3_t getDiffVec() const { return length / inverseDir; } // returns endPoint - origin
		inline vec3_t getDir() const { return T{1} / inverseDir; }
		inline vec3_t getEndPoint() const { return (length / inverseDir) + origin; }

		inline const vec3_t setOrigin(const vec3_t& origin) {
			this->origin = origin;
		}
		inline void setInverseDir(const vec3_t& invDir) {
			inverseDir = invDir;
		}
		inline void setDir(const vec3_t& dir) {
			inverseDir = T{1} / dir;
		}
		void setEndPoint(const vec3_t& endPoint) {
			vec3_t diff = endPoint - origin;
			length = glm::length(diff);
			inverseDir = length / diff;
		}
		inline void setLength(const T length) {
			this->length = length;
		}

		Ray<T> withLength(const T length) {
			return { origin, inverseDir, length };
		}

		// returns the point on the ray at the distance d
		vec3_t project(const T d) const {
			return (d / inverseDir) + origin;
		}

		AxisAlignedBox<T> getBoundingBox() const;

		constexpr bool operator ==(const Ray<T>& b) const {
			return (origin == b.origin) && (inverseDir == b.inverseDir) && (length == b.length);
		}
		constexpr bool operator !=(const Ray<T>& b) const {
			return (origin != b.origin) || (inverseDir != b.inverseDir) || (length != b.length);
		}

		constexpr operator bool() const {
			return length;
		}

	};

	template<typename T>
	constexpr bool epsEqual(const Ray<T>& a, const Ray<T>& b) {
		return epsEqual(a.origin, b.origin) && epsEqual(a.inverseDir, b.inverseDir) && epsEqual(a.length, b.length);
	}
	template<typename T>
	constexpr bool epsNotEqual(const Ray<T>& a, const Ray<T>& b) {
		return epsNotEqual(a.origin, b.origin) || epsNotEqual(a.inverseDir, b.inverseDir) || epsNotEqual(a.length, b.length);
	}


	using RayF = Ray<float>;
	using RayD = Ray<double>;

}

#include "AxisAlignedBox.h"

namespace eng {

	template<typename T>
	AxisAlignedBox<T> Ray<T>::getBoundingBox() const {
		return { origin, getEndPoint() };
	}

}