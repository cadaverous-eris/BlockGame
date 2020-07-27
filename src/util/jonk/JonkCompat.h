#pragma once

#include "util/type_traits.h"
#include "jonk_types.h"

namespace jonk {

	class Jonk;

	template<typename T, typename = void>
	struct JonkTypeCompat;

	template<typename T, typename = void>
	struct is_convertible_to_jonk : std::false_type {};
	template<typename T>
	struct is_convertible_to_jonk<T, std::enable_if_t<
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(JonkTypeCompat<T>::toJonk(std::declval<T>())), Jonk>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_convertible_to_jonk_v = is_convertible_to_jonk<T>::value;

	template<typename T, typename = void>
	struct is_convertible_from_jonk : std::false_type {};
	template<typename T>
	struct is_convertible_from_jonk<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(JonkTypeCompat<T>::fromJonk(std::declval<Jonk>())), T>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_convertible_from_jonk_v = is_convertible_from_jonk<T>::value;

	template<typename T, typename = void>
	struct is_testable_jonk_type : std::false_type {};
	template<typename T>
	struct is_testable_jonk_type<T, std::enable_if_t<
		!std::is_reference_v<T> && !std::is_volatile_v<T> && !std::is_const_v<T> &&
		(!std::is_pointer_v<T> || std::is_null_pointer_v<T>) &&
		std::is_same_v<decltype(JonkTypeCompat<std::remove_cvref_t<T>>::jonkIs(std::declval<Jonk>())), bool>
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_testable_jonk_type_v = is_testable_jonk_type<T>::value;

}