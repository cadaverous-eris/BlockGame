#pragma once

#include <array>
#include <type_traits>

#include <glm/mat4x4.hpp>

#include "Plane.h"
#include "AxisAlignedBox.h"

namespace eng {

	enum class FrustumPlane { Top = 0, Bottom = 1, Left = 2, Right = 3, Near = 4, Far = 5 };

	template<typename T = float>
	struct Frustum {
	public:
		inline static constexpr size_t plane_count = 6;
		inline static constexpr std::array<FrustumPlane, plane_count> frustum_plane_values {
			FrustumPlane::Top,  FrustumPlane::Bottom,
			FrustumPlane::Left, FrustumPlane::Right,
			FrustumPlane::Near, FrustumPlane::Far
		};
	private:
		std::array<Plane<T>, plane_count> planes;

	public:
		Frustum(const glm::mat<4, 4, T> mat, const bool normalize = true) : planes({
			Plane<T>(mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], mat[3][3] - mat[3][1]), // top
			Plane<T>(mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], mat[3][3] + mat[3][1]), // bottom
			Plane<T>(mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], mat[3][3] + mat[3][0]), // left
			Plane<T>(mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], mat[3][3] - mat[3][0]), // right
			Plane<T>(mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], mat[3][3] + mat[3][2]), // near
			Plane<T>(mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], mat[3][3] - mat[3][2])  // far
		}) {
			if (normalize) { // normalize the plane equations, if requested
				planes[0].normalize();
				planes[1].normalize();
				planes[2].normalize();
				planes[3].normalize();
				planes[4].normalize();
				planes[5].normalize();
			}
		}

		// update frustum from matrix
		void update(const glm::mat<4, 4, T> mat, const bool normalize = true) {
			planes[0] = Plane<T>(mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], mat[3][3] - mat[3][1]); // top
			planes[1] = Plane<T>(mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], mat[3][3] + mat[3][1]); // bottom
			planes[2] = Plane<T>(mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], mat[3][3] + mat[3][0]); // left
			planes[3] = Plane<T>(mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], mat[3][3] - mat[3][0]); // right
			planes[4] = Plane<T>(mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], mat[3][3] + mat[3][2]); // near
			planes[5] = Plane<T>(mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], mat[3][3] - mat[3][2]); // far
			if (normalize) { // normalize the plane equations, if requested
				planes[0].normalize();
				planes[1].normalize();
				planes[2].normalize();
				planes[3].normalize();
				planes[4].normalize();
				planes[5].normalize();
			}
		}

		Plane<T>& operator [](const size_t i) {
			return planes.at(i);
		}
		const Plane<T>& operator [](const size_t i) const {
			return planes.at(i);
		}
		Plane<T>& operator [](const FrustumPlane p) {
			return planes[static_cast<std::underlying_type_t<FrustumPlane>>(p)];
		}
		const Plane<T>& operator [](const FrustumPlane p) const {
			return planes[static_cast<std::underlying_type_t<FrustumPlane>>(p)];
		}

		template<typename U>
		bool contains(const glm::vec<3, U>& p) const {
			for (const auto& plane : planes) {
				if (plane.signedDistanceTo(p) < 0) return false;
			}
			return true;
		}
		template<typename U>
		bool contains(const AxisAlignedBox<U>& aabb) const {
			for (const auto& plane : planes) {
				if (plane.signedDistanceTo(aabb.getPositiveVert(plane.normal)) < 0) return false;
			}
			return true;
		}
		//template<typename U>
		//bool contains(const Sphere<U>& sphere) const {
		//	for (const auto& plane : planes) {
		//		if (plane.signedDistanceTo(sphere.pos) < -sphere.radius) return false;
		//	}
		//	return true;
		//}

		
	};

	using FrustumF = Frustum<float>;
	using FrustumD = Frustum<double>;

}