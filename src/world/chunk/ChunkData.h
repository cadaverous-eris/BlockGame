#pragma once

#include <array>
#include <utility>
#include <algorithm>
#include <cstring>
#include <type_traits>

#include <glm/vec3.hpp>

#include "util/math/math.h"


namespace eng {

	class Chunk;

	enum class ChunkDataOrder { ZYX, YZX };

	template<typename StateType, size_t S, ChunkDataOrder Order = ChunkDataOrder::ZYX>
	class ChunkData {
	public:
		using value_type = StateType;

		static constexpr size_t width = S;
		static constexpr size_t layer_size = S * S;
		static constexpr size_t volume = S * S * S;

		using iterator = typename std::array<StateType, volume>::iterator;
		using const_iterator = typename std::array<StateType, volume>::const_iterator;
		using reverse_iterator = typename std::array<StateType, volume>::reverse_iterator;
		using const_reverse_iterator = typename std::array<StateType, volume>::const_reverse_iterator;
	private:
		std::array<StateType, volume> states;

	public:
		explicit ChunkData() : states() {}

		ChunkData(const ChunkData& b) : states(b.states) {}
		template<typename U = StateType, std::enable_if_t<!std::is_standard_layout_v<U> || !std::is_trivially_copyable_v<U>, int> = 0>
		ChunkData(ChunkData&& b) : states(std::move(b.states)) {}
		template<typename U = StateType, std::enable_if_t<std::is_standard_layout_v<U> && std::is_trivially_copyable_v<U>, int> = 0>
		ChunkData(ChunkData&& b) : states() {
			std::memcpy(states.data(), b.states.data(), sizeof(std::array<StateType, volume>));
		}

		ChunkData& operator =(const ChunkData& b) {
			if (&b != this) {
				states = b.states;
			}
			return *this;
		}
		template<typename U = StateType, std::enable_if_t<!std::is_standard_layout_v<U> || !std::is_trivially_copyable_v<U>, int> = 0>
		ChunkData& operator =(ChunkData&& b) noexcept(std::is_nothrow_move_constructible_v<U> && std::is_nothrow_move_assignable_v<U>) {
			if (&b != this) {
				std::swap(states, b.states);
			}
			return *this;
		}
		template<typename U = StateType, std::enable_if_t<std::is_standard_layout_v<U> && std::is_trivially_copyable_v<U>, int> = 0>
		ChunkData& operator =(ChunkData&& b) noexcept {
			if (&b != this) {
				std::memcpy(states.data(), b.states.data(), sizeof(std::array<StateType, volume>));
			}
			return *this;
		}

		static inline constexpr size_t getWidth() noexcept { return width; }
		static inline constexpr size_t getLayerSize() noexcept { return layer_size; }
		static inline constexpr size_t getVolume() noexcept { return volume; }
		inline constexpr size_t size() const noexcept { return volume; }

		static inline constexpr ChunkDataOrder getDataOrder() noexcept { return Order; }

		inline constexpr const StateType* data() const noexcept { return states.data(); }
		inline constexpr StateType* data() noexcept { return states.data(); }

		inline const StateType& operator [](const size_t i) const noexcept { return states[i]; }
		inline StateType& operator [](const size_t i) noexcept { return states[i]; }
		inline const StateType& operator [](const glm::ivec3& p) const noexcept {
			if constexpr (Order == ChunkDataOrder::ZYX)
				return states[(p.z * layer_size) + (p.y * width) + p.x];
			else if constexpr (Order == ChunkDataOrder::YZX)
				return states[(p.y * layer_size) + (p.z * width) + p.x];
			else
				static_assert((Order == ChunkDataOrder::ZYX) || (Order == ChunkDataOrder::YZX));
		}
		inline StateType& operator [](const glm::ivec3& p) noexcept {
			if constexpr (Order == ChunkDataOrder::ZYX)
				return states[(p.z * layer_size) + (p.y * width) + p.x];
			else if constexpr (Order == ChunkDataOrder::YZX)
				return states[(p.y * layer_size) + (p.z * width) + p.x];
			else
				static_assert((Order == ChunkDataOrder::ZYX) || (Order == ChunkDataOrder::YZX));
		}

		inline const StateType& at(const size_t i) const { return states.at(i); }
		inline StateType& at(const size_t i) { return states.at(i); }
		inline const StateType& at(const glm::ivec3& p) const {
			if constexpr (Order == ChunkDataOrder::ZYX)
				return states.at((p.z * layer_size) + (p.y * width) + p.x);
			else if constexpr (Order == ChunkDataOrder::YZX)
				return states.at((p.y * layer_size) + (p.z * width) + p.x);
			else
				static_assert((Order == ChunkDataOrder::ZYX) || (Order == ChunkDataOrder::YZX));
		}
		inline StateType& at(const glm::ivec3& p) {
			if constexpr (Order == ChunkDataOrder::ZYX)
				return states.at((p.z * layer_size) + (p.y * width) + p.x);
			else if constexpr (Order == ChunkDataOrder::YZX)
				return states.at((p.y * layer_size) + (p.z * width) + p.x);
			else
				static_assert((Order == ChunkDataOrder::ZYX) || (Order == ChunkDataOrder::YZX));
		}

		inline void fill(const StateType& v) { states.fill(v); }

		inline bool empty() const {
			return std::all_of(begin(), end(), [](const StateType& state) -> bool { return state.isEmpty(); });
		}

		inline iterator begin() { return states.begin(); }
		inline iterator end() { return states.end(); }
		inline const_iterator begin() const { return states.cbegin(); }
		inline const_iterator end() const { return states.cend(); }
		inline const_iterator cbegin() const { return states.cbegin(); }
		inline const_iterator cend() const { return states.cend(); }

		inline reverse_iterator rbegin() { return states.rbegin(); }
		inline reverse_iterator rend() { return states.rend(); }
		inline const_reverse_iterator rbegin() const { return states.crbegin(); }
		inline const_reverse_iterator rend() const { return states.crend(); }
		inline const_reverse_iterator crbegin() const { return states.crbegin(); }
		inline const_reverse_iterator crend() const { return states.crend(); }

	};

}