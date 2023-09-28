#ifndef MISC_NBT_COMPAT_H
#define MISC_NBT_COMPAT_H

#include <array>
#include <vector>
#include <utility>
#include <limits>
#include <algorithm>

#include <glm/common.hpp>

#include "util/type_traits.h"
#include "util/Color.h"
#include "util/uuid.h"
#include "util/direction.h"
#include "NBT.h"
#include "NBTCompat.h"

namespace nbt {

    // TODO: support std::span<T>
    // TODO: support eng::Direction & eng::Axis

    // NBT compat for std::vector<T>
	template<typename T>
	struct NBTTypeCompat<std::vector<T>> {
		using Vec = std::vector<T>;
        template<typename U = T, std::enable_if_t<(is_convertible_to_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static nbt_list toNBT(const Vec& vec) {
            return nbt_list(vec.begin(), vec.end());
        }
		template<typename U = T, std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static Vec fromNBT(const NBT& nbt) {
			if (nbt.isList()) return nbt.getListOf<T>();
			throw bad_nbt_cast();
		}
		template<typename U = T, std::enable_if_t<(is_testable_nbt_type_v<U> || is_nbt_type_v<U>), int> = 0>
		static bool nbtIs(const NBT& nbt) noexcept {
            return nbt.isListOf<T>();
		}
        template<typename U = T, std::enable_if_t<(is_testable_nbt_type_v<U> || is_testable_nbt_list_type_v<U> || is_nbt_type_v<U>), int> = 0>
		static bool nbtListIs(const nbt_list& nbtList) noexcept {
            return nbtList.isLists() && nbtList.isEach<Vec>();
		}
	};

	// NBT compat for std::array<T, Size>
	template<typename T, size_t S>
	struct NBTTypeCompat<std::array<T, S>> {
		using Arr = std::array<T, S>;
        template<typename U = T, std::enable_if_t<(is_convertible_to_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static nbt_list toNBT(const Arr& arr) {
            return nbt_list(arr.begin(), arr.end());
        }
		template<typename U = T, std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static Arr fromNBT(const NBT& nbt) {
			if (nbt.isList()) {
				const nbt_list& nbtList = nbt.asList();
                if (nbtList.size() == S)
                    return std::visit<Arr>([](const auto& list) -> Arr {
                        using NBTListValType = typename std::decay_t<decltype(list)>::value_type;
                        Arr arr {};
                        for (size_t i = 0; i < S; i++) {
                            if constexpr (std::is_same_v<NBTListValType, T>)
                                arr[i] = list[i];
                            else
                                arr[i] = NBT(list[i]).get<T>();
                        }
                        return arr;
                    }, nbtList.asListVariant());
			}
			throw bad_nbt_cast();
		}
		template<typename U = T, std::enable_if_t<(is_testable_nbt_type_v<U> || is_nbt_type_v<U>), int> = 0>
		static bool nbtIs(const NBT& nbt) noexcept {
			if (nbt.isList()) {
				const nbt_list& nbtList = nbt.asList();
                return (nbtList.size() == S) && nbtList.is<T>();
			}
            return false;
		}
        template<typename U = T, std::enable_if_t<(is_testable_nbt_type_v<U> || is_testable_nbt_list_type_v<U> || is_nbt_type_v<U>), int> = 0>
		static bool nbtListIs(const nbt_list& nbtList) noexcept {
            return nbtList.isLists() && nbtList.isEach<Arr>();
		}
	};

    // NBT compat for glm::vec types
	template<glm::length_t L, typename V, glm::qualifier Q>
	struct NBTTypeCompat<glm::vec<L, V, Q>> {
		using T = glm::vec<L, V, Q>;
		using value_type = V;
		static constexpr bool is_value_t_byte = std::is_same_v<nbt_byte, value_type>;
        static constexpr bool is_value_t_short = std::is_same_v<nbt_short, value_type>;
        static constexpr bool is_value_t_int = std::is_same_v<nbt_int, value_type>;
        static constexpr bool is_value_t_long = std::is_same_v<nbt_long, value_type>;
		static constexpr bool is_value_t_float = std::is_same_v<nbt_float, value_type>;
        static constexpr bool is_value_t_double = std::is_same_v<nbt_double, value_type>;
        template<typename U = value_type, std::enable_if_t<(is_convertible_to_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static nbt_list toNBT(const T& vec) {
            return nbt_list(&vec[0], &vec[0] + vec.length());
        }
        template<typename U = value_type, std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static T fromNBT(const NBT& nbt) {
            if (nbt.isList()) {
                const nbt_list& nbtList = nbt.asList();
                if (nbtList.size() == eng::glm_type<T>::components)
                    return std::visit<T>([](const auto& list) -> T {
                        using NBTListValType = typename std::decay_t<decltype(list)>::value_type;
                        T vec {};
                        for (size_t i = 0; i < vec.length(); i++) {
                            if constexpr (std::is_same_v<NBTListValType, value_type>)
                                vec[i] = list[i];
                            else
                                vec[i] = NBT(list[i]).get<value_type>();
                        }
                        return vec;
                    }, nbtList.asListVariant());
            }
            throw bad_nbt_cast();
		}
        template<typename U = value_type, std::enable_if_t<(is_testable_nbt_type_v<U> || is_nbt_type_v<U>), int> = 0>
		static bool nbtIs(const NBT& nbt) noexcept {
            if (nbt.isList()) {
                const nbt_list& nbtList = nbt.asList();
                return (nbtList.size() == eng::glm_type<T>::components) && nbtList.is<value_type>();
            }
            return false;
		}
        template<typename U = value_type, std::enable_if_t<(is_testable_nbt_type_v<U> || is_testable_nbt_list_type_v<U> || is_nbt_type_v<U>), int> = 0>
		static bool nbtListIs(const nbt_list& nbtList) noexcept {
            return nbtList.isLists() && nbtList.isEach<T>();
		}
	};

    // NBT compat for eng::uuid
	template<>
	struct NBTTypeCompat<eng::uuid> {
		using uuid = eng::uuid;
		inline static nbt_list toNBT(const uuid& id) {
			const auto [ ab, cd ] = id.getUints();
            return nbt_list::vector<nbt_long> { static_cast<nbt_long>(ab), static_cast<nbt_long>(cd) };
		}
		static uuid fromNBT(const NBT& nbt) {
            const auto& nbtArr = nbt.asListOfLongs();
            if (nbtArr.size() == 2)
                return uuid::fromUints(static_cast<uint64_t>(nbtArr[0]), static_cast<uint64_t>(nbtArr[1]));
			throw bad_nbt_cast();
		}
		static bool nbtIs(const NBT& nbt) noexcept {
			return nbt.isListOfLongs() && (nbt.asList().size() == 2);
		}
        static bool nbtListIs(const nbt_list& nbtList) noexcept {
            return nbtList.isLists() && nbtList.isEach<uuid>();
        }
	};

    // NBT compat for eng::Color
	template<>
	struct NBTTypeCompat<eng::Color> {
		using Color = eng::Color;
		inline static nbt_int toNBT(const Color& color) {
			return static_cast<nbt_int>(Color::uint_rgba { color });
		}
		inline static Color fromNBT(const NBT& nbt) {
            return Color { Color::uint_rgba { nbt.get<uint32_t>() } };
		}
		inline static bool nbtIs(const NBT& nbt) noexcept {
			return nbt.isInt();
		}
        inline static bool nbtListIs(const nbt_list& nbtList) noexcept {
            return nbtList.isInts();
        }
	};


}

#endif