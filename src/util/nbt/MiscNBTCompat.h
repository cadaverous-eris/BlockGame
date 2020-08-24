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
        template<typename U>
        using to_nbt_type = std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), std::conditional_t<
            !(std::is_same_v<U, nbt_byte> || std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>), std::conditional_t<
                !(std::is_same_v<U, nbt_int> || std::is_same_v<U, uint32_t>), std::conditional_t<
                    !(std::is_same_v<U, nbt_long> || std::is_same_v<U, uint64_t>),
                        nbt_list , nbt_long_array >, nbt_int_array >, nbt_byte_array
        >>;
        template<typename U = T>
		static to_nbt_type<U> toNBT(const Vec& vec) {
            if constexpr (std::is_same_v<U, nbt_byte>) {
                return nbt_byte_array(vec.begin(), vec.end());
            } else if constexpr (std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>) {
                nbt_byte_array nbtArr {};
                nbtArr.resize(vec.size());
                for (size_t i = 0; i < vec.size(); i++)
                    nbtArr[i] = static_cast<nbt_byte>(vec[i]);
                return nbtArr;
            } else if constexpr (std::is_same_v<U, nbt_int>) {
                return nbt_int_array(vec.begin(), vec.end());
            } else if constexpr (std::is_same_v<U, uint32_t>) {
                nbt_int_array nbtArr {};
                nbtArr.resize(vec.size());
                for (size_t i = 0; i < vec.size(); i++)
                    nbtArr[i] = static_cast<nbt_int>(vec[i]);
                return nbtArr;
            } else if constexpr (std::is_same_v<U, nbt_long>) {
                return nbt_long_array(vec.begin(), vec.end());
            } else if constexpr (std::is_same_v<U, uint64_t>) {
                nbt_long_array nbtArr {};
                nbtArr.resize(vec.size());
                for (size_t i = 0; i < vec.size(); i++)
                    nbtArr[i] = static_cast<nbt_long>(vec[i]);
                return nbtArr;
            } else {
                return nbt_list(vec.begin(), vec.end());
            }
        }
		template<typename U = T, std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static Vec fromNBT(const NBT& nbt) {
            if constexpr (std::is_same_v<U, nbt_byte>) {
                if (nbt.isByteArray()) return nbt.getByteArray();
            } else if constexpr (std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>) {
                if (nbt.isByteArray()) {
                    const nbt_byte_array& nbtArr = nbt.asByteArray();
                    Vec vec {};
				    vec.resize(nbtArr.size());
                    for (size_t i = 0; i < nbtArr.size(); i++)
					    vec[i] = static_cast<U>(nbtArr[i]);
                    return vec;
                }
            } else if constexpr (std::is_same_v<U, nbt_int>) {
                if (nbt.isIntArray()) return nbt.getIntArray();
            } else if constexpr (std::is_same_v<U, uint32_t>) {
                if (nbt.isIntArray()) {
                    const nbt_int_array& nbtArr = nbt.asIntArray();
                    Vec vec {};
				    vec.resize(nbtArr.size());
                    for (size_t i = 0; i < nbtArr.size(); i++)
					    vec[i] = static_cast<U>(nbtArr[i]);
                    return vec;
                }
            } else if constexpr (std::is_same_v<U, nbt_long>) {
                if (nbt.isLongArray()) return nbt.getLongArray();
            } else if constexpr (std::is_same_v<U, uint64_t>) {
                if (nbt.isLongArray()) {
                    const nbt_long_array& nbtArr = nbt.asLongArray();
                    Vec vec {};
				    vec.resize(nbtArr.size());
                    for (size_t i = 0; i < nbtArr.size(); i++)
					    vec[i] = static_cast<U>(nbtArr[i]);
                    return vec;
                }
            }
			if (nbt.isList()) return nbt.getListOf<T>();
			throw bad_nbt_cast();
		}
		template<typename U = T, std::enable_if_t<(is_testable_nbt_type_v<U> || is_nbt_type_v<U>), int> = 0>
		static bool nbtIs(const NBT& nbt) noexcept {
            if constexpr (std::is_same_v<U, nbt_byte> || std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>) {
                if (nbt.isByteArray()) return true;
            } else if constexpr (std::is_same_v<U, nbt_int> || std::is_same_v<U, uint32_t>) {
                if (nbt.isIntArray()) return true;
            } else if constexpr (std::is_same_v<U, nbt_long> || std::is_same_v<U, uint64_t>) {
                if (nbt.isLongArray()) return true;
            }
			if (nbt.isList()) return nbt.asList().is<T>();
            return false;
		}
	};

	// NBT compat for std::array<T, Size>
	template<typename T, size_t S>
	struct NBTTypeCompat<std::array<T, S>> {
		using Arr = std::array<T, S>;
        template<typename U>
        using to_nbt_type = std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), std::conditional_t<
            !(std::is_same_v<U, nbt_byte> || std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>), std::conditional_t<
                !(std::is_same_v<U, nbt_int> || std::is_same_v<U, uint32_t>), std::conditional_t<
                    !(std::is_same_v<U, nbt_long> || std::is_same_v<U, uint64_t>),
                        nbt_list , nbt_long_array >, nbt_int_array >, nbt_byte_array
        >>;
        template<typename U = T>
		static to_nbt_type<U> toNBT(const Arr& arr) {
            if constexpr (std::is_same_v<U, nbt_byte>) {
                return nbt_byte_array(arr.begin(), arr.end());
            } else if constexpr (std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>) {
                nbt_byte_array nbtArr {};
                nbtArr.resize(arr.size());
                for (size_t i = 0; i < arr.size(); i++)
                    nbtArr[i] = static_cast<nbt_byte>(arr[i]);
                return nbtArr;
            } else if constexpr (std::is_same_v<U, nbt_int>) {
                return nbt_int_array(arr.begin(), arr.end());
            } else if constexpr (std::is_same_v<U, uint32_t>) {
                nbt_int_array nbtArr {};
                nbtArr.resize(arr.size());
                for (size_t i = 0; i < arr.size(); i++)
                    nbtArr[i] = static_cast<nbt_int>(arr[i]);
                return nbtArr;
            } else if constexpr (std::is_same_v<U, nbt_long>) {
                return nbt_long_array(arr.begin(), arr.end());
            } else if constexpr (std::is_same_v<U, uint64_t>) {
                nbt_long_array nbtArr {};
                nbtArr.resize(arr.size());
                for (size_t i = 0; i < arr.size(); i++)
                    nbtArr[i] = static_cast<nbt_long>(arr[i]);
                return nbtArr;
            } else {
                return nbt_list(arr.begin(), arr.end());
            }
        }
		template<typename U = T, std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), int> = 0>
		static Arr fromNBT(const NBT& nbt) {
            if constexpr (std::is_same_v<U, nbt_byte> || std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>) {
                if (nbt.isByteArray()) {
                    const nbt_byte_array& nbtArr = nbt.asByteArray();
                    if (nbtArr.size() == S) {
                        Arr arr {};
                        for (size_t i = 0; i < S; i++)
                            arr[i] = static_cast<U>(nbtArr[i]);
                        return arr;
                    }
                }
            } else if constexpr (std::is_same_v<U, nbt_int> || std::is_same_v<U, uint32_t>) {
                if (nbt.isIntArray()) {
                    const nbt_int_array& nbtArr = nbt.asIntArray();
                    if (nbtArr.size() == S) {
                        Arr arr {};
                        for (size_t i = 0; i < S; i++)
                            arr[i] = static_cast<U>(nbtArr[i]);
                        return arr;
                    }
                }
            } else if constexpr (std::is_same_v<U, nbt_long> || std::is_same_v<U, uint64_t>) {
                if (nbt.isLongArray()) {
                    const nbt_long_array& nbtArr = nbt.asLongArray();
                    if (nbtArr.size() == S) {
                        Arr arr {};
                        for (size_t i = 0; i < S; i++)
                            arr[i] = static_cast<U>(nbtArr[i]);
                        return arr;
                    }
                }
            }
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
            if constexpr (std::is_same_v<U, nbt_byte> || std::is_same_v<U, uint8_t> || std::is_same_v<U, unsigned char>) {
                if (nbt.isByteArray() && (nbt.asByteArray().size() == S)) return true;
            } else if constexpr (std::is_same_v<U, nbt_int> || std::is_same_v<U, uint32_t>) {
                if (nbt.isIntArray() && (nbt.asIntArray().size() == S)) return true;
            } else if constexpr (std::is_same_v<U, nbt_long> || std::is_same_v<U, uint64_t>) {
                if (nbt.isLongArray() && (nbt.asLongArray().size() == S)) return true;
            }
			if (nbt.isList()) {
				const nbt_list& nbtList = nbt.asList();
                return (nbtList.size() == S) && nbtList.is<T>();
			}
            return false;
		}
	};

    // NBT compat for glm::vec types
	template<glm::length_t L, typename V, glm::qualifier Q>
	struct NBTTypeCompat<glm::vec<L, V, Q>> {
		using T = glm::vec<L, V, Q>;
		using value_type = V;
        template<typename U>
        using to_nbt_type = std::enable_if_t<(is_convertible_from_nbt_v<U> || is_nbt_type_v<U>), std::conditional_t<
            !(std::is_same_v<U, nbt_byte> || std::is_same_v<U, uint8_t> || std::is_same_v<U, bool>), std::conditional_t<
                !(std::is_same_v<U, nbt_int> || std::is_same_v<U, uint32_t>), std::conditional_t<
                    !(std::is_same_v<U, nbt_long> || std::is_same_v<U, uint64_t>),
                        nbt_list , nbt_long_array >, nbt_int_array >, nbt_byte_array
        >>;
		static constexpr bool is_value_t_byte = std::is_same_v<nbt_byte, value_type>;
        static constexpr bool is_value_t_short = std::is_same_v<nbt_short, value_type>;
        static constexpr bool is_value_t_int = std::is_same_v<nbt_int, value_type>;
        static constexpr bool is_value_t_long = std::is_same_v<nbt_long, value_type>;
		static constexpr bool is_value_t_float = std::is_same_v<nbt_float, value_type>;
        static constexpr bool is_value_t_double = std::is_same_v<nbt_double, value_type>;
        template<typename U = value_type>
		static to_nbt_type<U> toNBT(const T& vec) {
            if constexpr (std::is_same_v<U, nbt_byte>) {
                return nbt_byte_array(&vec[0], &vec[0] + vec.length());
            } else if constexpr (std::is_same_v<U, uint8_t> || std::is_same_v<U, bool>) {
                nbt_byte_array nbtArr {};
                nbtArr.resize(vec.length());
                for (size_t i; i < vec.length(); i++)
                    nbtArr[i] = static_cast<nbt_byte>(vec[i]);
                return nbtArr;
            } else if constexpr (std::is_same_v<U, nbt_int>) {
                return nbt_int_array(&vec[0], &vec[0] + vec.length());
            } else if constexpr (std::is_same_v<uint32_t, value_type>) {
                nbt_int_array nbtArr {};
                nbtArr.resize(vec.length());
                for (size_t i; i < vec.length(); i++)
                    nbtArr[i] = static_cast<nbt_int>(vec[i]);
                return nbtArr;
            } else if constexpr (std::is_same_v<U, nbt_long>) {
                return nbt_long_array(&vec[0], &vec[0] + vec.length());
            } else if constexpr (std::is_same_v<uint64_t, value_type>) {
                nbt_long_array nbtArr {};
                nbtArr.resize(vec.length());
                for (size_t i; i < vec.length(); i++)
                    nbtArr[i] = static_cast<nbt_long>(vec[i]);
                return nbtArr;
            } else {
                return nbt_list(&vec[0], &vec[0] + vec.length());
            }
        }
		static T fromNBT(const NBT& nbt) {
            if constexpr (is_value_t_byte || std::is_same_v<uint8_t, value_type> || std::is_same_v<bool, value_type>) {
                if (nbt.isByteArray()) {
                    const nbt_byte_array& nbtArr = nbt.asByteArray();
                    if (nbtArr.size() == eng::glm_type<T>::components) {
                        T vec {};
                        for (size_t i = 0; i < vec.length(); i++)
                            vec[i] = static_cast<value_type>(nbtArr[i]);
                        return vec;
                    }
                }
            } else if constexpr (is_value_t_int || std::is_same_v<uint32_t, value_type>) {
                if (nbt.isIntArray()) {
                    const nbt_int_array& nbtArr = nbt.asIntArray();
                    if (nbtArr.size() == eng::glm_type<T>::components) {
                        T vec {};
                        for (size_t i = 0; i < vec.length(); i++)
                            vec[i] = static_cast<value_type>(nbtArr[i]);
                        return vec;
                    }
                }
            } else if constexpr (is_value_t_long || std::is_same_v<uint64_t, value_type>) {
                if (nbt.isLongArray()) {
                    const nbt_long_array& nbtArr = nbt.asLongArray();
                    if (nbtArr.size() == eng::glm_type<T>::components) {
                        T vec {};
                        for (size_t i = 0; i < vec.length(); i++)
                            vec[i] = static_cast<value_type>(nbtArr[i]);
                        return vec;
                    }
                }
            }
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
		static bool nbtIs(const NBT& nbt) noexcept {
            if constexpr (is_value_t_byte || std::is_same_v<value_type, uint8_t> || std::is_same_v<value_type, bool>) {
                if (nbt.isByteArray() && (nbt.asByteArray().size() == eng::glm_type<T>::components)) return true;
                if (nbt.isListOf<TagByte>() && (nbt.asListOf<TagByte>().size() == eng::glm_type<T>::components)) return true;
            } else if constexpr (is_value_t_int || std::is_same_v<value_type, uint32_t>) {
                if (nbt.isIntArray() && (nbt.asIntArray().size() == eng::glm_type<T>::components)) return true;
                if (nbt.isListOf<TagInt>() && (nbt.asListOf<TagInt>().size() == eng::glm_type<T>::components)) return true;
            } else if constexpr (is_value_t_long || std::is_same_v<value_type, uint64_t>) {
                if (nbt.isLongArray() && (nbt.asLongArray().size() == eng::glm_type<T>::components)) return true;
                if (nbt.isListOf<TagLong>() && (nbt.asListOf<TagLong>().size() == eng::glm_type<T>::components)) return true;
            } else {
                if (nbt.isList()) {
                    const nbt_list& nbtList = nbt.asList();
                    return (nbtList.size() == eng::glm_type<T>::components) && nbtList.is<value_type>();
                }
            }
            return false;
		}
	};

    // NBT compat for eng::uuid
	template<>
	struct NBTTypeCompat<eng::uuid> {
		using uuid = eng::uuid;
		inline static nbt_long_array toNBT(const uuid& id) {
			const auto [ ab, cd ] = id.getUints();
            return nbt_long_array { static_cast<nbt_long>(ab), static_cast<nbt_long>(cd) };
		}
		static uuid fromNBT(const NBT& nbt) {
            const nbt_long_array& nbtArr = nbt.asLongArray();
            if (nbtArr.size() == 2)
                return uuid::fromUints(static_cast<uint64_t>(nbtArr[0]), static_cast<uint64_t>(nbtArr[1]));
			throw bad_nbt_cast();
		}
		static bool nbtIs(const NBT& nbt) noexcept {
			return nbt.isLongArray() && (nbt.asLongArray().size() == 2);
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
	};


}

#endif