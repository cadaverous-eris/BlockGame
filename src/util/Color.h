#pragma once

#include <cstdint>
#include <array>
#include <type_traits>
#include <map>
#include <string>
#include <iostream>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/fwd.hpp>

#include "util/math/math.h"

namespace eng {

	struct Color {
		uint8_t r, g, b, a = 0xFF;

		constexpr Color() noexcept : r(0xFF), g(0xFF), b(0xFF), a(0xFF) {}
		constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) noexcept : r(r), g(g), b(b), a(a) {}
		constexpr Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 0xFF) noexcept : Color(
			static_cast<uint8_t>(r & 0xFF),
			static_cast<uint8_t>(g & 0xFF),
			static_cast<uint8_t>(b & 0xFF),
			static_cast<uint8_t>(a & 0xFF)
		) {}
		constexpr Color(int r, int g, int b, int a = 0xFF) noexcept : Color(
			static_cast<uint8_t>(r & 0xFF),
			static_cast<uint8_t>(g & 0xFF),
			static_cast<uint8_t>(b & 0xFF),
			static_cast<uint8_t>(a & 0xFF)
		) {}
		constexpr Color(float r, float g, float b, float a = 1.0f) noexcept : Color(
			static_cast<uint8_t>(glm::clamp(r, 0.0f, 1.0f) * 0xFF),
			static_cast<uint8_t>(glm::clamp(g, 0.0f, 1.0f) * 0xFF),
			static_cast<uint8_t>(glm::clamp(b, 0.0f, 1.0f) * 0xFF),
			static_cast<uint8_t>(glm::clamp(a, 0.0f, 1.0f) * 0xFF)
		) {}
		explicit constexpr Color(const uint8_t v) noexcept : Color(v, v, v) {}
		explicit constexpr Color(const float v) noexcept : Color(static_cast<uint8_t>(glm::clamp(v, 0.0f, 1.0f) * 0xFF)) {}
		constexpr Color(const glm::u8vec4& v) noexcept : Color(v.x, v.y, v.z, v.w) {}
		constexpr Color(const glm::u8vec3& v) noexcept : Color(v.x, v.y, v.z) {}
		constexpr Color(const glm::vec4& v) noexcept : Color(v.x, v.y, v.z, v.w) {}
		constexpr Color(const glm::vec3& v) noexcept : Color(v.x, v.y, v.z) {}

		constexpr bool operator ==(const Color& rhs) const noexcept {
			return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
		}
		constexpr bool operator !=(const Color& rhs) const noexcept {
			return (r != rhs.r) || (g != rhs.g) || (b != rhs.b) || (a != rhs.a);
		}

		inline constexpr operator glm::u8vec4() const noexcept { return { r, g, b, a }; }
		inline explicit constexpr operator glm::u8vec3() const noexcept { return { r, g, b }; }
		inline constexpr operator glm::vec4() const noexcept {
			return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
		}
		inline explicit constexpr operator glm::vec3() const noexcept {
			return { r / 255.0f, g / 255.0f, b / 255.0f };
		}
		inline explicit constexpr operator glm::ivec4() const noexcept { return { r, g, b, a }; }
		inline explicit constexpr operator glm::ivec3() const noexcept { return { r, g, b }; }
		inline explicit constexpr operator glm::uvec4() const noexcept { return { r, g, b, a }; }
		inline explicit constexpr operator glm::uvec3() const noexcept { return { r, g, b }; }
		inline explicit constexpr operator glm::i16vec4() const noexcept { return { r, g, b, a }; }
		inline explicit constexpr operator glm::i16vec3() const noexcept { return { r, g, b }; }
		inline explicit constexpr operator glm::u16vec4() const noexcept { return { r, g, b, a }; }
		inline explicit constexpr operator glm::u16vec3() const noexcept { return { r, g, b }; }

		constexpr Color& operator *=(const Color& rhs) noexcept {
			r = static_cast<uint8_t>(((r / 255.0f) * (rhs.r / 255.0f)) * 0xFF);
			g = static_cast<uint8_t>(((g / 255.0f) * (rhs.g / 255.0f)) * 0xFF);
			b = static_cast<uint8_t>(((b / 255.0f) * (rhs.b / 255.0f)) * 0xFF);
			a = static_cast<uint8_t>(((a / 255.0f) * (rhs.a / 255.0f)) * 0xFF);
			return *this;
		}
		constexpr Color operator *(const Color& rhs) const noexcept {
			return {
				static_cast<uint8_t>(((r / 255.0f) * (rhs.r / 255.0f)) * 0xFF),
				static_cast<uint8_t>(((g / 255.0f) * (rhs.g / 255.0f)) * 0xFF),
				static_cast<uint8_t>(((b / 255.0f) * (rhs.b / 255.0f)) * 0xFF),
				static_cast<uint8_t>(((a / 255.0f) * (rhs.a / 255.0f)) * 0xFF),
			};
		}
		constexpr Color& operator /=(const Color& rhs) noexcept {
			r = static_cast<uint8_t>(glm::clamp((r / 255.0f) / (rhs.r / 255.0f), 0.0f, 1.0f) * 0xFF);
			g = static_cast<uint8_t>(glm::clamp((g / 255.0f) / (rhs.g / 255.0f), 0.0f, 1.0f) * 0xFF);
			b = static_cast<uint8_t>(glm::clamp((b / 255.0f) / (rhs.b / 255.0f), 0.0f, 1.0f) * 0xFF);
			a = static_cast<uint8_t>(glm::clamp((a / 255.0f) / (rhs.a / 255.0f), 0.0f, 1.0f) * 0xFF);
			return *this;
		}
		constexpr Color operator /(const Color& rhs) const noexcept {
			return {
				(r / 255.0f) / (rhs.r / 255.0f),
				(g / 255.0f) / (rhs.g / 255.0f),
				(b / 255.0f) / (rhs.b / 255.0f),
				(a / 255.0f) / (rhs.a / 255.0f),
			};
		}
		constexpr Color& operator +=(const Color& rhs) noexcept {
			r = static_cast<uint8_t>(glm::clamp((r / 255.0f) + (rhs.r / 255.0f), 0.0f, 1.0f) * 0xFF);
			g = static_cast<uint8_t>(glm::clamp((g / 255.0f) + (rhs.g / 255.0f), 0.0f, 1.0f) * 0xFF);
			b = static_cast<uint8_t>(glm::clamp((b / 255.0f) + (rhs.b / 255.0f), 0.0f, 1.0f) * 0xFF);
			a = static_cast<uint8_t>(glm::clamp((a / 255.0f) + (rhs.a / 255.0f), 0.0f, 1.0f) * 0xFF);
			return *this;
		}
		constexpr Color operator +(const Color& rhs) const noexcept {
			return static_cast<glm::vec4>(*this) + static_cast<glm::vec4>(rhs);
		}
		constexpr Color& operator -=(const Color& rhs) noexcept {
			r = static_cast<uint8_t>(glm::clamp((r / 255.0f) - (rhs.r / 255.0f), 0.0f, 1.0f) * 0xFF);
			g = static_cast<uint8_t>(glm::clamp((g / 255.0f) - (rhs.g / 255.0f), 0.0f, 1.0f) * 0xFF);
			b = static_cast<uint8_t>(glm::clamp((b / 255.0f) - (rhs.b / 255.0f), 0.0f, 1.0f) * 0xFF);
			a = static_cast<uint8_t>(glm::clamp((a / 255.0f) - (rhs.a / 255.0f), 0.0f, 1.0f) * 0xFF);
			return *this;
		}
		constexpr Color operator -(const Color& rhs) const noexcept {
			return static_cast<glm::vec4>(*this) - static_cast<glm::vec4>(rhs);
		}

		constexpr Color& operator |=(const Color& rhs) noexcept {
			r = r | rhs.r;
			g = g | rhs.g;
			b = b | rhs.b;
			a = a | rhs.a;
			return *this;
		}
		constexpr Color operator |(const Color& rhs) const noexcept {
			return { r | rhs.r, g | rhs.g, b | rhs.b, a | rhs.a };
		}
		constexpr Color& operator &=(const Color& rhs) noexcept {
			r = r & rhs.r;
			g = g & rhs.g;
			b = b & rhs.b;
			a = a & rhs.a;
			return *this;
		}
		constexpr Color operator &(const Color& rhs) const noexcept {
			return { r & rhs.r, g & rhs.g, b & rhs.b, a & rhs.a };
		}
		constexpr Color& operator ^=(const Color& rhs) {
			r = r ^ rhs.r;
			g = g ^ rhs.g;
			b = b ^ rhs.b;
			a = a ^ rhs.a;
			return *this;
		}
		constexpr Color operator ^(const Color& rhs) const noexcept {
			return { r ^ rhs.r, g ^ rhs.g, b ^ rhs.b, a ^ rhs.a };
		}
		constexpr Color operator ~() const noexcept {
			return { ~r, ~g, ~b, ~a };
		}

		static constexpr Color mix(const Color& a, const Color& b, float t) noexcept {
			t = glm::clamp(t, 0.0f, 1.0f);
			const glm::vec4 av = a;
			const glm::vec4 bv = b;
			return av + ((bv - av) * t);
		}

		static Color fromString(std::string_view str, std::string_view hexPrefix = "#");
		static bool isValidColorString(std::string_view str, std::string_view hexPrefix = "#");
		std::string toString(std::string_view hexPrefix = "#") const;
		inline explicit operator std::string() const { return toString(); }

		// returns the hue as a float in the range [0, 360)
		constexpr float getHue() const noexcept {
			const float red = r / 255.0f;
			const float grn = g / 255.0f;
			const float blu = b / 255.0f;
			const float max = vmax(red, grn, blu);
			const float min = vmin(red, grn, blu);
			if (max == min) return 0;
			float hue { 0 };
			if (max == red)
				hue = ((grn - blu) / (max - min)) * 60.0f;
			else if (max == grn)
				hue = (2 + ((blu - red) / (max - min))) * 60.0f;
			else
				hue = (4 + ((red - grn) / (max - min))) * 60.0f;
			hue = pmod(hue, 360.0f);
			return hue;
		}

		constexpr float getLuma() const noexcept {
			const auto wr = 0.299f * (r / 255.0f);
			const auto wg = 0.587f * (g / 255.0f);
			const auto wb = 0.114f * (b / 255.0f);
			return wr + wg + wb;
		}

		constexpr Color toGrayScale() const noexcept {
			const auto luma = getLuma();
			return { luma, luma, luma, (a / 255.0f) };
		}

		struct hsl;
		struct hsv;
		struct hsl {
			float h, s, l, a = 1.0f;

			constexpr hsl() noexcept : hsl(0.0f, 1.0f, 1.0f) {}
			constexpr hsl(const float h, const float s, const float l, const float a = 1.0f) noexcept :
					h(pmod(h, 360.0f)),
					s(glm::clamp(s, 0.0f, 1.0f)),
					l(glm::clamp(l, 0.0f, 1.0f)),
					a(glm::clamp(a, 0.0f, 1.0f)) {}
			constexpr hsl(const glm::vec4& v) noexcept : hsl(v.x, v.y, v.z, v.w) {}
			constexpr hsl(const glm::vec3& v) noexcept : hsl(v.x, v.y, v.z) {}

			explicit constexpr hsl(const Color& color) noexcept : hsl() {
				const float r = color.r / 255.0f;
				const float g = color.g / 255.0f;
				const float b = color.b / 255.0f;
				const float max = vmax(r, g, b);
				const float min = vmin(r, g, b);
				if (max == min) h = 0;
				else if (max == r) h = ((g - b) / (max - min)) * 60.0f;
				else if (max == g) h = (2 + ((b - r) / (max - min))) * 60.0f;
				else			   h = (4 + ((r - g) / (max - min))) * 60.0f;
				h = pmod(h, 360.0f);
				l = (max + min) / 2.0f;
				s = ((max == 0.0f) || (min == 1.0f)) ? 0 : ((max - l) / vmin(l, 1.0f - l));
				a = color.a / 255.0f;
			}
			explicit constexpr hsl(const Color::hsv& hsv) noexcept;

			constexpr operator Color() const noexcept {
				const float hue = pmod(h, 360.0f);
				const float sat = glm::clamp(s, 0.0f, 1.0f);
				const float lit = glm::clamp(l, 0.0f, 1.0f);
				const auto f = [hue, sat, lit](int n) -> float {
					const float k = fmod(n + (hue / 30), 12.0f);
					const float x = sat * vmin(lit, 1 - lit);
					return lit - (x * vmax(vmin(k - 3, 9 - k, 1.0f), -1.0f));
				};
				return { f(0), f(8), f(4), a };
			}

			constexpr bool operator ==(const hsl& rhs) const noexcept {
				return (h == rhs.h) && (s == rhs.s) && (l == rhs.l) && (a == rhs.a);
			}
			constexpr bool operator !=(const hsl& rhs) const noexcept {
				return (h != rhs.h) || (s != rhs.s) || (l != rhs.l) || (a != rhs.a);
			}

			explicit constexpr operator glm::vec4() const noexcept { return { h, s, l, a }; }
			explicit constexpr operator glm::vec3() const noexcept { return { h, s, l }; }
		};
		struct hsv {
			float h, s, v, a = 1.0f;

			constexpr hsv() noexcept : hsv(0.0f, 1.0f, 1.0f) {}
			constexpr hsv(const float h, const float s, const float v, const float a = 1.0f) noexcept :
					h(pmod(h, 360.0f)),
					s(glm::clamp(s, 0.0f, 1.0f)),
					v(glm::clamp(v, 0.0f, 1.0f)),
					a(glm::clamp(a, 0.0f, 1.0f)) {}
			constexpr hsv(const glm::vec4& vec) noexcept : hsv(vec.x, vec.y, vec.z, vec.w) {}
			constexpr hsv(const glm::vec3& vec) noexcept : hsv(vec.x, vec.y, vec.z) {}

			explicit constexpr hsv(const Color& color) noexcept : hsv() {
				const float r = color.r / 255.0f;
				const float g = color.g / 255.0f;
				const float b = color.b / 255.0f;
				const float max = vmax(r, g, b);
				const float min = vmin(r, g, b);
				if (max == min) h = 0;
				else if (max == r) h = ((g - b) / (max - min)) * 60.0f;
				else if (max == g) h = (2 + ((b - r) / (max - min))) * 60.0f;
				else			   h = (4 + ((r - g) / (max - min))) * 60.0f;
				h = pmod(h, 360.0f);
				s = (max == 0.0f) ? 0 : ((max - min) / max);
				v = max;
				a = color.a / 255.0f;
			}
			explicit constexpr hsv(const Color::hsl& hsl) noexcept;

			constexpr operator Color() const noexcept {
				const float hue = pmod(h, 360.0f);
				const float sat = glm::clamp(s, 0.0f, 1.0f);
				const float val = glm::clamp(v, 0.0f, 1.0f);
				const auto f = [hue, sat, val](float n) -> float {
					const float k = fmod(n + (hue / 60.0f), 6.0f);
					return val - (val * sat * vmax(vmin(k, 4 - k, 1.0f), 0.0f));
				};
				return { f(5), f(3), f(1), a };
			}

			constexpr bool operator ==(const hsv& rhs) const noexcept {
				return (h == rhs.h) && (s == rhs.s) && (v == rhs.v) && (a == rhs.a);
			}
			constexpr bool operator !=(const hsv& rhs) const noexcept {
				return (h != rhs.h) || (s != rhs.s) || (v != rhs.v) || (a != rhs.a);
			}

			explicit constexpr operator glm::vec4() const noexcept { return { h, s, v, a }; }
			explicit constexpr operator glm::vec3() const noexcept { return { h, s, v }; }
		};

		struct yuv { // https://en.wikipedia.org/wiki/YUV
			static constexpr float u_max = 0.436f;
			static constexpr float v_max = 0.615f;

			float y, u, v, a = 1.0f;

			constexpr yuv() noexcept : yuv(0.0f, 0.0f, 0.0f) {}
			constexpr yuv(const float y, const float u, const float v, const float a = 1.0f) noexcept :
				y(glm::clamp(y, 0.0f, 1.0f)),
				u(glm::clamp(u, -u_max, u_max)),
				v(glm::clamp(v, -v_max, v_max)),
				a(glm::clamp(a, 0.0f, 1.0f)) {}
			constexpr yuv(const glm::vec4& vec) noexcept : yuv(vec.x, vec.y, vec.z, vec.w) {}
			constexpr yuv(const glm::vec3& vec) noexcept : yuv(vec.x, vec.y, vec.z) {}

			explicit constexpr yuv(const Color& color) noexcept : yuv() {
				y = color.getLuma();
				u = 0.492f * ((color.b / 255.0f) - y);
				v = 0.877f * ((color.r / 255.0f) - y);
				a = color.a / 255.0f;
			}

			constexpr operator Color() const noexcept {
				const float r = y + (1.140f * v);
				const float g = y - (0.395f * u) - (0.581f * v);
				const float b = y + (2.033f * u);
				return { r, g, b, a };
			}

			constexpr bool operator ==(const yuv& rhs) const noexcept {
				return (y == rhs.y) && (u == rhs.u) && (v == rhs.v) && (a == rhs.a);
			}
			constexpr bool operator !=(const yuv& rhs) const noexcept {
				return (y != rhs.y) || (u != rhs.u) || (v != rhs.v) || (a != rhs.a);
			}

			explicit constexpr operator glm::vec4() const noexcept { return { y, u, v, a }; }
			explicit constexpr operator glm::vec3() const noexcept { return { y, u, v }; }
		};

		struct uint_rgba;
		struct uint_argb;
		struct uint_bgra;
		struct uint_abgr;
		struct uint_rgba {
		private:
			uint32_t value;
		public:
			constexpr uint_rgba() noexcept : value(0x000000FFu) {}
			constexpr uint_rgba(const uint32_t value) noexcept : value(value) {}

			explicit constexpr uint_rgba(const Color& color) noexcept :
					value((color.r << 24u) | (color.g << 16u) | (color.b << 8u) | (color.a)) {}

			inline constexpr operator uint32_t() const noexcept { return value; }

			explicit inline constexpr operator Color() const noexcept {
				return Color((value >> 24u) & 0xFFu, (value >> 16u) & 0xFFu, (value >> 8u) & 0xFFu, value & 0xFFu);
			}

			explicit constexpr operator Color::uint_argb() noexcept;
			explicit constexpr operator Color::uint_bgra() noexcept;
			explicit constexpr operator Color::uint_abgr() noexcept;

			inline constexpr bool operator ==(const uint_rgba& rhs) const noexcept { return value == rhs.value; }
			inline constexpr bool operator !=(const uint_rgba& rhs) const noexcept { return value != rhs.value; }
		};
		struct uint_argb {
		private:
			uint32_t value;
		public:
			constexpr uint_argb() noexcept : value(0xFF000000u) {}
			constexpr uint_argb(const uint32_t value) noexcept : value(value) {}

			explicit constexpr uint_argb(const Color& color) noexcept :
				value((color.a << 24u) | (color.r << 16u) | (color.g << 8u) | (color.b)) {}

			inline constexpr operator uint32_t() const noexcept { return value; }

			explicit inline constexpr operator Color() const noexcept {
				return Color((value >> 16u) & 0xFFu, (value >> 8u) & 0xFFu, value & 0xFFu, (value >> 24u) & 0xFFu);
			}

			explicit constexpr operator Color::uint_rgba() noexcept;
			explicit constexpr operator Color::uint_bgra() noexcept;
			explicit constexpr operator Color::uint_abgr() noexcept;

			inline constexpr bool operator ==(const uint_argb& rhs) const noexcept { return value == rhs.value; }
			inline constexpr bool operator !=(const uint_argb& rhs) const noexcept { return value != rhs.value; }
		};
		struct uint_bgra {
		private:
			uint32_t value;
		public:
			constexpr uint_bgra() noexcept : value(0x000000FFu) {}
			constexpr uint_bgra(const uint32_t value) noexcept : value(value) {}

			explicit constexpr uint_bgra(const Color& color) noexcept :
				value((color.b << 24u) | (color.g << 16u) | (color.r << 8u) | (color.a)) {}

			inline constexpr operator uint32_t() const noexcept { return value; }

			explicit inline constexpr operator Color() const noexcept {
				return Color((value >> 8u) & 0xFFu, (value >> 16u) & 0xFFu, (value >> 24u) & 0xFFu, value & 0xFFu);
			}

			explicit constexpr operator Color::uint_rgba() noexcept;
			explicit constexpr operator Color::uint_argb() noexcept;
			explicit constexpr operator Color::uint_abgr() noexcept;

			inline constexpr bool operator ==(const uint_bgra& rhs) const noexcept { return value == rhs.value; }
			inline constexpr bool operator !=(const uint_bgra& rhs) const noexcept { return value != rhs.value; }
		};
		struct uint_abgr {
		private:
			uint32_t value;
		public:
			constexpr uint_abgr() noexcept : value(0xFF000000u) {}
			constexpr uint_abgr(const uint32_t value) noexcept : value(value) {}

			explicit constexpr uint_abgr(const Color& color) noexcept :
				value((color.a << 24u) | (color.b << 16u) | (color.g << 8u) | (color.r)) {}

			inline constexpr operator uint32_t() const noexcept { return value; }

			explicit inline constexpr operator Color() const noexcept {
				return Color(value & 0xFFu, (value >> 8u) & 0xFFu, (value >> 16u) & 0xFFu, (value >> 24u) & 0xFFu);
			}

			explicit constexpr operator Color::uint_rgba() noexcept;
			explicit constexpr operator Color::uint_argb() noexcept;
			explicit constexpr operator Color::uint_bgra() noexcept;

			inline constexpr bool operator ==(const uint_abgr& rhs) const noexcept { return value == rhs.value; }
			inline constexpr bool operator !=(const uint_abgr& rhs) const noexcept { return value != rhs.value; }
		};

	};

	constexpr Color::hsl::hsl(const Color::hsv& hsv) noexcept : h(hsv.h), s(0), l(0), a(hsv.a) {
		const float v = glm::clamp(hsv.v, 0.0f, 1.0f);
		l = v - (v * glm::clamp(hsv.s, 0.0f, 1.0f) / 2);
		if ((l > 0.0f) && (l < 1.0f)) s = (v - l) / vmin(l, 1 - l);
	}
	constexpr Color::hsv::hsv(const Color::hsl& hsl) noexcept : h(hsl.h), s(0), v(0), a(hsl.a) {
		const float l = glm::clamp(hsl.l, 0.0f, 1.0f);
		v = l + (glm::clamp(hsl.s, 0.0f, 1.0f) * vmin(l, 1 - l));
		if (v > 0.0f) s = 2 - (2 * l / v);
	}

	inline constexpr Color::uint_rgba::operator Color::uint_argb() noexcept {
		return ((value & 0xFFu) << 24u) | ((value >> 8u) & 0x00FFFFFFu);
	}
	inline constexpr Color::uint_rgba::operator Color::uint_bgra() noexcept {
		return (value & 0x00FF00FFu) | (((value >> 8u) & 0xFFu) << 24u) | (((value >> 24u) & 0xFFu) << 8u);
	}
	inline constexpr Color::uint_rgba::operator Color::uint_abgr() noexcept {
		return ((value & 0xFFu) << 24u) | (((value >> 8u) & 0xFFu) << 16u) | (((value >> 16u) & 0xFFu) << 8u) | ((value >> 24u) & 0xFFu);
	}

	inline constexpr Color::uint_argb::operator Color::uint_rgba() noexcept {
		return ((value & 0x00FFFFFFu) << 8u) | ((value >> 24u) & 0xFFu);
	}
	inline constexpr Color::uint_argb::operator Color::uint_bgra() noexcept {
		return ((value & 0xFFu) << 24u) | (((value >> 8u) & 0xFFu) << 16u) | (((value >> 16u) & 0xFFu) << 8u) | ((value >> 24u) & 0xFFu);
	}
	inline constexpr Color::uint_argb::operator Color::uint_abgr() noexcept {
		return (value & 0xFF00FF00u) | ((value & 0xFFu) << 16u) | ((value >> 16u) & 0xFFu);
	}

	inline constexpr Color::uint_bgra::operator Color::uint_rgba() noexcept {
		return (value & 0x00FF00FFu) | (((value >> 8u) & 0xFFu) << 24u) | (((value >> 24u) & 0xFFu) << 8u);
	}
	inline constexpr Color::uint_bgra::operator Color::uint_argb() noexcept {
		return ((value & 0xFFu) << 24u) | (((value >> 8u) & 0xFFu) << 16u) | (((value >> 16u) & 0xFFu) << 8u) | ((value >> 24u) & 0xFFu);
	}
	inline constexpr Color::uint_bgra::operator Color::uint_abgr() noexcept {
		return ((value & 0xFFu) << 24u) | ((value >> 8u) & 0x00FFFFFFu);
	}

	inline constexpr Color::uint_abgr::operator Color::uint_rgba() noexcept {
		return ((value & 0xFFu) << 24u) | (((value >> 8u) & 0xFFu) << 16u) | (((value >> 16u) & 0xFFu) << 8u) | ((value >> 24u) & 0xFFu);
	}
	inline constexpr Color::uint_abgr::operator Color::uint_argb() noexcept {
		return (value & 0xFF00FF00u) | ((value & 0xFFu) << 16u) | ((value >> 16u) & 0xFFu);
	}
	inline constexpr Color::uint_abgr::operator Color::uint_bgra() noexcept {
		return ((value & 0x00FFFFFFu) << 8u) | ((value >> 24u) & 0xFFu);
	}


	std::ostream& operator <<(std::ostream& os, const Color& c);


	inline namespace color_literals {

		template<char C>
		constexpr uint8_t getHexValue() noexcept {
			if constexpr (C >= 48 && C <= 57)
				return C - 48; // 0-9
			else if constexpr (C >= 65 && C <= 70)
				return C - 55; // A-F
			else if constexpr (C >= 97 && C <= 102)
				return C - 87; // a-f
			else
				static_assert(C == '0', "");
		}
		constexpr uint8_t toHexByte(uint8_t h) noexcept {
			const float f = glm::clamp(h / 15.0f, 0.0f, 1.0f);
			return static_cast<uint8_t>(f * 0xFF);
		}
		constexpr uint8_t toHexByte(uint8_t h0, uint8_t h1) noexcept {
			return static_cast<uint8_t>((h0 & 0xF) << 4) | (h1 & 0xF);
		}

		/*template<typename... CTs>
		constexpr bool is_valid_color_literal(char c0, char cx, CTs... cs) noexcept {
			constexpr auto hexLength = sizeof...(CTs);
			if constexpr (hexLength != 3 && hexLength != 4 && hexLength != 6 && hexLength != 8) {
				return false;
			} else {
				return c0 == '0' && (cx == 'X' || cx == 'x');
			}
		}*/
		template<char... Cs>
		constexpr bool is_valid_color_literal() noexcept {
			constexpr std::array<char, sizeof...(Cs)> chars { Cs... };
			constexpr auto hexLength = sizeof...(Cs) - 2;
			if constexpr (hexLength != 3 && hexLength != 4 && hexLength != 6 && hexLength != 8) {
				return false;
			} else {
				return chars[0] == '0' && (chars[1] == 'X' || chars[1] == 'x');
			}
		}

		template<char... Cs>
		constexpr std::enable_if_t<is_valid_color_literal<Cs...>(), Color> operator "" _c() noexcept {
			constexpr std::array<char, sizeof...(Cs)> chars { Cs... };
			constexpr auto hexLength = chars.size() - 2;
			if constexpr (hexLength == 3) { // RGB
				return Color {
					toHexByte(getHexValue<chars[2]>()),
					toHexByte(getHexValue<chars[3]>()),
					toHexByte(getHexValue<chars[4]>()),
				};
			} else if constexpr (hexLength == 4) { // RGBA
				return Color {
					toHexByte(getHexValue<chars[2]>()),
					toHexByte(getHexValue<chars[3]>()),
					toHexByte(getHexValue<chars[4]>()),
					toHexByte(getHexValue<chars[5]>()),
				};
			} else if constexpr (hexLength == 6) { // RRGGBB
				return Color {
					toHexByte(getHexValue<chars[2]>(), getHexValue<chars[3]>()),
					toHexByte(getHexValue<chars[4]>(), getHexValue<chars[5]>()),
					toHexByte(getHexValue<chars[6]>(), getHexValue<chars[7]>()),
				};
			} else if constexpr (hexLength == 8) { // RRGGBBAA
				return Color {
					toHexByte(getHexValue<chars[2]>(), getHexValue<chars[3]>()),
					toHexByte(getHexValue<chars[4]>(), getHexValue<chars[5]>()),
					toHexByte(getHexValue<chars[6]>(), getHexValue<chars[7]>()),
					toHexByte(getHexValue<chars[8]>(), getHexValue<chars[9]>()),
				};
			} else {
				static_assert(is_valid_color_literal(Cs...), "Invalid color literal (must be hexadecimal RGB, RGBA, RRGGBB, or RRGGBBAA)");
				//return { 0xFF, 0xFF, 0xFF, 0xFF };
			}
		}

	}


	namespace color {

		constexpr Color transparent = 0x00000000_c;
		constexpr Color black = 0x000000_c;
		constexpr Color silver = 0xC0C0C0_c;
		constexpr Color gray = 0x808080_c;
		constexpr Color white = 0xFFFFFF_c;
		constexpr Color maroon = 0x800000_c;
		constexpr Color red = 0xFF0000_c;
		constexpr Color purple = 0x800080_c;
		constexpr Color fuchsia = 0xFF00FF_c;
		constexpr Color green = 0x008000_c;
		constexpr Color lime = 0x00FF00_c;
		constexpr Color olive = 0x808000_c;
		constexpr Color yellow = 0xFFFF00_c;
		constexpr Color navy = 0x000080_c;
		constexpr Color blue = 0x0000FF_c;
		constexpr Color teal = 0x008080_c;
		constexpr Color aqua = 0x00FFFF_c;
		constexpr Color orange = 0xFFA500_c;
		constexpr Color aliceblue = 0xF0F8FF_c;
		constexpr Color antiquewhite = 0xFAEBD7_c;
		constexpr Color aquamarine = 0x7FFFD4_c;
		constexpr Color azure = 0xF0FFFF_c;
		constexpr Color beige = 0xF5F5DC_c;
		constexpr Color bisque = 0xFFE4C4_c;
		constexpr Color blanchedalmond = 0xFFEBCD_c;
		constexpr Color blueviolet = 0x8A2BE2_c;
		constexpr Color brown = 0xA52A2A_c;
		constexpr Color burlywood = 0xDEB887_c;
		constexpr Color cadetblue = 0x5F9EA0_c;
		constexpr Color chartreuse = 0x7FFF00_c;
		constexpr Color chocolate = 0xD2691E_c;
		constexpr Color coral = 0xFF7F50_c;
		constexpr Color cornflowerblue = 0x6495ED_c;
		constexpr Color cornsilk = 0xFFF8DC_c;
		constexpr Color crimson = 0xDC143C_c;
		constexpr Color cyan = 0x00FFFF_c;
		constexpr Color darkblue = 0x00008B_c;
		constexpr Color darkcyan = 0x008B8B_c;
		constexpr Color darkgoldenrod = 0xB8860B_c;
		constexpr Color darkgray = 0xA9A9A9_c;
		constexpr Color darkgreen = 0x006400_c;
		constexpr Color darkgrey = 0xA9A9A9_c;
		constexpr Color darkkhaki = 0xBDB76B_c;
		constexpr Color darkmagenta = 0x8B008B_c;
		constexpr Color darkolivegreen = 0x556B2F_c;
		constexpr Color darkorange = 0xFF8C00_c;
		constexpr Color darkorchid = 0x9932CC_c;
		constexpr Color darkred = 0x8B0000_c;
		constexpr Color darksalmon = 0xE9967A_c;
		constexpr Color darkseagreen = 0x8FBC8F_c;
		constexpr Color darkslateblue = 0x483D8B_c;
		constexpr Color darkslategray = 0x2F4F4F_c;
		constexpr Color darkslategrey = 0x2F4F4F_c;
		constexpr Color darkturquoise = 0x00CED1_c;
		constexpr Color darkviolet = 0x9400D3_c;
		constexpr Color deeppink = 0xFF1493_c;
		constexpr Color deepskyblue = 0x00BFFF_c;
		constexpr Color dimgray = 0x696969_c;
		constexpr Color dimgrey = 0x696969_c;
		constexpr Color dodgerblue = 0x1E90FF_c;
		constexpr Color firebrick = 0xB22222_c;
		constexpr Color floralwhite = 0xFFFAF0_c;
		constexpr Color forestgreen = 0x228B22_c;
		constexpr Color gainsboro = 0xDCDCDC_c;
		constexpr Color ghostwhite = 0xF8F8FF_c;
		constexpr Color gold = 0xFFD700_c;
		constexpr Color goldenrod = 0xDAA520_c;
		constexpr Color greenyellow = 0xADFF2F_c;
		constexpr Color grey = 0x808080_c;
		constexpr Color honeydew = 0xF0FFF0_c;
		constexpr Color hotpink = 0xFF69B4_c;
		constexpr Color indianred = 0xCD5C5C_c;
		constexpr Color indigo = 0x4B0082_c;
		constexpr Color ivory = 0xFFFFF0_c;
		constexpr Color khaki = 0xF0E68C_c;
		constexpr Color lavender = 0xE6E6FA_c;
		constexpr Color lavenderblush = 0xFFF0F5_c;
		constexpr Color lawngreen = 0x7CFC00_c;
		constexpr Color lemonchiffon = 0xFFFACD_c;
		constexpr Color lightblue = 0xADD8E6_c;
		constexpr Color lightcoral = 0xF08080_c;
		constexpr Color lightcyan = 0xE0FFFF_c;
		constexpr Color lightgoldenrodyellow = 0xFAFAD2_c;
		constexpr Color lightgray = 0xD3D3D3_c;
		constexpr Color lightgreen = 0x90EE90_c;
		constexpr Color lightgrey = 0xD3D3D3_c;
		constexpr Color lightpink = 0xFFB6C1_c;
		constexpr Color lightsalmon = 0xFFA07A_c;
		constexpr Color lightseagreen = 0x20B2AA_c;
		constexpr Color lightskyblue = 0x87CEFA_c;
		constexpr Color lightslategray = 0x778899_c;
		constexpr Color lightslategrey = 0x778899_c;
		constexpr Color lightsteelblue = 0xB0C4DE_c;
		constexpr Color lightyellow = 0xFFFFE0_c;
		constexpr Color limegreen = 0x32CD32_c;
		constexpr Color linen = 0xFAF0E6_c;
		constexpr Color magenta = 0xFF00FF_c;
		constexpr Color mediumaquamarine = 0x66CDAA_c;
		constexpr Color mediumblue = 0x0000CD_c;
		constexpr Color mediumorchid = 0xBA55D3_c;
		constexpr Color mediumpurple = 0x9370DB_c;
		constexpr Color mediumseagreen = 0x3CB371_c;
		constexpr Color mediumslateblue = 0x7B68EE_c;
		constexpr Color mediumspringgreen = 0x00FA9A_c;
		constexpr Color mediumturquoise = 0x48D1CC_c;
		constexpr Color mediumvioletred = 0xC71585_c;
		constexpr Color midnightblue = 0x191970_c;
		constexpr Color mintcream = 0xF5FFFA_c;
		constexpr Color mistyrose = 0xFFE4E1_c;
		constexpr Color moccasin = 0xFFE4B5_c;
		constexpr Color navajowhite = 0xFFDEAD_c;
		constexpr Color oldlace = 0xFDF5E6_c;
		constexpr Color olivedrab = 0x6B8E23_c;
		constexpr Color orangered = 0xFF4500_c;
		constexpr Color orchid = 0xDA70D6_c;
		constexpr Color palegoldenrod = 0xEEE8AA_c;
		constexpr Color palegreen = 0x98FB98_c;
		constexpr Color paleturquoise = 0xAFEEEE_c;
		constexpr Color palevioletred = 0xDB7093_c;
		constexpr Color papayawhip = 0xFFEFD5_c;
		constexpr Color peachpuff = 0xFFDAB9_c;
		constexpr Color peru = 0xCD853F_c;
		constexpr Color pink = 0xFFC0CB_c;
		constexpr Color plum = 0xDDA0DD_c;
		constexpr Color powderblue = 0xB0E0E6_c;
		constexpr Color rosybrown = 0xBC8F8F_c;
		constexpr Color royalblue = 0x4169E1_c;
		constexpr Color saddlebrown = 0x8B4513_c;
		constexpr Color salmon = 0xFA8072_c;
		constexpr Color sandybrown = 0xF4A460_c;
		constexpr Color seagreen = 0x2E8B57_c;
		constexpr Color seashell = 0xFFF5EE_c;
		constexpr Color sienna = 0xA0522D_c;
		constexpr Color skyblue = 0x87CEEB_c;
		constexpr Color slateblue = 0x6A5ACD_c;
		constexpr Color slategray = 0x708090_c;
		constexpr Color slategrey = 0x708090_c;
		constexpr Color snow = 0xFFFAFA_c;
		constexpr Color springgreen = 0x00FF7F_c;
		constexpr Color steelblue = 0x4682B4_c;
		constexpr Color tan = 0xD2B48C_c;
		constexpr Color thistle = 0xD8BFD8_c;
		constexpr Color tomato = 0xFF6347_c;
		constexpr Color turquoise = 0x40E0D0_c;
		constexpr Color violet = 0xEE82EE_c;
		constexpr Color wheat = 0xF5DEB3_c;
		constexpr Color whitesmoke = 0xF5F5F5_c;
		constexpr Color yellowgreen = 0x9ACD32_c;
		constexpr Color rebeccapurple = 0x663399_c;


		struct ColorValuesMapKeyCompare {
			using is_transparent = void;
			inline bool operator ()(const std::string& lhs, const std::string& rhs) const noexcept { return lhs < rhs; }
			inline bool operator ()(const std::string_view lhs, const std::string& rhs) const noexcept { return lhs < rhs; }
			inline bool operator ()(const std::string& lhs, const std::string_view rhs) const noexcept { return lhs < rhs; }
			constexpr bool operator ()(const std::string_view lhs, const std::string_view rhs) const noexcept { return lhs < rhs; }
		};
		inline const std::map<std::string, Color, ColorValuesMapKeyCompare> color_values {
			{ std::string("transparent"), transparent },
			{ std::string("black"), black },
			{ std::string("silver"), silver },
			{ std::string("gray"), gray },
			{ std::string("white"), white },
			{ std::string("maroon"), maroon },
			{ std::string("red"), red },
			{ std::string("purple"), purple },
			{ std::string("fuchsia"), fuchsia },
			{ std::string("green"), green },
			{ std::string("lime"), lime },
			{ std::string("olive"), olive },
			{ std::string("yellow"), yellow },
			{ std::string("navy"), navy },
			{ std::string("blue"), blue },
			{ std::string("teal"), teal },
			{ std::string("aqua"), aqua },
			{ std::string("orange"), orange },
			{ std::string("aliceblue"), aliceblue },
			{ std::string("antiquewhite"), antiquewhite },
			{ std::string("aquamarine"), aquamarine },
			{ std::string("azure"), azure },
			{ std::string("beige"), beige },
			{ std::string("bisque"), bisque },
			{ std::string("blanchedalmond"), blanchedalmond },
			{ std::string("blueviolet"), blueviolet },
			{ std::string("brown"), brown },
			{ std::string("burlywood"), burlywood },
			{ std::string("cadetblue"), cadetblue },
			{ std::string("chartreuse"), chartreuse },
			{ std::string("chocolate"), chocolate },
			{ std::string("coral"), coral },
			{ std::string("cornflowerblue"), cornflowerblue },
			{ std::string("cornsilk"), cornsilk },
			{ std::string("crimson"), crimson },
			{ std::string("cyan"), cyan },
			{ std::string("darkblue"), darkblue },
			{ std::string("darkcyan"), darkcyan },
			{ std::string("darkgoldenrod"), darkgoldenrod },
			{ std::string("darkgray"), darkgray },
			{ std::string("darkgreen"), darkgreen },
			{ std::string("darkgrey"), darkgrey },
			{ std::string("darkkhaki"), darkkhaki },
			{ std::string("darkmagenta"), darkmagenta },
			{ std::string("darkolivegreen"), darkolivegreen },
			{ std::string("darkorange"), darkorange },
			{ std::string("darkorchid"), darkorchid },
			{ std::string("darkred"), darkred },
			{ std::string("darksalmon"), darksalmon },
			{ std::string("darkseagreen"), darkseagreen },
			{ std::string("darkslateblue"), darkslateblue },
			{ std::string("darkslategray"), darkslategray },
			{ std::string("darkslategrey"), darkslategrey },
			{ std::string("darkturquoise"), darkturquoise },
			{ std::string("darkviolet"), darkviolet },
			{ std::string("deeppink"), deeppink },
			{ std::string("deepskyblue"), deepskyblue },
			{ std::string("dimgray"), dimgray },
			{ std::string("dimgrey"), dimgrey },
			{ std::string("dodgerblue"), dodgerblue },
			{ std::string("firebrick"), firebrick },
			{ std::string("floralwhite"), floralwhite },
			{ std::string("forestgreen"), forestgreen },
			{ std::string("gainsboro"), gainsboro },
			{ std::string("ghostwhite"), ghostwhite },
			{ std::string("gold"), gold },
			{ std::string("goldenrod"), goldenrod },
			{ std::string("greenyellow"), greenyellow },
			{ std::string("grey"), grey },
			{ std::string("honeydew"), honeydew },
			{ std::string("hotpink"), hotpink },
			{ std::string("indianred"), indianred },
			{ std::string("indigo"), indigo },
			{ std::string("ivory"), ivory },
			{ std::string("khaki"), khaki },
			{ std::string("lavender"), lavender },
			{ std::string("lavenderblush"), lavenderblush },
			{ std::string("lawngreen"), lawngreen },
			{ std::string("lemonchiffon"), lemonchiffon },
			{ std::string("lightblue"), lightblue },
			{ std::string("lightcoral"), lightcoral },
			{ std::string("lightcyan"), lightcyan },
			{ std::string("lightgoldenrodyellow"), lightgoldenrodyellow },
			{ std::string("lightgray"), lightgray },
			{ std::string("lightgreen"), lightgreen },
			{ std::string("lightgrey"), lightgrey },
			{ std::string("lightpink"), lightpink },
			{ std::string("lightsalmon"), lightsalmon },
			{ std::string("lightseagreen"), lightseagreen },
			{ std::string("lightskyblue"), lightskyblue },
			{ std::string("lightslategray"), lightslategray },
			{ std::string("lightslategrey"), lightslategrey },
			{ std::string("lightsteelblue"), lightsteelblue },
			{ std::string("lightyellow"), lightyellow },
			{ std::string("limegreen"), limegreen },
			{ std::string("linen"), linen },
			{ std::string("magenta"), magenta },
			{ std::string("mediumaquamarine"), mediumaquamarine },
			{ std::string("mediumblue"), mediumblue },
			{ std::string("mediumorchid"), mediumorchid },
			{ std::string("mediumpurple"), mediumpurple },
			{ std::string("mediumseagreen"), mediumseagreen },
			{ std::string("mediumslateblue"), mediumslateblue },
			{ std::string("mediumspringgreen"), mediumspringgreen },
			{ std::string("mediumturquoise"), mediumturquoise },
			{ std::string("mediumvioletred"), mediumvioletred },
			{ std::string("midnightblue"), midnightblue },
			{ std::string("mintcream"), mintcream },
			{ std::string("mistyrose"), mistyrose },
			{ std::string("moccasin"), moccasin },
			{ std::string("navajowhite"), navajowhite },
			{ std::string("oldlace"), oldlace },
			{ std::string("olivedrab"), olivedrab },
			{ std::string("orangered"), orangered },
			{ std::string("orchid"), orchid },
			{ std::string("palegoldenrod"), palegoldenrod },
			{ std::string("palegreen"), palegreen },
			{ std::string("paleturquoise"), paleturquoise },
			{ std::string("palevioletred"), palevioletred },
			{ std::string("papayawhip"), papayawhip },
			{ std::string("peachpuff"), peachpuff },
			{ std::string("peru"), peru },
			{ std::string("pink"), pink },
			{ std::string("plum"), plum },
			{ std::string("powderblue"), powderblue },
			{ std::string("rosybrown"), rosybrown },
			{ std::string("royalblue"), royalblue },
			{ std::string("saddlebrown"), saddlebrown },
			{ std::string("salmon"), salmon },
			{ std::string("sandybrown"), sandybrown },
			{ std::string("seagreen"), seagreen },
			{ std::string("seashell"), seashell },
			{ std::string("sienna"), sienna },
			{ std::string("skyblue"), skyblue },
			{ std::string("slateblue"), slateblue },
			{ std::string("slategray"), slategray },
			{ std::string("slategrey"), slategrey },
			{ std::string("snow"), snow },
			{ std::string("springgreen"), springgreen },
			{ std::string("steelblue"), steelblue },
			{ std::string("tan"), tan },
			{ std::string("thistle"), thistle },
			{ std::string("tomato"), tomato },
			{ std::string("turquoise"), turquoise },
			{ std::string("violet"), violet },
			{ std::string("wheat"), wheat },
			{ std::string("whitesmoke"), whitesmoke },
			{ std::string("yellowgreen"), yellowgreen },
			{ std::string("rebeccapurple"), rebeccapurple },
		};


		/* JS to generate color definitions
		(listIn => {
			listIn = listIn.replace(/\n\r?\((?:[^\n\r\t])+\)/g, ''); // remove (synonym of ____)
			let colorDefs = listIn.split('\n').map(l => l.trim()).filter(l => (!!l)).map(l => {
				if (l.indexOf('\t') !== l.lastIndexOf('\t')) l = l.substring(l.indexOf('\t') + 1);
				const name = l.substring(0, l.indexOf('\t')).trim();
				const hex = l.substring(l.indexOf('#') + 1).trim().toUpperCase();
				return { name, hex };
			}).filter(c => (!!c.name && (name.indexOf(' ') === -1) && !!c.hex && [3,4,6,8].includes(c.hex.length)));
			//colorDefs = colorDefs.sort((a, b) => {
			//	if (a.name < b.name) return -1;
			//	if (a.name > b.name) return 1;
			//	return 0;
			//});
			colorDefs.unshift({ name: 'transparent', hex: '00000000' }); // add 'transparent' color

			const colorDefinitionLines = colorDefs.map(c => `constexpr Color ${c.name} = 0x${c.hex}_c;`).join('\n');
			const colorNameMapLines = colorDefs.map(c => ('\t' + `{ std::string("${c.name}"), ${c.name} },`)).join('\n');

			const colorNameMapKeyCompareDef = "struct ColorValuesMapKeyCompare {\n" +
											  "\tusing is_transparent = void;\n" +
											  "\tinline bool operator ()(const std::string& lhs, const std::string& rhs) const noexcept { return lhs < rhs; }\n" +
											  "\tinline bool operator ()(const std::string_view lhs, const std::string& rhs) const noexcept { return lhs < rhs; }\n" +
											  "\tinline bool operator ()(const std::string& lhs, const std::string_view rhs) const noexcept { return lhs < rhs; }\n" +
											  "\tconstexpr bool operator ()(const std::string_view lhs, const std::string_view rhs) const noexcept { return lhs < rhs; }\n" +
											  "};"

			console.log(colorDefinitionLines + '\n\n\n' + colorNameMapKeyCompareDef + '\ninline const std::map<std::string, Color, ColorValuesMapKeyCompare> color_values {\n' + colorNameMapLines + '\n};\n');
		})(`
		`); // copy and past contents of table from https://developer.mozilla.org/en-US/docs/Web/CSS/color_value into string literal
		*/

	}

}

namespace std {

	template<> struct hash<eng::Color> {
		size_t operator ()(const eng::Color& color) const noexcept {
			return static_cast<size_t>(static_cast<uint32_t>(eng::Color::uint_argb{ color }));
		}
	};

}