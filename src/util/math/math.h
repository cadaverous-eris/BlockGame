#pragma once

#include <utility>
#include <type_traits>
#include <limits>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <bit>
#ifdef _MSC_VER
#include <intrin.h>
#endif

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/vector_relational.hpp>
#include <glm/ext/vector_relational.hpp>
#include <glm/ext/scalar_relational.hpp>

namespace eng {

	template<size_t S> struct sized_uint_type;
	template<> struct sized_uint_type<1> { using type = uint8_t; static_assert(sizeof(type) == 1); };
	template<> struct sized_uint_type<2> { using type = uint16_t; static_assert(sizeof(type) == 2); };
	template<> struct sized_uint_type<4> { using type = uint32_t; static_assert(sizeof(type) == 4); };
	template<> struct sized_uint_type<8> { using type = uint64_t; static_assert(sizeof(type) == 8); };
	template<size_t S> using sized_uint_t = typename sized_uint_type<S>::type;

	namespace math {

		template<typename T = float>
		constexpr T pi = glm::pi<T>();
		template<typename T = float>
		constexpr T tau = glm::two_pi<T>();
		template<typename T = float>
		constexpr T epsilon = glm::epsilon<T>();
		template<typename T = float>
		constexpr T e_constant = glm::e<T>();
		template<typename T = float>
		constexpr T eulers_constant = glm::euler<T>();
		template<typename T = float>
		constexpr T golden_ratio = glm::golden_ratio<T>();
		template<typename T = float>
		constexpr T sqrt2 = glm::root_two<T>();

	}

	// TODO: remove once standard library math functions are made constexpr
	template<typename T, typename U, typename = std::enable_if_t<std::is_floating_point_v<std::common_type_t<T, U>>>>
	constexpr auto fmod(const T f, const U m) noexcept {
		return f - (m * static_cast<int64_t>(f / m));
	}

	// TODO: remove once standard library math functions are made constexpr
	template<typename T>
	constexpr auto floor(T f) noexcept ->
			std::enable_if_t<std::numeric_limits<T>::is_iec559, T> {
		// casting to int truncates the value, which is floor(f) for positive values,
		// but we have to substract 1 for negative values (unless f is already floored)
		const auto i = static_cast<int64_t>(f);
		const auto floored = static_cast<T>(i);
		return ((f >= T{0}) ? floored : ((f == floored) ? f : floored - T{1}));
	}
	// TODO: remove once standard library math functions are made constexpr
	template<typename T>
	constexpr auto ceil(T f) noexcept ->
			std::enable_if_t<std::numeric_limits<T>::is_iec559, T> {
		// casting to int truncates the value, which is ceil(val) for negative values,
		// but we have to add 1 for positive values (unless val is already ceiled == recasted int val)
		const auto i = static_cast<int64_t>(f);
		const auto ceiled = static_cast<T>(i);
		return ((f < T{0}) ? ceiled : ((f == ceiled) ? f : ceiled + T{1}));
	}

	// ((i % m) + m) % m
	// can still be negative if both arguments are negative
	template<typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && !std::is_unsigned_v<U>>>
	constexpr auto pmod(const T i, const U m) noexcept {
		if constexpr (std::is_floating_point_v<std::common_type_t<T, U>>) {
			auto r = fmod(i, m);
			return r + ((r < 0) * m);
		} else if constexpr (std::is_unsigned_v<T>) {
			return i % m;
		} else {
			return ((i % m) + m) % m;
		}
	}

	template<typename T>
	constexpr auto is_pow2(T i) noexcept ->
			std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, bool> {
		return (i != 0) && !(i & (i - 1));
	}

	template<typename T>
	constexpr auto constexpr_floor_log2(T i) noexcept ->
			std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, T> {
		if (i == 0) {
			return std::numeric_limits<T>::max();
		}
		T l { 0 };
		for (; i > 1; i >>= 1, l++);
		return l;
	}

	// helper type trait for variadic min() to determine if glm::min can be called with arguments of the provided types
	template<typename, typename, typename = std::void_t<>>
	struct is_glm_min_invocable : std::false_type {};
	template<typename T0, typename T1>
	struct is_glm_min_invocable<T0, T1, std::void_t<
		decltype(glm::min(std::declval<std::decay_t<T0&&>>, std::declval<std::decay_t<T1&&>>))
	>> : std::true_type {};
	template<typename T0, typename T1>
	inline constexpr bool is_glm_min_invocable_v = is_glm_min_invocable<T0, T1>::value;

	// helper type trait for variadic max() to determine if glm::max can be called with arguments of the provided types
	template<typename, typename, typename = std::void_t<>>
	struct is_glm_max_invocable : std::false_type {};
	template<typename T0, typename T1>
	struct is_glm_max_invocable<T0, T1, std::void_t<
		decltype(glm::max(std::declval<std::decay_t<T0&&>>, std::declval<std::decay_t<T1&&>>))
	>> : std::true_type {};
	template<typename T0, typename T1>
	inline constexpr bool is_glm_max_invocable_v = is_glm_max_invocable<T0, T1>::value;

	// variadic min()
	template<typename T>
	T vmin(T&& t) noexcept { return std::forward<T>(t); }
	// variadic min()
	template<typename T0, typename T1, typename... Ts>
	constexpr std::common_type_t<T0, T1, Ts...> vmin(T0&& val1, T1&& val2, Ts&&... vs) {
		if constexpr (is_glm_min_invocable_v<T0, T1>) {
			return vmin(glm::min(val1, val2), std::forward<Ts>(vs)...);
		} else {
			return (val1 < val2) ? vmin(val1, std::forward<Ts>(vs)...) : vmin(val2, std::forward<Ts>(vs)...);
		}
	}

	// variadic max()
	template<typename T>
	T vmax(T&& t) noexcept { return std::forward<T>(t); }
	// variadic max()
	template<typename T0, typename T1, typename... Ts>
	constexpr std::common_type_t<T0, T1, Ts...> vmax(T0&& val1, T1&& val2, Ts&&... vs) {
		if constexpr (is_glm_max_invocable_v<T0, T1>) {
			return vmax(glm::max(val1, val2), std::forward<Ts>(vs)...);
		} else {
			return (val1 > val2) ? vmax(val1, std::forward<Ts>(vs)...) : vmax(val2, std::forward<Ts>(vs)...);
		}
	}

	template<typename T, glm::qualifier Q>
	T getArea(const glm::vec<1, T, Q>& rectSize) noexcept { return rectSize.x; }
	template<typename T, glm::qualifier Q>
	T getArea(const glm::vec<2, T, Q>& rectSize) noexcept { return rectSize.x * rectSize.y; }
	template<typename T, glm::qualifier Q>
	T getArea(const glm::vec<3, T, Q>& rectSize) noexcept { return rectSize.x * rectSize.y * rectSize.z; }
	template<typename T, glm::qualifier Q>
	T getArea(const glm::vec<4, T, Q>& rectSize) noexcept { return rectSize.x * rectSize.y * rectSize.z * rectSize.w; }

	template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	constexpr bool epsEqual(const T a, const T b) {
		return glm::equal(a, b, math::epsilon<T>);
	}
	template<glm::length_t L, typename T, glm::qualifier Q, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	constexpr bool epsEqual(const glm::vec<L, T, Q>& a, const glm::vec<L, T, Q>& b) {
		return glm::all(glm::equal(a, b, math::epsilon<T>));
	}
	template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>> * = nullptr>
	constexpr bool epsNotEqual(const T a, const T b) {
		return glm::notEqual(a, b, math::epsilon<T>);
	}
	template<glm::length_t L, typename T, glm::qualifier Q, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	constexpr bool epsNotEqual(const glm::vec<L, T, Q>& a, const glm::vec<L, T, Q>& b) {
		return glm::any(glm::notEqual(a, b, math::epsilon<T>));
	}

	template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>> * = nullptr>
	constexpr bool epsZero(const T a) {
		return glm::equal(a, T { 0 }, math::epsilon<T>);
	}
	template<glm::length_t L, typename T, glm::qualifier Q, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	constexpr bool epsZero(const glm::vec<L, T, Q>& a) {
		return glm::all(glm::equal(a, glm::vec<L, T, Q>{0}, math::epsilon<T>));
	}
	template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>> * = nullptr>
	constexpr bool epsNonzero(const T a) {
		return glm::notEqual(a, T { 0 }, math::epsilon<T>);
	}
	template<glm::length_t L, typename T, glm::qualifier Q, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	constexpr bool epsNonzero(const glm::vec<L, T, Q>& a) {
		return glm::any(glm::notEqual(a, glm::vec<L, T, Q>{0}, math::epsilon<T>));
	}

	template<typename T, std::enable_if_t<(std::is_integral_v<T> && std::is_unsigned_v<T> && !std::is_same_v<bool, T> && ((sizeof(T) == 2) || (sizeof(T) == 4) || (sizeof(T) == 8))), int> = 0>
	inline constexpr T flipBytes(const T& i) noexcept {
		if constexpr (sizeof(T) == 2) {
#if defined(_MSC_VER)
			return _byteswap_ushort(i);
#elif defined(__GNUC__) || defined(__GNUG__)
			return __builtin_bswap16(i);
#else
			return (i << 8) | (i >> 8);
#endif
		} else if constexpr (sizeof(T) == 4) {
#if defined(_MSC_VER)
			return _byteswap_ulong(i);
#elif defined(__GNUC__) || defined(__GNUG__)
			return __builtin_bswap32(i);
#else
			return (((i & 0x000000FFu) << 24) |
					((i & 0x0000FF00u) << 8) |
					((i & 0x00FF0000u) >> 8) |
					((i & 0xFF000000u) >> 24));
#endif
		} else if constexpr (sizeof(T) == 8) {
#if defined(_MSC_VER)
			return _byteswap_uint64(i);
#elif defined(__GNUC__) || defined(__GNUG__)
			return __builtin_bswap64(i);
#else
			return (((i & 0x00000000000000FFull) << 56) |
					((i & 0x000000000000FF00ull) << 40) |
					((i & 0x0000000000FF0000ull) << 24) |
					((i & 0x00000000FF000000ull) << 8) |
					((i & 0x000000FF00000000ull) >> 8) |
					((i & 0x0000FF0000000000ull) >> 24) |
					((i & 0x00FF000000000000ull) >> 40) |
					((i & 0xFF00000000000000ull) >> 56));
#endif
		}
	}

	template<typename T, size_t S = sizeof(T), typename Uint = sized_uint_t<S>, std::enable_if_t<((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<bool, T>), int> = 0>
	constexpr Uint toUnsignedIntBytes(const T& t) noexcept {
		Uint b;
		std::memcpy(&b, &t, S); // use std::memcpy to avoid undefined behavior
		return b;
	}
	template<typename T, size_t S = sizeof(T), typename Uint = sized_uint_t<S>, std::enable_if_t<((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<bool, T>), int> = 0>
	constexpr T fromUnsignedIntBytes(const Uint& b) noexcept {
		T t;
		std::memcpy(&t, &b, S); // use std::memcpy to avoid undefined behavior
		return t;
	}

	template<typename T, size_t S = sizeof(T), typename Uint = sized_uint_t<S>, std::enable_if_t<((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<bool, T>), int> = 0>
	constexpr Uint toBigEndianUintBytes(const T& t) noexcept {
		static_assert((std::endian::native == std::endian::big) || (std::endian::native == std::endian::little));
		if constexpr ((std::endian::native == std::endian::big) || (S == 1)) {
			return toUnsignedIntBytes(t);
		} else {
			return flipBytes(toUnsignedIntBytes(t));
		}
	}
	template<typename T, size_t S = sizeof(T), typename Uint = sized_uint_t<S>, std::enable_if_t<((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<bool, T>), int> = 0>
	constexpr T fromBigEndianUintBytes(const Uint& b) noexcept {
		static_assert((std::endian::native == std::endian::big) || (std::endian::native == std::endian::little));
		if constexpr ((std::endian::native == std::endian::big) || (S == 1)) {
			return fromUnsignedIntBytes(b);
		} else {
			return fromUnsignedIntBytes(flipBytes(b));
		}
	}

}