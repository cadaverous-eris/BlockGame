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
#include "render/3d/Vert3D.h"

namespace eng {

	struct UIVertColor {
		static const inline VertexFormat format {
			{	"pos",		VertexAttribType::FLOAT,	3,												},
			{	"color",	VertexAttribType::UINT8,	4,	VertexAttribShaderType::FLOAT_NORMALIZED	},
		};

		glm::vec3 pos { 0.0f, 0.0f, 0.0f };
		Color color = 0xFFF_c;

		explicit UIVertColor(const glm::vec3& pos) noexcept : pos(pos), color(0xFFF_c) {}
		explicit UIVertColor(const glm::vec2& pos) : pos(glm::vec3{ pos, 0.0f }), color(0xFFF_c) {}
		UIVertColor(const glm::vec3& pos, const Color& color) noexcept : pos(pos), color(color) {}
		UIVertColor(const glm::vec2& pos, const Color& color) : UIVertColor(glm::vec3{ pos, 0.0f }, color) {}

		bool operator ==(const UIVertColor& b) const noexcept {
			return (pos == b.pos) && (color == b.color);
		}
		bool operator !=(const UIVertColor& b) const noexcept {
			return (pos != b.pos) || (color != b.color);
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor getTranslated(const glm::vec<3, T, Q>& offset) const noexcept {
			return { pos + static_cast<glm::vec3>(offset), color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor getTranslated(const glm::vec<2, T, Q>& offset) const noexcept {
			return { pos + glm::vec3{ static_cast<glm::vec2>(offset), 0.0f }, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& translate(const glm::vec<3, T, Q>& offset) noexcept {
			pos += static_cast<glm::vec3>(offset);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& translate(const glm::vec<2, T, Q>& offset) noexcept {
			pos += glm::vec3{ static_cast<glm::vec2>(offset), 0.0f };
			return *this;
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor getScaled(const T scale) const noexcept {
			return { pos * static_cast<float>(scale), color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor getScaled(const glm::vec<3, T, Q>& scale) const noexcept {
			return { pos * static_cast<glm::vec3>(scale), color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor getScaled(const glm::vec<2, T, Q>& scale) const noexcept {
			return { pos * glm::vec3{ static_cast<glm::vec2>(scale), 1.0f }, color };
		}
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& scale(const T scale) noexcept {
			pos *= static_cast<float>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& scale(const glm::vec<3, T, Q>& scale) noexcept {
			pos *= static_cast<glm::vec3>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& scale(const glm::vec<2, T, Q>& scale) noexcept {
			pos *= glm::vec3{ static_cast<glm::vec2>(scale), 1.0f };
			return *this;
		}

		// Rotation

		UIVertColor getRotated(const float radians, const glm::vec3& axis = { 0, 0, 1.0f }) const {
			return { glm::mat3(glm::rotate(radians, axis)) * pos, color };
		}
		UIVertColor& rotate(const float radians, const glm::vec3& axis = { 0, 0, 1.0f }) {
			pos = glm::mat3(glm::rotate(radians, axis)) * pos;
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor getRotatedAround(const float radians, const glm::vec<3, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) const {
			const glm::vec3 o = static_cast<glm::vec3>(origin);
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return getTransformed(mat);
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor getRotatedAround(const float radians, const glm::vec<2, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) const {
			const glm::vec3 o { static_cast<glm::vec2>(origin), 0.0f };
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return getTransformed(mat);
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& rotateAround(const float radians, const glm::vec<3, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) {
			const glm::vec3 o = static_cast<glm::vec3>(origin);
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return transform(mat);
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& rotateAround(const float radians, const glm::vec<2, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) {
			const glm::vec3 o { static_cast<glm::vec2>(origin), 0.0f };
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return transform(mat);
		}

		// Transformation

		UIVertColor getTransformed(const glm::mat3& mat) const {
			return { mat * pos, color };
		}
		UIVertColor getTransformed(const glm::mat4& mat) const {
			return { glm::vec3(mat * glm::vec4(pos, 1.0f)), color };
		}
		UIVertColor& transform(const glm::mat3& mat) {
			pos = mat * pos;
			return *this;
		}
		UIVertColor& transform(const glm::mat4& mat) {
			pos = glm::vec3(mat * glm::vec4(pos, 1.0f));
			return *this;
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator +=(const glm::vec<3, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator +=(const glm::vec<2, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator -=(const glm::vec<3, T, Q>& offset) { return this->translate(-static_cast<glm::vec3>(offset)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator -=(const glm::vec<2, T, Q>& offset) { return this->translate(-static_cast<glm::vec2>(offset)); }

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator +(const glm::vec<3, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator +(const glm::vec<2, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator -(const glm::vec<3, T, Q>& offset) const { return getTranslated(-static_cast<glm::vec3>(offset)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator -(const glm::vec<2, T, Q>& offset) const { return getTranslated(-static_cast<glm::vec2>(offset)); }
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertColor> operator +(const glm::vec<3, T, Q>& offset, const UIVertColor& vert) {
			return vert.getTranslated(offset);
		}
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertColor> operator +(const glm::vec<2, T, Q>& offset, const UIVertColor& vert) {
			return vert.getTranslated(offset);
		}
	
		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator *=(const T scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator *=(const glm::vec<3, T, Q>& scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator *=(const glm::vec<2, T, Q>& scale) { return this->scale(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator /=(const T scale) { return this->scale(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator /=(const glm::vec<3, T, Q>& scale) { return this->scale(1.0f / static_cast<glm::vec3>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor& operator /=(const glm::vec<2, T, Q>& scale) { return this->scale(1.0f / static_cast<glm::vec2>(scale)); }

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator *(const T scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator *(const glm::vec<3, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator *(const glm::vec<2, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator /(const T scale) const { return getScaled(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator /(const glm::vec<3, T, Q>& scale) const { return getScaled(1.0f / static_cast<glm::vec3>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertColor operator /(const glm::vec<2, T, Q>& scale) const { return getScaled(1.0f / static_cast<glm::vec2>(scale)); }
		template <typename T>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertColor> operator *(const T scale, const UIVertColor& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertColor> operator *(const glm::vec<3, T, Q>& scale, const UIVertColor& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertColor> operator *(const glm::vec<2, T, Q>& scale, const UIVertColor& vert) {
			return vert.getScaled(scale);
		}
	
		explicit UIVertColor(const VertPos3Color4& v) noexcept : UIVertColor(v.pos, v.color) {}
		explicit operator VertPos3Color4() const { return { pos, color }; }
	};

	struct UIVertTex {
		static const inline VertexFormat format {
			{	"pos",		VertexAttribType::FLOAT,	3,												},
			{	"texCoord",	VertexAttribType::FLOAT,	2,												},
			{	"color",	VertexAttribType::UINT8,	4,	VertexAttribShaderType::FLOAT_NORMALIZED	},
		};

		glm::vec3 pos { 0.0f, 0.0f, 0.0f };
		glm::vec2 texCoord { 0.0f, 0.0f };
		Color color = 0xFFF_c;

		UIVertTex(const glm::vec3& pos, const glm::vec2& texCoord, const Color& color = 0xFFF_c) noexcept :
				pos(pos), texCoord(texCoord), color(color) {}
		UIVertTex(const glm::vec2& pos, const glm::vec2& texCoord, const Color& color = 0xFFF_c) :
				UIVertTex(glm::vec3{ pos, 0.0f }, texCoord, color) {}
		UIVertTex(const UIVertColor& vert, const glm::vec2& texCoord) noexcept :
				UIVertTex(vert.pos, texCoord, vert.color) {}

		bool operator ==(const UIVertTex& b) const noexcept {
			return (pos == b.pos) && (texCoord == b.texCoord) && (color == b.color);
		}
		bool operator !=(const UIVertTex& b) const noexcept {
			return (pos != b.pos) || (texCoord != b.texCoord) || (color != b.color);
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex getTranslated(const glm::vec<3, T, Q>& offset) const noexcept {
			return { pos + static_cast<glm::vec3>(offset), texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex getTranslated(const glm::vec<2, T, Q>& offset) const noexcept {
			return { pos + glm::vec3{ static_cast<glm::vec2>(offset), 0.0f }, texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& translate(const glm::vec<3, T, Q>& offset) noexcept {
			pos += static_cast<glm::vec3>(offset);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& translate(const glm::vec<2, T, Q>& offset) noexcept {
			pos += glm::vec3{ static_cast<glm::vec2>(offset), 0.0f };
			return *this;
		}

		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex getScaled(const T scale) const noexcept {
			return { pos * static_cast<float>(scale), texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex getScaled(const glm::vec<3, T, Q>& scale) const noexcept {
			return { pos * static_cast<glm::vec3>(scale), texCoord, color };
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex getScaled(const glm::vec<2, T, Q>& scale) const noexcept {
			return { pos * glm::vec3{ static_cast<glm::vec2>(scale), 1.0f }, texCoord, color };
		}
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& scale(const T scale) noexcept {
			pos *= static_cast<float>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& scale(const glm::vec<3, T, Q>& scale) noexcept {
			pos *= static_cast<glm::vec3>(scale);
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& scale(const glm::vec<2, T, Q>& scale) noexcept {
			pos *= glm::vec3{ static_cast<glm::vec2>(scale), 1.0f };
			return *this;
		}

		// Rotation

		UIVertTex getRotated(const float radians, const glm::vec3& axis = { 0, 0, 1.0f }) const {
			return { glm::mat3(glm::rotate(radians, axis)) * pos, texCoord, color };
		}
		UIVertTex& rotate(const float radians, const glm::vec3& axis = { 0, 0, 1.0f }) {
			pos = glm::mat3(glm::rotate(radians, axis)) * pos;
			return *this;
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex getRotatedAround(const float radians, const glm::vec<3, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) const {
			const glm::vec3 o = static_cast<glm::vec3>(origin);
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return getTransformed(mat);
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex getRotatedAround(const float radians, const glm::vec<2, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) const {
			const glm::vec3 o { static_cast<glm::vec2>(origin), 0.0f };
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return getTransformed(mat);
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& rotateAround(const float radians, const glm::vec<3, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) {
			const glm::vec3 o = static_cast<glm::vec3>(origin);
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return transform(mat);
		}
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& rotateAround(const float radians, const glm::vec<2, T, Q>& origin, const glm::vec3& axis = { 0, 0, 1.0f }) {
			const glm::vec3 o { static_cast<glm::vec2>(origin), 0.0f };
			const glm::mat4 mat = glm::translate(glm::rotate(glm::translate(o), radians, axis), -o);
			return transform(mat);
		}

		// Transformation

		UIVertTex getTransformed(const glm::mat3& mat) const {
			return { mat * pos, texCoord, color };
		}
		UIVertTex getTransformed(const glm::mat4& mat) const {
			return { glm::vec3(mat * glm::vec4(pos, 1.0f)), texCoord, color };
		}
		UIVertTex& transform(const glm::mat3& mat) {
			pos = mat * pos;
			return *this;
		}
		UIVertTex& transform(const glm::mat4& mat) {
			pos = glm::vec3(mat * glm::vec4(pos, 1.0f));
			return *this;
		}

		// Translation

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator +=(const glm::vec<3, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator +=(const glm::vec<2, T, Q>& offset) { return this->translate(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator -=(const glm::vec<3, T, Q>& offset) { return this->translate(-static_cast<glm::vec3>(offset)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator -=(const glm::vec<2, T, Q>& offset) { return this->translate(-static_cast<glm::vec2>(offset)); }

		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator +(const glm::vec<3, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator +(const glm::vec<2, T, Q>& offset) const { return getTranslated(offset); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator -(const glm::vec<3, T, Q>& offset) const { return getTranslated(-static_cast<glm::vec3>(offset)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator -(const glm::vec<2, T, Q>& offset) const { return getTranslated(-static_cast<glm::vec2>(offset)); }
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertTex> operator +(const glm::vec<3, T, Q>& offset, const UIVertTex& vert) {
			return vert.getTranslated(offset);
		}
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertTex> operator +(const glm::vec<2, T, Q>& offset, const UIVertTex& vert) {
			return vert.getTranslated(offset);
		}
	
		// Dilation

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator *=(const T scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator *=(const glm::vec<3, T, Q>& scale) { return this->scale(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator *=(const glm::vec<2, T, Q>& scale) { return this->scale(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator /=(const T scale) { return this->scale(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator /=(const glm::vec<3, T, Q>& scale) { return this->scale(1.0f / static_cast<glm::vec3>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex& operator /=(const glm::vec<2, T, Q>& scale) { return this->scale(1.0f / static_cast<glm::vec2>(scale)); }

		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator *(const T scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator *(const glm::vec<3, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator *(const glm::vec<2, T, Q>& scale) const { return getScaled(scale); }
		template <typename T, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator /(const T scale) const { return getScaled(1.0f / static_cast<float>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator /(const glm::vec<3, T, Q>& scale) const { return getScaled(1.0f / static_cast<glm::vec3>(scale)); }
		template <typename T, glm::qualifier Q, typename std::enable_if_t<is_float_or_integral_v<T>>* = nullptr>
		UIVertTex operator /(const glm::vec<2, T, Q>& scale) const { return getScaled(1.0f / static_cast<glm::vec2>(scale)); }
		template <typename T>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertTex> operator *(const T scale, const UIVertTex& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertTex> operator *(const glm::vec<3, T, Q>& scale, const UIVertTex& vert) {
			return vert.getScaled(scale);
		}
		template <typename T, glm::qualifier Q>
		friend std::enable_if_t<is_float_or_integral_v<T>, UIVertTex> operator *(const glm::vec<2, T, Q>& scale, const UIVertTex& vert) {
			return vert.getScaled(scale);
		}

		explicit operator UIVertColor() const noexcept { return { pos, color }; }

		explicit UIVertTex(const VertPos3Tex2Color4& v) noexcept : UIVertTex(v.pos, v.texCoord, v.color) {}
		explicit operator VertPos3Tex2Color4() const { return { pos, texCoord, color }; }
	};

}