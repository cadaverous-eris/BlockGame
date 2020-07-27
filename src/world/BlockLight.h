#pragma once

#include <cstdint>

#include <glm/vec4.hpp>

#include "util/math/math.h"
#include "util/Color.h"

namespace eng {

	struct BlockLight {
		using data_t = uint16_t;

		union {
			struct {
				data_t sky : 4;
				data_t r : 4;
				data_t g : 4;
				data_t b : 4;
			};
			data_t data;
		};

		constexpr BlockLight() noexcept : data(0) {}
		constexpr BlockLight(const data_t data) noexcept : data(data) {}
		constexpr BlockLight(data_t sky, data_t r, data_t g, data_t b) noexcept :
				sky(sky), r(r), g(g), b(b) {}


        inline constexpr BlockLight getRGBLight() const noexcept { return { 0, r, g, b }; }
        inline constexpr BlockLight getSkyLight() const noexcept { return { sky, 0, 0, 0 }; }

		inline constexpr data_t getMinComponent() const { return vmin(sky, r, g, b); }
		inline constexpr data_t getMaxComponent() const { return vmax(sky, r, g, b); }


		inline bool operator ==(const BlockLight& rhs) const noexcept { return data == rhs.data; }
		inline bool operator !=(const BlockLight& rhs) const noexcept { return data != rhs.data; }

        
		inline static constexpr data_t componentwiseLessThan(const data_t a, const data_t b) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t borrow_guard = 0x2020;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpHalfLT = [](const data_t lhs, const data_t rhs) noexcept -> data_t {
				const data_t d = (((lhs & component_mask) | borrow_guard) - (rhs & component_mask)) & carry_mask;
				return (d >> 1) | (d >> 2) | (d >> 3) | (d >> 4);
			};
			return wlpHalfLT(a, b) | (wlpHalfLT(a >> 4, b >> 4) << 4);
		}
		inline static constexpr data_t componentwiseGreaterThan(const data_t a, const data_t b) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t borrow_guard = 0x2020;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpHalfGT = [](const data_t lhs, const data_t rhs) noexcept -> data_t {
				const data_t d = (((rhs & component_mask) | borrow_guard) - (lhs & component_mask)) & carry_mask;
				return (d >> 1) | (d >> 2) | (d >> 3) | (d >> 4);
			};
			return wlpHalfGT(a, b) | (wlpHalfGT(a >> 4, b >> 4) << 4);
		}

		inline static constexpr data_t componentwiseMax(const data_t a, const data_t b) noexcept {
			return a ^ ((a ^ b) & componentwiseLessThan(a, b));
		}
		inline static constexpr BlockLight componentwiseMax(const BlockLight a, const BlockLight b) noexcept {
			return a.data ^ ((a.data ^ b.data) & componentwiseLessThan(a.data, b.data));
		}
		inline static constexpr data_t componentwiseMin(const data_t a, const data_t b) noexcept {
			return a ^ ((a ^ b) & componentwiseGreaterThan(a, b));
		}
		inline static constexpr BlockLight componentwiseMin(const BlockLight a, const BlockLight b) noexcept {
			return a.data ^ ((a.data ^ b.data) & componentwiseGreaterThan(a.data, b.data));
		}

		inline static constexpr data_t componentwiseSubtract(const data_t a, const data_t b) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t borrow_guard = 0x2020;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpHalfSub = [](const data_t lhs, const data_t rhs) noexcept -> data_t {
				const data_t d = (((lhs & component_mask) | borrow_guard) - (rhs & component_mask)); // compute component-wise difference
				const data_t b = d & carry_mask; // check for underflow
				const data_t l = ~((b >> 1) | (b >> 2) | (b >> 3) | (b >> 4)); // create less-than mask
				return d & l & component_mask; // zero out any components that had to borrow
			};
			return wlpHalfSub(a, b) | (wlpHalfSub(a >> 4, b >> 4) << 4);
		}
		friend inline constexpr BlockLight operator -(const BlockLight a, const BlockLight b) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t borrow_guard = 0x2020;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpHalfSub = [](const data_t lhs, const data_t rhs) noexcept -> data_t {
				const data_t d = (((lhs & component_mask) | borrow_guard) - (rhs & component_mask)); // compute component-wise difference
				const data_t b = d & carry_mask; // check for underflow
				const data_t l = ~((b >> 1) | (b >> 2) | (b >> 3) | (b >> 4)); // create less-than mask
				return d & l & component_mask; // zero out any components that had to borrow
			};
			return BlockLight(wlpHalfSub(a.data, b.data) | (wlpHalfSub(a.data >> 4, b.data >> 4) << 4));
		}

		inline static constexpr data_t componentwiseAdd(const data_t a, const data_t b) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpHalfSub = [](const data_t lhs, const data_t rhs) noexcept -> data_t {
				const data_t s = (lhs & component_mask) + (rhs & component_mask); // compute component-wise sum
				const data_t b = s & carry_mask; // check for overflow
				const data_t l = (b >> 1) | (b >> 2) | (b >> 3) | (b >> 4);
				return (s | l) & component_mask;
			};
			return wlpHalfSub(a, b) | (wlpHalfSub(a >> 4, b >> 4) << 4);
		}
		friend inline constexpr BlockLight operator +(const BlockLight a, const BlockLight b) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpHalfSub = [](const data_t lhs, const data_t rhs) noexcept -> data_t {
				const data_t s = (lhs & component_mask) + (rhs & component_mask); // compute component-wise sum
				const data_t b = s & carry_mask; // check for overflow
				const data_t l = (b >> 1) | (b >> 2) | (b >> 3) | (b >> 4);
				return (s | l) & component_mask;
			};
			return BlockLight(wlpHalfSub(a.data, b.data) | (wlpHalfSub(a.data >> 4, b.data >> 4) << 4));
		}

		inline constexpr static data_t decrementAll(const data_t a) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t borrow_guard = 0x2020;
			constexpr data_t component_decrement = 0x0101;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpDecHalf = [](const data_t x) noexcept -> data_t {
				const data_t d = ((x & component_mask) | borrow_guard) - component_decrement; // compute component-wise decrement
				const data_t b = d & carry_mask; // check for underflow
				return (d + (b >> 4)) & component_mask; // saturate underflowed values
			};
			return wlpDecHalf(a) | (wlpDecHalf(a >> 4) << 4);
		}
		inline constexpr static data_t decrementRGB(const data_t a) noexcept {
			constexpr data_t component_mask = 0x0F0F;
			constexpr data_t borrow_guard = 0x2020;
			constexpr data_t component_decrement = 0x0101;
			constexpr data_t component_decrement_1 = 0x0100;
			constexpr data_t carry_mask = 0x1010;
			constexpr auto wlpDecHalf = [](const data_t x, const data_t compDecr) noexcept -> data_t {
				const data_t d = ((x & component_mask) | borrow_guard) - compDecr; // compute component-wise decrement
				const data_t b = d & carry_mask; // check for underflow
				return (d + (b >> 4)) & component_mask; // saturate underflowed values
			};
			return wlpDecHalf(a, component_decrement_1) | (wlpDecHalf(a >> 4, component_decrement) << 4);
		}

		inline constexpr explicit operator glm::u8vec4() const { return { sky, r, g, b }; }
		inline constexpr explicit operator glm::u16vec4() const { return { sky, r, g, b }; }


		/*
		friend std::ostream& operator <<(std::ostream& ostream, const BlockLight light) {
			constexpr std::array<std::string_view, 16> lut { " 0", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15" };
			return ostream << "{ " << lut[light.sky] << ", " << lut[light.r] << ", " << lut[light.g] << ", " << lut[light.b] << " }";
		}
		*/

	};

	using BlockOpacity = BlockLight;


	inline constexpr BlockOpacity createBlockOpacity(const BlockOpacity::data_t r, const BlockOpacity::data_t g, const BlockOpacity::data_t b) noexcept {
		return BlockOpacity((r + g + b) / 3u, r, g, b);
	}
	inline constexpr BlockOpacity createBlockOpacity(const BlockOpacity::data_t o) noexcept {
		return BlockOpacity(o, o, o, o);
	}
	constexpr BlockOpacity createBlockOpacity(const Color& color) noexcept {
		using data_t = BlockOpacity::data_t;
		const data_t r = color.r >> 4;
		const data_t g = color.g >> 4;
		const data_t b = color.b >> 4;
		return BlockOpacity((r + g + b) / 3u, r, g, b);
	}

}