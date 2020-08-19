#include <cstdint>
#include <type_traits>
#include <cstring>
#include <span>
#include <bit>

namespace eng {

    template<size_t S> struct sized_uint_type;
	template<> struct sized_uint_type<1> { using type = uint8_t; static_assert(sizeof(type) == 1); };
	template<> struct sized_uint_type<2> { using type = uint16_t; static_assert(sizeof(type) == 2); };
	template<> struct sized_uint_type<4> { using type = uint32_t; static_assert(sizeof(type) == 4); };
	template<> struct sized_uint_type<8> { using type = uint64_t; static_assert(sizeof(type) == 8); };
	template<size_t S> using sized_uint_t = typename sized_uint_type<S>::type;

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
    template<typename T, size_t S = sizeof(T), typename Span = std::span<unsigned char, S>, std::enable_if_t<((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<bool, T> && ((S == 1) || (S == 2) || (S == 4) || (S == 8))), int> = 0>
	constexpr T fromByteSpan(const Span& b) noexcept {
		T t;
		std::memcpy(&t, b.data(), S); // use std::memcpy to avoid undefined behavior
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
    template<typename T, size_t S = sizeof(T), typename Span = std::span<unsigned char, S>, typename Uint = sized_uint_t<S>, std::enable_if_t<((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<bool, T>), int> = 0>
	constexpr T fromBigEndianByteSpan(const Span& b) noexcept {
		static_assert((std::endian::native == std::endian::big) || (std::endian::native == std::endian::little));
        if constexpr ((std::endian::native == std::endian::big) || (S == 1)) {
			return fromByteSpan(b);
		} else {
            Uint u;
	        std::memcpy(&u, b.data(), S);
			return fromUnsignedIntBytes(flipBytes(u));
		}
	}

}