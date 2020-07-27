#pragma once

#include <limits>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>

#include "util/math/math.h"

namespace eng {

	template<typename T = float>
	struct Plane {
		static_assert(std::numeric_limits<T>::is_iec559, "Plane only accepts floating-point template arguments");

		using vec3_t = glm::vec<3, T>;
		using vec4_t = glm::vec<4, T>;

		vec3_t normal;
		T d; // distance from origin

		constexpr explicit Plane(const vec4_t& v) : normal(v.x, v.y, v.z), d(v.w) {}
		constexpr Plane(const vec3_t& normal, const T d) : normal(normal), d(d) {}
		constexpr Plane(const T x, const T y, const T z, const T d) noexcept : normal(x, y, z), d(d) {}

		// get the plane that intersects three points
		static Plane fromPoints(const vec3_t& a, const vec3_t& b, const vec3_t& c) {
			if (a == b || a == c || b == c) return { 0, 0, 0, 0 }; // TODO: use epsilon equals?
			const vec3_t normal = glm::normalize(glm::cross(c - a, c - b));
			const T d = -glm::dot(normal, c); // any point on the plane could replace c
			return { normal, d };
		}
		// get a plane with a given normal that intersects a given point
		static Plane fromNormalAndPoint(const vec3_t& normal, const vec3_t& p) {
			const auto l = glm::length(normal);
			if (l == 0) return { 0, 0, 0, 0 }; // TODO: use epsilon equals?
			const vec3_t n = normal / l;
			const T d = -glm::dot(n, p);
			return { n, d };
		}

		constexpr explicit operator vec4_t() const {
			return vec4_t{ normal, d };
		}

		Plane& normalize() {
			const auto l = glm::length(normal);
			if (l != 0) { // TODO: use epsilon equals?
				normal /= l;
				d /= l;
			} else {
				normal = vec3_t(0, 0, 0);
				d = 0;
			}
			return *this;
		}

		constexpr bool operator ==(const Plane<T>& b) const {
			return (normal == b.normal) && (d == b.d);
		}
		constexpr bool operator !=(const Plane<T>& b) const {
			return (normal != b.normal) || (d != b.d);
		}

		constexpr operator bool() const {
			return (normal.x != T(0)) || (normal.y != T(0)) || (normal.z != T(0));
		}
		constexpr bool operator !() const {
			return (normal.x == T(0)) && (normal.y == T(0)) && (normal.z == T(0));
		}

		constexpr Plane<T> operator +() const { return *this; }
		constexpr Plane<T> operator -() const { return { -normal, -d }; }

		template<typename U = T>
		T signedDistanceTo(const glm::vec<3, U>& p) const {
			return glm::dot(static_cast<vec3_t>(p), normal) + d;
		}

		template<typename U = T>
		constexpr T intersects(const glm::vec<3, U>& p) const {
			epsZero(signedDistanceTo(p));
		}

	};

	template<typename T>
	constexpr bool epsEqual(const Plane<T>& a, const Plane<T>& b) {
		return epsEqual(a.normal, b.normal) && epsEqual(a.d, b.d);
	}
	template<typename T>
	constexpr bool epsNotEqual(const Plane<T>& a, const Plane<T>& b) {
		return epsNotEqual(a.normal, b.normal) || epsNotEqual(a.d, b.d);
	}


	using PlaneF = Plane<float>;
	using PlaneD = Plane<double>;

}