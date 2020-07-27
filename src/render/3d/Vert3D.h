#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/trigonometric.hpp>

#include "render/VertexFormat.h"
#include "util/Color.h"
#include "util/type_traits.h"

namespace eng {

	struct VertPos3Color4 {
		static const inline VertexFormat format {
			{	"pos",		VertexAttribType::FLOAT,	3,												},
			{	"color",	VertexAttribType::UINT8,	4,	VertexAttribShaderType::FLOAT_NORMALIZED	},
		};

		using vec3 = glm::vec3;

		vec3 pos = vec3(0, 0, 0);
		Color color = color::white;

		VertPos3Color4(const vec3& pos, const Color& color) noexcept : pos(pos), color(color) {}
		explicit VertPos3Color4(const vec3& pos) noexcept : pos(pos) {}
		VertPos3Color4() noexcept {}

		inline bool operator ==(const VertPos3Color4& b) const noexcept {
			return (pos == b.pos) && (color == b.color);
		}
		inline bool operator !=(const VertPos3Color4& b) const noexcept {
			return (pos != b.pos) || (color != b.color);
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 getTranslated(const glm::vec<3, T, Q>& offset) const noexcept {
			return { pos + static_cast<vec3>(offset), color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& translate(const glm::vec<3, T, Q>& offset) noexcept {
			pos += static_cast<vec3>(offset);
			return *this;
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 getScaled(const T scale) const noexcept {
			return { pos * static_cast<float>(scale), color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 getScaled(const glm::vec<3, T, Q>& scale) const noexcept {
			return { pos * static_cast<vec3>(scale), color };
		}
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& scale(const T scale) noexcept {
			pos *= static_cast<float>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& scale(const glm::vec<3, T, Q>& scale) noexcept {
			pos *= static_cast<vec3>(scale);
			return *this;
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& operator +=(const glm::vec<3, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& operator -=(const glm::vec<3, T, Q>& offset) { return this->translate(-static_cast<vec3>(offset)); }

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 operator +(const glm::vec<3, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 operator -(const glm::vec<3, T, Q>& offset) const { return getTranslated(-static_cast<vec3>(offset)); }
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Color4> operator +(const glm::vec<3, T, Q>& offset, const VertPos3Color4& vert) {
			return vert.getTranslated(offset);
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& operator *=(const T scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& operator *=(const glm::vec<3, T, Q>& scale) { return this->scale(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& operator /=(const T scale) { return this->scale(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4& operator /=(const glm::vec<3, T, Q>& scale) { return this->scale(1.0f / static_cast<vec3>(scale)); }

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 operator *(const T scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 operator *(const glm::vec<3, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 operator /(const T scale) const { return getScaled(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Color4 operator /(const glm::vec<3, T, Q>& scale) const { return getScaled(1.0f / static_cast<vec3>(scale)); }
		template <typename T>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Color4> operator *(const T scale, const VertPos3Color4& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Color4> operator *(const glm::vec<3, T, Q>& scale, const VertPos3Color4& vert) {
			return vert.getScaled(scale);
		}
	};

	struct VertPos3Tex2Color4 {
		static const inline VertexFormat format {
			{	"pos",		VertexAttribType::FLOAT,	3,												},
			{	"texCoord",	VertexAttribType::FLOAT,	2,												},
			{	"color",	VertexAttribType::UINT8,	4,	VertexAttribShaderType::FLOAT_NORMALIZED	},
		};

		using vec3 = glm::vec3;
		using vec2 = glm::vec2;

		vec3 pos = vec3(0, 0, 0);
		vec2 texCoord = vec2(0, 0);
		Color color = color::white;

		VertPos3Tex2Color4(const vec3& pos, const vec2& texCoord, const Color& color) noexcept :
			pos(pos), texCoord(texCoord), color(color) {}
		VertPos3Tex2Color4(const vec3& pos, const vec2& texCoord) noexcept :
			pos(pos), texCoord(texCoord) {}
		VertPos3Tex2Color4() noexcept {}

		inline bool operator ==(const VertPos3Tex2Color4& b) const noexcept {
			return (pos == b.pos) && (texCoord == b.texCoord) && (color == b.color);
		}
		inline bool operator !=(const VertPos3Tex2Color4& b) const noexcept {
			return (pos != b.pos) || (texCoord != b.texCoord) || (color != b.color);
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 getTranslated(const glm::vec<3, T, Q>& offset) const noexcept {
			return { pos + static_cast<vec3>(offset), texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& translate(const glm::vec<3, T, Q>& offset) noexcept {
			pos += static_cast<vec3>(offset);
			return *this;
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 getScaled(const T scale) const noexcept {
			return { pos * static_cast<float>(scale), texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 getScaled(const glm::vec<3, T, Q>& scale) const noexcept {
			return { pos * static_cast<vec3>(scale), texCoord, color };
		}
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& scale(const T scale) noexcept {
			pos *= static_cast<float>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& scale(const glm::vec<3, T, Q>& scale) noexcept {
			pos *= static_cast<vec3>(scale);
			return *this;
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& operator +=(const glm::vec<3, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& operator -=(const glm::vec<3, T, Q>& offset) { return this->translate(-static_cast<vec3>(offset)); }

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 operator +(const glm::vec<3, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 operator -(const glm::vec<3, T, Q>& offset) const { return getTranslated(-static_cast<vec3>(offset)); }
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Tex2Color4> operator +(const glm::vec<3, T, Q>& offset, const VertPos3Tex2Color4& vert) {
			return vert.getTranslated(offset);
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& operator *=(const T scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& operator *=(const glm::vec<3, T, Q>& scale) { return this->scale(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& operator /=(const T scale) { return this->scale(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4& operator /=(const glm::vec<3, T, Q>& scale) { return this->scale(1.0f / static_cast<vec3>(scale)); }

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 operator *(const T scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 operator *(const glm::vec<3, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 operator /(const T scale) const { return getScaled(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Tex2Color4 operator /(const glm::vec<3, T, Q>& scale) const { return getScaled(1.0f / static_cast<vec3>(scale)); }
		template <typename T>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Tex2Color4> operator *(const T scale, const VertPos3Tex2Color4& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Tex2Color4> operator *(const glm::vec<3, T, Q>& scale, const VertPos3Tex2Color4& vert) {
			return vert.getScaled(scale);
		}

		inline explicit operator VertPos3Color4() const { return { pos, color }; }
	};

	struct VertPos3Norm3Color4 {
		static const inline VertexFormat format {
			{	"pos",		VertexAttribType::FLOAT,	3,												},
			{	"normal",	VertexAttribType::FLOAT,	3,												},
			{	"color",	VertexAttribType::UINT8,	4,	VertexAttribShaderType::FLOAT_NORMALIZED	},
		};

		using vec3 = glm::vec3;

		vec3 pos = vec3(0, 0, 0);
		vec3 normal = vec3(0, 0, 0);
		Color color = color::white;

		VertPos3Norm3Color4(const vec3& pos, const vec3& normal, const Color& color) noexcept :
			pos(pos), normal(normal), color(color) {}
		VertPos3Norm3Color4(const vec3& pos, const vec3& normal) noexcept :
			pos(pos), normal(normal) {}
		VertPos3Norm3Color4() noexcept {}

		inline bool operator ==(const VertPos3Norm3Color4& b) const noexcept {
			return (pos == b.pos) && (normal == b.normal) && (color == b.color);
		}
		inline bool operator !=(const VertPos3Norm3Color4& b) const noexcept {
			return (pos != b.pos) || (normal != b.normal) || (color != b.color);
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 getTranslated(const glm::vec<3, T, Q>& offset) const noexcept {
			return { pos + static_cast<vec3>(offset), normal, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& translate(const glm::vec<3, T, Q>& offset) noexcept {
			pos += static_cast<vec3>(offset);
			return *this;
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 getScaled(const T scale) const noexcept {
			return { pos * static_cast<float>(scale), normal, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 getScaled(const glm::vec<3, T, Q>& scale) const noexcept {
			return { pos * static_cast<vec3>(scale), normal, color };
		}
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& scale(const T scale) noexcept {
			pos *= static_cast<float>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& scale(const glm::vec<3, T, Q>& scale) noexcept {
			pos *= static_cast<vec3>(scale);
			return *this;
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& operator +=(const glm::vec<3, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& operator -=(const glm::vec<3, T, Q>& offset) { return this->translate(-static_cast<vec3>(offset)); }

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 operator +(const glm::vec<3, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 operator -(const glm::vec<3, T, Q>& offset) const { return getTranslated(-static_cast<vec3>(offset)); }
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Norm3Color4> operator +(const glm::vec<3, T, Q>& offset, const VertPos3Norm3Color4& vert) {
			return vert.getTranslated(offset);
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& operator *=(const T scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& operator *=(const glm::vec<3, T, Q>& scale) { return this->scale(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& operator /=(const T scale) { return this->scale(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4& operator /=(const glm::vec<3, T, Q>& scale) { return this->scale(1.0f / static_cast<vec3>(scale)); }

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 operator *(const T scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 operator *(const glm::vec<3, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 operator /(const T scale) const { return getScaled(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Color4 operator /(const glm::vec<3, T, Q>& scale) const { return getScaled(1.0f / static_cast<vec3>(scale)); }
		template <typename T>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Norm3Color4> operator *(const T scale, const VertPos3Norm3Color4& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Norm3Color4> operator *(const glm::vec<3, T, Q>& scale, const VertPos3Norm3Color4& vert) {
			return vert.getScaled(scale);
		}

		inline explicit operator VertPos3Color4() const { return { pos, color }; }
	};

	struct VertPos3Norm3Tex2Color4 {
		static const inline VertexFormat format {
			{	"pos",		VertexAttribType::FLOAT,	3,												},
			{	"normal",	VertexAttribType::FLOAT,	3,												},
			{	"texCoord",	VertexAttribType::FLOAT,	2,												},
			{	"color",	VertexAttribType::UINT8,	4,	VertexAttribShaderType::FLOAT_NORMALIZED	},
		};

		using vec3 = glm::vec3;
		using vec2 = glm::vec2;

		vec3 pos = vec3(0, 0, 0);
		vec3 normal = vec3(0, 0, 0);
		vec2 texCoord = vec2(0, 0);
		Color color = color::white;

		VertPos3Norm3Tex2Color4(const vec3& pos, const vec3& normal, const vec2& texCoord, const Color& color) noexcept :
			pos(pos), normal(normal), texCoord(texCoord), color(color) {}
		VertPos3Norm3Tex2Color4(const vec3& pos, const vec3& normal, const vec2& texCoord) noexcept :
			pos(pos), normal(normal), texCoord(texCoord) {}
		VertPos3Norm3Tex2Color4() noexcept {}

		inline bool operator ==(const VertPos3Norm3Tex2Color4& b) const noexcept {
			return (pos == b.pos) && (normal == b.normal) && (texCoord == b.texCoord) && (color == b.color);
		}
		inline bool operator !=(const VertPos3Norm3Tex2Color4& b) const noexcept {
			return (pos != b.pos) || (normal != b.normal) || (texCoord != b.texCoord) || (color != b.color);
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 getTranslated(const glm::vec<3, T, Q>& offset) const noexcept {
			return { pos + static_cast<vec3>(offset), normal, texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& translate(const glm::vec<3, T, Q>& offset) noexcept {
			pos += static_cast<vec3>(offset);
			return *this;
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 getScaled(const T scale) const noexcept {
			return { pos * static_cast<float>(scale), normal, texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 getScaled(const glm::vec<3, T, Q>& scale) const noexcept {
			return { pos * static_cast<vec3>(scale), normal, texCoord, color };
		}
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& scale(const T scale) noexcept {
			pos *= static_cast<float>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& scale(const glm::vec<3, T, Q>& scale) noexcept {
			pos *= static_cast<vec3>(scale);
			return *this;
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& operator +=(const glm::vec<3, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& operator -=(const glm::vec<3, T, Q>& offset) { return this->translate(-static_cast<vec3>(offset)); }

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 operator +(const glm::vec<3, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 operator -(const glm::vec<3, T, Q>& offset) const { return getTranslated(-static_cast<vec3>(offset)); }
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Norm3Tex2Color4> operator +(const glm::vec<3, T, Q>& offset, const VertPos3Norm3Tex2Color4& vert) {
			return vert.getTranslated(offset);
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& operator *=(const T scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& operator *=(const glm::vec<3, T, Q>& scale) { return this->scale(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& operator /=(const T scale) { return this->scale(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4& operator /=(const glm::vec<3, T, Q>& scale) { return this->scale(1.0f / static_cast<vec3>(scale)); }

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 operator *(const T scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 operator *(const glm::vec<3, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 operator /(const T scale) const { return getScaled(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		inline VertPos3Norm3Tex2Color4 operator /(const glm::vec<3, T, Q>& scale) const { return getScaled(1.0f / static_cast<vec3>(scale)); }
		template <typename T>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Norm3Tex2Color4> operator *(const T scale, const VertPos3Norm3Tex2Color4& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend inline std::enable_if_t<is_float_or_integral_v<T>, VertPos3Norm3Tex2Color4> operator *(const glm::vec<3, T, Q>& scale, const VertPos3Norm3Tex2Color4& vert) {
			return vert.getScaled(scale);
		}

		inline explicit operator VertPos3Color4() const { return { pos, color }; }
		inline explicit operator VertPos3Tex2Color4() const { return { pos, texCoord, color }; }
		inline explicit operator VertPos3Norm3Color4() const { return { pos, normal, color }; }
	};

}