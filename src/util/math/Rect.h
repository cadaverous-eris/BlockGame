#pragma once

#include <type_traits>
#include <limits>
#include <utility>

#include <glm/vec2.hpp>

#include "util/math/math.h"

namespace eng {

    template<typename T = float, typename = void>
	struct Rect;

    template<typename T>
    struct Rect<T, typename std::enable_if_t<std::is_arithmetic_v<T>>> {
        using vec_type = glm::vec<2, T>;

        vec_type offset { 0, 0 };
        vec_type size { 0, 0 };

        constexpr Rect(const vec_type& offset, const vec_type& size) noexcept : offset(offset), size(size) {}
        constexpr Rect(const vec_type& size) noexcept : size(size) {}
        constexpr Rect() noexcept = default;

        constexpr Rect(const Rect&) noexcept = default;
		constexpr Rect(Rect&&) noexcept = default;
		constexpr Rect& operator =(const Rect&) noexcept = default;
		constexpr Rect& operator =(Rect&&) noexcept = default;

        template<typename U>
		explicit Rect(const std::enable_if_t<!std::is_same_v<T, U> && std::is_convertible_v<glm::vec<2, U>, vec_type>, Rect<U>>& b) :
				offset(static_cast<vec_type>(b.offset)), size(static_cast<vec_type>(b.size)) {}

        static constexpr Rect fromPoints(const vec_type& p1, const vec_type& p2) noexcept {
            return { p1, p2 - p1 };
        }


        T& getX() noexcept { return offset.x; }
        const T& getX() const noexcept { return offset.x; }
        T& getY() noexcept { return offset.y; }
        const T& getY() const noexcept { return offset.y; }
        T& getWidth() noexcept { return size.x; }
        const T& getWidth() const noexcept { return size.x; }
        T& getHeight() noexcept { return size.y; }
        const T& getHeight() const noexcept { return size.y; }

        const T& getX1() const noexcept { return offset.x; }
        const T& getY1() const noexcept { return offset.y; }
        T getX2() const noexcept { return offset.x + size.x; }
        T getY2() const noexcept { return offset.y + size.y; }

        const vec_type& getP1() const noexcept { return offset; }
        vec_type getP2() const noexcept { return offset + size; }

		vec_type getCenter() const noexcept {
			return (size / T(2)) + offset;
		}

		T getArea() const noexcept {
			return size.x * size.y;
		}

        template<typename U>
		bool contains(const glm::vec<2, U>& p) const noexcept {
			return (static_cast<bool>(*this) &&
					(p.x >= offset.x && p.x <= (offset.x + size.x)) &&
					(p.y >= offset.y && p.y <= (offset.y + size.y)));
		}
		template<typename U>
		bool contains(const Rect<U>& b) const noexcept {
			return (static_cast<bool>(b) && contains(b.offset) && contains(b.offset + b.size));
		}
		template<typename U>
		bool intersects(const Rect<U>& b) const noexcept {
			return (static_cast<bool>(b) && static_cast<bool>(*this) &&
					((offset.x < (b.offset.x + b.size.x)) && ((offset.x + size.x) > b.offset.x)) &&
					((offset.y < (b.offset.y + b.size.y)) && ((offset.y + size.y) > b.offset.y)));
		}

		operator bool() const noexcept {
			return static_cast<bool>(size.x) && static_cast<bool>(size.y);
		}

        bool operator ==(const Rect<T>& b) const noexcept {
			return (offset == b.offset) && (size == b.size);
		}
		bool operator !=(const Rect<T>& b) const noexcept {
			return (offset != b.offset) || (size != b.size);
		}

        Rect<T> operator +() const noexcept { return *this; }
		Rect<T> operator -() const noexcept { return { offset + size, -size }; }

        // offset rect
		template<typename U>
		auto operator +=(const glm::vec<2, U>& offset) noexcept ->
				std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, vec_type>, Rect<T>&> {
			const auto o = static_cast<vec_type>(offset);
			offset += o;
			return *this;
		}
		// offset rect
		template<typename U>
		auto operator -=(const glm::vec<2, U>& offset) noexcept ->
				std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, vec_type>, Rect<T>&> {
			const auto o = static_cast<vec_type>(offset);
			offset -= o;
			return *this;
		}

        // scale rect
		template<typename U>
		auto operator *=(const glm::vec<2, U>& scale) noexcept ->
				std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, vec_type>, Rect<T>&> {
			const auto s = static_cast<vec_type>(scale);
			offset *= s;
			size *= s;
			return *this;
		}
		// scale rect
		template<typename U>
		auto operator *=(const U scale) noexcept ->
				std::enable_if_t<std::is_convertible_v<U, T>, Rect<T>&> {
			const auto s = static_cast<T>(scale);
			offset *= s;
			size *= s;
			return *this;
		}
		// scale rect
		template<typename U>
		auto operator /=(const glm::vec<2, U>& scale) noexcept ->
				std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, vec_type>, Rect<T>&> {
			const auto s = static_cast<vec_type>(scale);
			offset /= s;
			size /= s;
			return *this;
		}
		// scale rect
		template<typename U>
		auto operator /=(const U scale) noexcept ->
				std::enable_if_t<std::is_convertible_v<U, T>, Rect<T>&> {
			const auto s = static_cast<T>(scale);
			offset /= s;
			size /= s;
			return *this;
		}

        // union of 2 rects
		template<typename U>
		auto operator |=(const Rect<U>& b) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, vec_type>, Rect<T>&> {
            const vec_type newOffset = glm::min(offset, static_cast<vec_type>(b.offset));
			size = glm::max(size + offset, static_cast<vec_type>(b.size + b.offset)) - newOffset;
			offset = newOffset;
			return *this;
		}
		// intersection of 2 rects
		template<typename U>
		auto operator &=(const Rect<U>& b) ->
				std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, vec_type>, Rect<T>&> {
            const vec_type newOffset = glm::max(offset, static_cast<vec_type>(b.offset));
			size = glm::min(size + offset, static_cast<vec_type>(b.size + b.offset)) - newOffset;
			offset = newOffset;
			return *this;
		}

        template<typename U>
		auto getExpanded(const U d) const noexcept -> Rect<std::common_type_t<U, T>> {
			using ret_val_type = std::common_type_t<U, T>;
			using ret_vec_type = glm::vec<2, ret_val_type>;
			return {
				static_cast<ret_vec_type>(offset) - static_cast<ret_val_type>(d),
				static_cast<ret_vec_type>(size) + (static_cast<ret_val_type>(d) * ret_val_type(2)),
			};
		}
		template<typename U>
		auto getExpanded(const glm::vec<2, U>& d) const noexcept -> Rect<std::common_type_t<U, T>> {
			using ret_val_type = std::common_type_t<U, T>;
			using ret_vec_type = glm::vec<2, ret_val_type>;
			return {
				static_cast<ret_vec_type>(offset) - static_cast<ret_vec_type>(d),
				static_cast<ret_vec_type>(size) + (static_cast<ret_vec_type>(d) * ret_val_type(2)),
			};
		}
		template<typename U>
		auto getContracted(const U d) const noexcept -> Rect<std::common_type_t<U, T>> {
			return getExpanded(-d);
		}
		template<typename U>
		auto getContracted(const glm::vec<2, U>& d) const noexcept -> Rect<std::common_type_t<U, T>> {
			return getExpanded(-d);
		}

    };

    // offset rect
	template<typename T, typename U>
	auto operator +(Rect<T> lhs, const glm::vec<2, U>& offset) noexcept ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return lhs += offset;
	}
    // offset rect
	template<typename T, typename U>
	auto operator +(const glm::vec<2, U>& offset, Rect<T> rhs) noexcept ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return rhs += offset;
	}
	// offset rect
	template<typename T, typename U>
	auto operator -(Rect<T> lhs, const glm::vec<2, U>& offset) noexcept ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return lhs -= offset;
	}

	// scale rect
	template<typename T, typename U>
	auto operator *(Rect<T> lhs, const glm::vec<2, U>& scale) noexcept ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return lhs *= scale;
	}
	// scale rect
	template<typename T, typename U>
	auto operator *(Rect<T> lhs, const U scale) noexcept ->
			std::enable_if_t<std::is_convertible_v<U, T>, Rect<T>> {
		return lhs *= scale;
	}
    // scale rect
	template<typename T, typename U>
	auto operator *(const glm::vec<2, U>& scale, Rect<T> rhs) noexcept ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return rhs *= scale;
	}
	// scale rect
	template<typename T, typename U>
	auto operator *(const U scale, Rect<T> rhs) noexcept ->
			std::enable_if_t<std::is_convertible_v<U, T>, Rect<T>> {
		return rhs *= scale;
	}
	// scale rect
	template<typename T, typename U>
	auto operator /(Rect<T> lhs, const glm::vec<2, U>& scale) noexcept ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return lhs /= scale;
	}
	// scale rect
	template<typename T, typename U>
	auto operator /(Rect<T> lhs, const U scale) noexcept ->
			std::enable_if_t<std::is_convertible_v<U, T>, Rect<T>> {
		return lhs /= scale;
	}

	// union of 2 rects
	template<typename T, typename U>
	auto operator |(Rect<T> lhs, const Rect<U>& rhs) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return lhs |= rhs;
	}
	// intersection of 2 rects
	template<typename T, typename U>
	auto operator &(Rect<T> lhs, const Rect<U>& rhs) ->
			std::enable_if_t<std::is_convertible_v<glm::vec<2, U>, typename Rect<T>::vec_type>, Rect<T>> {
		return lhs &= rhs;
	}

    using RectF = Rect<float>;
	using RectD = Rect<double>;
	using RectI8 = Rect<int8_t>;
	using RectI16 = Rect<int16_t>;
	using RectI32 = Rect<int32_t>;
	using RectI64 = Rect<int64_t>;
	using RectI = RectI32;

}