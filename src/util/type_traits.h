#pragma once

#include <type_traits>
#include <iterator>
#include <optional>

#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>

namespace eng {

	// converts an enum value to it's underlying type
	template<typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
	inline constexpr std::underlying_type_t<T> to_underlying(const T e) noexcept {
		return static_cast<std::underlying_type_t<T>>(e);
	}

	template<typename T>
	inline constexpr bool is_float_or_integral_v = std::is_same_v<T, float> || (std::is_integral_v<T> && !std::is_same_v<T, bool>);

	// detect if Iter is an iterator with a value type of T
	template<typename Iter, typename T, typename = void>
	struct is_iterator : std::false_type {};
	template<typename Iter, typename T>
	struct is_iterator<Iter, T, typename std::enable_if_t<
		std::is_same_v<typename std::iterator_traits<Iter>::value_type, T>
	>> : std::true_type {};
	template<typename Iter, typename T>
	constexpr bool is_iterator_v = is_iterator<Iter, T>::value;

	// detect if T is an optional type
	template<typename T>
	struct is_optional : std::false_type {};
	template<typename T>
	struct is_optional<std::optional<T>> : std::true_type {};
	template<typename T>
	constexpr bool is_optional_v = is_optional<T>::value;

	namespace detail {
		template<typename T, typename = void, typename... Args>
		struct is_direct_list_initializable : std::false_type {};
		template<typename T, typename... Args>
		struct is_direct_list_initializable<T, std::void_t<decltype(T{ std::declval<Args>()... })>, Args...> : std::true_type {};
	}
	// detect if T is list-constructible from a set of arguments
	template<typename T, typename... Args>
	using is_direct_list_initializable = detail::is_direct_list_initializable<T, void, Args...>;
	template<typename T, typename... Args>
	constexpr bool is_direct_list_initializable_v = is_direct_list_initializable<T, Args...>::value;


	template<typename T>
	struct glm_type {
		static constexpr bool isVec = false, isMat = false, isQuat = false;
		static constexpr glm::length_t components = 0;
		static constexpr glm::length_t cols = 0, rows = 0;
	};

	template <glm::length_t L, typename T, glm::qualifier Q>
	struct glm_type<glm::vec<L, T, Q>> {
		using type = glm::vec<L, T, Q>;
		using value_type = T;
		static constexpr glm::qualifier qualifier = Q;
		static constexpr bool isVec = true, isMat = false, isQuat = false;
		static constexpr glm::length_t components = L;
	};
	template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	struct glm_type<glm::mat<C, R, T, Q>> {
		using type = glm::mat<C, R, T, Q>;
		using value_type = T;
		static constexpr glm::qualifier qualifier = Q;
		static constexpr bool isVec = false, isMat = true, isQuat = false;
		static constexpr glm::length_t components = C;
		static constexpr glm::length_t cols = C, rows = R;
	};
	template <typename T, glm::qualifier Q>
	struct glm_type<glm::qua<T, Q>> {
		using type = glm::qua<T, Q>;
		using value_type = T;
		static constexpr glm::qualifier qualifier = Q;
		static constexpr bool isVec = false, isMat = false, isQuat = true;
		static constexpr glm::length_t components = 4;
	};
	template <typename T, glm::qualifier Q>
	struct glm_type<glm::tdualquat<T, Q>> {
		using type = glm::tdualquat<T, Q>;
		using value_type = T;
		static constexpr glm::qualifier qualifier = Q;
		static constexpr bool isVec = false, isMat = false, isQuat = true;
		static constexpr glm::length_t components = 8;
	};


}