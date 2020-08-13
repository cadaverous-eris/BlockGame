#pragma once

#include <array>
#include <vector>
#include <utility>
#include <limits>
#include <variant>
#include <algorithm>

#include <glm/common.hpp>

#include "util/type_traits.h"
#include "util/Color.h"
#include "util/uuid.h"
#include "util/direction.h"
#include "util/text/text_utils.h"
#include "render/world/RenderLayer.h"
#include "JonkCompat.h"
#include "jonk_types.h"
#include "Jonk.h"

#include <iostream>

namespace jonk {

	// TODO: support std::span<T>
	// TODO: support std::unordered_map<std::string, T>
	// TODO: support std::multimap<std::string, T>
	// TODO: support std::unordered_multimap<std::string, T>
	// TODO: support std::map<Integral, T>
	// TODO: support std::unordered_map<Integral, T>
	// TODO: support std::multimap<Integral, T>
	// TODO: support std::unordered_multimap<Integral, T>
	// TODO: support std::set<T>
	// TODO: support std::unordered_set<T>
	// TODO: support std::variant<T...>???

	template<>
	struct JonkTypeCompat<std::monostate> {
		static Jonk toJonk(const std::monostate&) {
			return Jonk(nullptr);
		}
		static std::monostate fromJonk(const Jonk& jonk) {
			if (jonk.isNull() || (jonk.isObject() && jonk.asObject().empty()))
				return {};
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			return jonk.isNull() || (jonk.isObject() && jonk.asObject().empty());
		}
	};

	// Jonk compat for std::vector<T>
	template<typename T>
	struct JonkTypeCompat<std::vector<T>> {
		using Vec = std::vector<T>;
		template<typename U = T, std::enable_if_t<(is_convertible_to_jonk_v<U> || is_jonk_type_v<U>), int> = 0>
		static Jonk toJonk(const Vec& vec) {
			return JonkArray(vec.begin(), vec.end());
		}
		template<typename U = T, std::enable_if_t<(is_convertible_from_jonk_v<U> || is_jonk_type_v<U>), int> = 0>
		static Vec fromJonk(const Jonk& jonk) {
			if (jonk.isArray()) {
				const JonkArray& jArr = jonk.asArray();
				Vec vec {};
				vec.resize(jArr.size());
				for (size_t i = 0; i < jArr.size(); i++)
					vec[i] = jArr[i].get<T>();
				return vec;
			}
			throw bad_jonk_cast();
		}
		template<typename U = T, std::enable_if_t<(is_testable_jonk_type_v<U> || is_jonk_type_v<U>), int> = 0>
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (!jonk.isArray())
				return false;
			const JonkArray& jArr = jonk.asArray();
			return std::all_of(jArr.begin(), jArr.end(), [](const Jonk& j) noexcept -> bool {
				return j.is<T>();
			});
		}
	};

	// Jonk compat for std::array<T, Size>
	template<typename T, size_t S>
	struct JonkTypeCompat<std::array<T, S>> {
		using Arr = std::array<T, S>;
		template<typename U = T, std::enable_if_t<(is_convertible_to_jonk_v<U> || is_jonk_type_v<U>), int> = 0>
		static Jonk toJonk(const Arr& arr) {
			return JonkArray(arr.begin(), arr.end());
		}
		template<typename U = T, std::enable_if_t<(is_convertible_from_jonk_v<U> || is_jonk_type_v<U>), int> = 0>
		static Arr fromJonk(const Jonk& jonk) {
			if (jonk.isArray()) {
				const JonkArray& jArr = jonk.asArray();
				if (jArr.size() == S) {
					Arr arr {};
					for (size_t i = 0; i < S; i++)
						arr[i] = jArr[i].get<T>();
					return arr;
				}
			}
			throw bad_jonk_cast();
		}
		template<typename U = T, std::enable_if_t<(is_testable_jonk_type_v<U> || is_jonk_type_v<U>), int> = 0>
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (!jonk.isArray())
				return false;
			const JonkArray& jArr = jonk.asArray();
			if (jArr.size() != S)
				return false;
			return std::all_of(jArr.begin(), jArr.end(), [](const Jonk& j) noexcept -> bool {
				return j.is<T>();
			});
		}
	};

	// Jonk compat for std::map<std::string, T>
	template<typename T>
	struct JonkTypeCompat<std::map<std::string, T>> {
		using Map = std::map<std::string, T>;
		template<typename U = T, std::enable_if_t<(is_convertible_to_jonk_v<U> || is_jonk_type_v<U>), int> = 0>
		static Jonk toJonk(const Map& map) {
			JonkObject obj {};
			for (const auto& [key, val] : map)
				obj[key] = val;
			return obj;
		}
		template<typename U = T, std::enable_if_t<(is_convertible_from_jonk_v<U> || is_jonk_type_v<U>), int> = 0>
		static Map fromJonk(const Jonk& jonk) {
			if (jonk.isObject()) {
				const JonkObject& jObj = jonk.asObject();
				Map map {};
				for (const auto& [key, val] : jObj)
					map.try_emplace(key, val.template get<U>());
				return map;
			}
			throw bad_jonk_cast();
		}
		template<typename U = T, std::enable_if_t<(is_testable_jonk_type_v<U> || is_jonk_type_v<U>), int> = 0>
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (!jonk.isObject())
				return false;
			const JonkObject& jObj = jonk.asObject();
			return std::all_of(jObj.begin(), jObj.end(), [](const auto& entry) noexcept -> bool {
				return entry.second.template is<U>();
			});
		}
	};

	// Jonk compat for std::pair<T, U>
	template<typename T1, typename T2>
	struct JonkTypeCompat<std::pair<T1, T2>> {
		using Pair = std::pair<T1, T2>;
		template<typename U1 = T1, typename U2 = T2, std::enable_if_t<((is_convertible_to_jonk_v<U1> || is_jonk_type_v<U1>) && (is_convertible_to_jonk_v<U2> || is_jonk_type_v<U2>)), int> = 0>
		static Jonk toJonk(const Pair& pair) {
			return JonkArray { Jonk(pair.first), Jonk(pair.second) };
		}
		template<typename U1 = T1, typename U2 = T2, std::enable_if_t<((is_convertible_from_jonk_v<U1> || is_jonk_type_v<U1>) && (is_convertible_from_jonk_v<U2> || is_jonk_type_v<U2>)), int> = 0>
		static Pair fromJonk(const Jonk& jonk) {
			if (jonk.isArray()) {
				const JonkArray& jArr = jonk.asArray();
				if (jArr.size() == 2)
					return Pair(jArr[0].get<T1>(), jArr[1].get<T2>());
			}
			throw bad_jonk_cast();
		}
		template<typename U1 = T1, typename U2 = T2, std::enable_if_t<((is_testable_jonk_type_v<U1> || is_jonk_type_v<U1>) && (is_testable_jonk_type_v<U2> || is_jonk_type_v<U2>)), int> = 0>
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (!jonk.isArray())
				return false;
			const JonkArray& jArr = jonk.asArray();
			if (jArr.size() != 2)
				return false;
			return jArr[0].is<T1>() && jArr[1].is<T2>();
		}
	};

	// Jonk compat for glm::vec types
	template<glm::length_t L, typename V, glm::qualifier Q>
	struct JonkTypeCompat<glm::vec<L, V, Q>> {
		using T = glm::vec<L, V, Q>;
		using value_type = V;
		static constexpr bool is_value_t_bool = std::is_same_v<bool, value_type>;
		static constexpr bool is_value_t_float = std::numeric_limits<value_type>::is_iec559;
		static constexpr bool is_value_t_signed = std::is_signed_v<value_type>;
		static constexpr bool is_value_t_unsigned = std::is_unsigned_v<value_type>;
		static Jonk toJonk(const T& vec) {
			return JonkArray(&vec[0], &vec[0] + vec.length());
		}
		static T fromJonk(const Jonk& jonk) {
			if (jonk.isArray()) {
				const JonkArray& arr = jonk.asArray();
				if (arr.size() == eng::glm_type<T>::components) {
					T vec {};
					for (auto i = 0; i < vec.length(); i++) {
						const Jonk& j = arr[i];
						if constexpr (is_value_t_bool) {
							if (j.isBool()) {
								vec[i] = static_cast<value_type>(j.asBool());
								continue;
							}
						}
						if constexpr (is_value_t_float || is_value_t_bool) {
							if (j.isFloat()) {
								vec[i] = static_cast<value_type>(j.asFloat());
								continue;
							}
						}
						if constexpr (is_value_t_signed || is_value_t_bool) {
							if (j.isInt()) {
								vec[i] = static_cast<value_type>(j.asInt());
								continue;
							}
						} else if constexpr (is_value_t_unsigned) {
							if (j.isInt() && (j.asInt() >= 0)) {
								vec[i] = static_cast<value_type>(j.asInt());
								continue;
							}
						}
						if constexpr (is_value_t_signed || is_value_t_unsigned) {
							if (j.isUint()) {
								vec[i] = static_cast<value_type>(j.asUint());
								continue;
							}
						}
						throw bad_jonk_cast();
					}
					return vec;
				} else {
					throw bad_jonk_cast();
				}
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (!jonk.isArray())
				return false;
			const JonkArray& arr = jonk.asArray();
			if (arr.size() != eng::glm_type<T>::components)
				return false;
			return std::all_of(arr.begin(), arr.end(), [](const Jonk& j) {
				if constexpr (JonkTypeCompat<T>::is_value_t_bool)
					return j.isBool();
				if constexpr (JonkTypeCompat<T>::is_value_t_float)
					return j.isNumber();
				else if constexpr (JonkTypeCompat<T>::is_value_t_signed)
					return j.isIntegral();
				else
					return j.isUint() || (j.isInt() && (j.asInt() >= 0));
			});
		}
	};

	// Jonk compat for eng::Direction
	template<>
	struct JonkTypeCompat<eng::Direction> {
		using Direction = eng::Direction;
		static Jonk toJonk(const Direction& dir) {
			switch (dir) {
				case Direction::DOWN: return Jonk("down");
				case Direction::UP: return Jonk("up");
				case Direction::NORTH: return Jonk("north");
				case Direction::SOUTH: return Jonk("south");
				case Direction::WEST: return Jonk("west");
				case Direction::EAST: return Jonk("east");
				default: return Jonk(nullptr);
			}
		}
		static Direction fromJonk(const Jonk& jonk) {
			if (jonk.isNull()) return Direction::UNDEFINED;
			if (jonk.isString()) {
				const auto jonkString = eng::toLowerCase(jonk.asString());
				if (jonkString == "down") return Direction::DOWN;
				if (jonkString == "up") return Direction::UP;
				if (jonkString == "north") return Direction::NORTH;
				if (jonkString == "south") return Direction::SOUTH;
				if (jonkString == "west") return Direction::WEST;
				if (jonkString == "east") return Direction::EAST;
				if (jonkString == "undefined") return Direction::UNDEFINED;
				if (jonkString == "null") return Direction::UNDEFINED;
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) {
			if (jonk.isNull()) return true;
			if (jonk.isString()) {
				const auto jonkString = eng::toLowerCase(jonk.asString());
				return ((jonkString == "down") || (jonkString == "up") ||
						(jonkString == "north") || (jonkString == "south") ||
						(jonkString == "west") || (jonkString == "east") ||
						(jonkString == "undefined") || (jonkString == "null"));
			}
			return false;
		}
	};

	// Jonk compat for eng::Axis
	template<>
	struct JonkTypeCompat<eng::Axis> {
		using Axis = eng::Axis;
		static Jonk toJonk(const Axis& axis) {
			switch (axis) {
				case Axis::X: return Jonk("x");
				case Axis::Y: return Jonk("y");
				case Axis::Z: return Jonk("z");
				default: return Jonk(nullptr);
			}
		}
		static Axis fromJonk(const Jonk& jonk) {
			if (jonk.isNull()) return Axis::UNDEFINED;
			if (jonk.isString()) {
				const auto jonkString = eng::toLowerCase(jonk.asString());
				if (jonkString == "x") return Axis::X;
				if (jonkString == "y") return Axis::Y;
				if (jonkString == "z") return Axis::Z;
				if (jonkString == "undefined") return Axis::UNDEFINED;
				if (jonkString == "null") return Axis::UNDEFINED;
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) {
			if (jonk.isNull()) return true;
			if (jonk.isString()) {
				const auto jonkString = eng::toLowerCase(jonk.asString());
				return ((jonkString == "x") || (jonkString == "y") || (jonkString == "z") ||
						(jonkString == "undefined") || (jonkString == "null"));
			}
			return false;
		}
	};

	// Jonk compat for eng::Color
	template<>
	struct JonkTypeCompat<eng::Color> {
		using Color = eng::Color;
		inline static Jonk toJonk(const Color& c) {
			return Jonk(c.toString());
		}
		static inline bool isValidColorUint(const Jonk& jonk) noexcept {
			constexpr auto maxUint32 = std::numeric_limits<uint32_t>::max();
			return (
				(jonk.isUint() && (jonk.asUint() <= static_cast<JonkUint>(maxUint32))) ||
				(jonk.isInt() && (jonk.asInt() >= 0) && (jonk.asInt() <= static_cast<JonkInt>(maxUint32)))
			);
		}
		static Color fromJonk(const Jonk& jonk) {
			if (jonk.isString()) {
				const JonkString& str = jonk.asString();
				if (Color::isValidColorString(str))
					return Color::fromString(str);
			} else if (isValidColorUint(jonk)) {
				const auto c = jonk.get<uint32_t>();
				return {
					static_cast<uint8_t>((c >> 24) & 0xFF),	// r
					static_cast<uint8_t>((c >> 16) & 0xFF),	// g
					static_cast<uint8_t>((c >> 8) & 0xFF),	// b
					static_cast<uint8_t>(c & 0xFF),			// a
				};
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (jonk.isString()) {
				return Color::isValidColorString(jonk.asString());
			}
			return isValidColorUint(jonk);
		}
	};

	// Jonk compat for eng::uuid
	template<>
	struct JonkTypeCompat<eng::uuid> {
		using uuid = eng::uuid;
		using UintPair = std::pair<uint64_t, uint64_t>;
		inline static Jonk toJonk(const uuid& id) {
			return Jonk(id.toString());
		}
		static uuid fromJonk(const Jonk& jonk) {
			if (jonk.isString()) {
				const auto parseResult = uuid::fromString(jonk.asString());
				if (parseResult) return *parseResult;
			} else if (jonk.is<UintPair>()) {
				return uuid::fromUints(jonk.get<UintPair>());
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (jonk.isString()) {
				return uuid::fromString(jonk.asString()).has_value();
			}
			return jonk.is<UintPair>();
		}
	};

	// Jonk compat for eng::RenderLayer
	template<>
	struct JonkTypeCompat<eng::RenderLayer> {
		using RenderLayer = eng::RenderLayer;
		static Jonk toJonk(const RenderLayer& layer) {
			switch (layer) {
				case RenderLayer::Opaque: return Jonk("opaque");
				case RenderLayer::Cutout: return Jonk("cutout");
				case RenderLayer::Transparent: return Jonk("transparent");
				default: return Jonk("opaque");
			}
		}
		static RenderLayer fromJonk(const Jonk& jonk) {
			if (jonk.is<std::underlying_type_t<RenderLayer>>()) {
				const auto jonkInt = jonk.get<std::underlying_type_t<RenderLayer>>();
				if (jonkInt == eng::to_underlying(RenderLayer::Opaque)) return RenderLayer::Opaque;
				if (jonkInt == eng::to_underlying(RenderLayer::Cutout)) return RenderLayer::Cutout;
				if (jonkInt == eng::to_underlying(RenderLayer::Transparent)) return RenderLayer::Transparent;
			} else if (jonk.isString()) {
				const auto jonkString = eng::toLowerCase(jonk.asString());
				if (jonkString == "opaque") return RenderLayer::Opaque;
				if (jonkString == "cutout") return RenderLayer::Cutout;
				if (jonkString == "transparent") return RenderLayer::Transparent;
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) {
			if (jonk.is<std::underlying_type_t<RenderLayer>>()) {
				const auto jonkInt = jonk.get<std::underlying_type_t<RenderLayer>>();
				return ((jonkInt == eng::to_underlying(RenderLayer::Opaque)) ||
						(jonkInt == eng::to_underlying(RenderLayer::Cutout)) ||
						(jonkInt == eng::to_underlying(RenderLayer::Transparent)));
			} else if (jonk.isString()) {
				const auto jonkString = eng::toLowerCase(jonk.asString());
				return (jonkString == "opaque") || (jonkString == "cutout") || (jonkString == "transparent");
			}
			return false;
		}
	};

}