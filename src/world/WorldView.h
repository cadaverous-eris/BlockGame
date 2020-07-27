#pragma once

#include <type_traits>

#include <glm/vec3.hpp>

#include "block/BlockState.h"

namespace eng {

	// if T fulfills the requirements of WorldView
	template<typename T, typename = void>
	struct is_world_view : std::false_type {};
	// if T fulfills the requirements of WorldView
	template<typename T>
	struct is_world_view<T, std::void_t<
		std::enable_if_t<std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(std::declval<const T>().getBlockState(std::declval<glm::ivec3>()))>>, BlockState>> // T::getBlockState() returns a BlockState or const BlockState&
	>> : std::true_type {};
	// if T fulfills the requirements of WorldView
	template<typename T>
	inline constexpr bool is_world_view_v = is_world_view<T>::value;

}