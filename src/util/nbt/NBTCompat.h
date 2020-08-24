#ifndef NBT_COMPAT_H
#define NBT_COMPAT_H

#include <type_traits>

#include "nbt_types.h"

namespace nbt {

    class NBT;

    template<typename T, typename = void>
	struct NBTTypeCompat;

    template<typename T, typename = void>
	struct is_convertible_to_nbt : std::false_type {};
	template<typename T>
	struct is_convertible_to_nbt<T, std::enable_if_t<
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		is_nbt_type_v<decltype(NBTTypeCompat<T>::toNBT(std::declval<T>()))>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_convertible_to_nbt_v = is_convertible_to_nbt<T>::value;

	template<typename T, typename = void>
	struct is_convertible_from_nbt : std::false_type {};
	template<typename T>
	struct is_convertible_from_nbt<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(NBTTypeCompat<T>::fromNBT(std::declval<NBT>())), T>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_convertible_from_nbt_v = is_convertible_from_nbt<T>::value;

	template<typename T, typename = void>
	struct is_testable_nbt_type : std::false_type {};
	template<typename T>
	struct is_testable_nbt_type<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(NBTTypeCompat<std::remove_cvref_t<T>>::nbtIs(std::declval<NBT>())), bool>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_testable_nbt_type_v = is_testable_nbt_type<T>::value;

	template<typename T, typename = void>
	struct is_testable_nbt_list_type : std::false_type {};
	template<typename T>
	struct is_testable_nbt_list_type<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(NBTTypeCompat<std::remove_cvref_t<T>>::nbtListIs(std::declval<nbt_list>())), bool>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_testable_nbt_list_type_v = is_testable_nbt_list_type<T>::value;
	template<typename T, typename = void>
	struct is_testable_nbt_byte_array_type : std::false_type {};
	template<typename T>
	struct is_testable_nbt_byte_array_type<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(NBTTypeCompat<std::remove_cvref_t<T>>::nbtByteArrayIs(std::declval<nbt_byte_array>())), bool>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_testable_nbt_byte_array_type_v = is_testable_nbt_byte_array_type<T>::value;
	template<typename T, typename = void>
	struct is_testable_nbt_int_array_type : std::false_type {};
	template<typename T>
	struct is_testable_nbt_int_array_type<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(NBTTypeCompat<std::remove_cvref_t<T>>::nbtIntArrayIs(std::declval<nbt_int_array>())), bool>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_testable_nbt_int_array_type_v = is_testable_nbt_int_array_type<T>::value;
	template<typename T, typename = void>
	struct is_testable_nbt_long_array_type : std::false_type {};
	template<typename T>
	struct is_testable_nbt_long_array_type<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(NBTTypeCompat<std::remove_cvref_t<T>>::nbtLongArrayIs(std::declval<nbt_long_array>())), bool>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_testable_nbt_long_array_type_v = is_testable_nbt_long_array_type<T>::value;

	template<typename T, typename = void>
	struct to_nbt_conversion_result_type {};
	template<typename T>
	struct to_nbt_conversion_result_type<T, std::enable_if_t<is_convertible_to_nbt_v<T>>> {
		using type = decltype(NBTTypeCompat<T>::toNBT(std::declval<T>()));
	};
	template<typename T>
	struct to_nbt_conversion_result_type<T, std::enable_if_t<is_nbt_type_v<T>>> {
		using type = T;
	};
	template<typename T>
	using to_nbt_conversion_result_type_t = typename to_nbt_conversion_result_type<T>::type;

}

#endif