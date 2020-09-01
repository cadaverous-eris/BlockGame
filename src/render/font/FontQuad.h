#pragma once

#include <cstdint>
#include <array>
#include <algorithm>
#include <type_traits>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/fwd.hpp>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>

#include "render/VertexFormat.h"
#include "util/Color.h"

namespace eng {

	struct FontVert {
		glm::vec3 pos;
		glm::vec2 texCoord;

		constexpr FontVert() noexcept = default;
		constexpr FontVert(const glm::vec3& pos, const glm::vec2& texCoord) noexcept :
				pos(pos), texCoord(texCoord) {}
		constexpr FontVert(const glm::vec2& pos, const glm::vec2& texCoord) noexcept :
				pos(pos, 0.0f), texCoord(texCoord) {}

		constexpr bool operator ==(const FontVert& b) const noexcept {
			return (pos == b.pos) && (texCoord == b.texCoord);
		}
		constexpr bool operator !=(const FontVert& b) const noexcept {
			return (pos != b.pos) || (texCoord != b.texCoord);
		}
	};
	struct FontQuad {
		static const VertexFormat format;

		glm::vec4 posX, posY, posZ;
		glm::vec4 texU, texV;
		Color color, outlineColor;
		float outlineSize;
		float outlineSpread;

		constexpr FontQuad() noexcept = default;
		// top left, bottom left, top right, bottom right, color, outlineColor, outlineSize, outlineSpread
		constexpr FontQuad(const FontVert& v0, const FontVert& v1, const FontVert& v2, const FontVert& v3,
						   const Color& color, const Color& outlineColor, float outlineSize, float outlineSpread) noexcept :
				posX(v0.pos.x, v1.pos.x, v2.pos.x, v3.pos.x),
				posY(v0.pos.y, v1.pos.y, v2.pos.y, v3.pos.y),
				posZ(v0.pos.z, v1.pos.z, v2.pos.z, v3.pos.z),
				texU(v0.texCoord.x, v1.texCoord.x, v2.texCoord.x, v3.texCoord.x),
				texV(v0.texCoord.y, v1.texCoord.y, v2.texCoord.y, v3.texCoord.y),
				color(color), outlineColor(outlineColor),
				outlineSize(outlineSize), outlineSpread(outlineSpread) {}

		constexpr FontVert getVertex(const size_t vertIndex) const noexcept {
			using length_t = typename glm::vec4::length_type;
			const length_t i = static_cast<length_t>(vertIndex) & length_t(3);
			return { { posX[i], posY[i], posZ[i] }, { texU[i], texV[i] } };
		}
		void setVertex(const size_t vertIndex, const FontVert& vert) noexcept {
			using length_t = typename glm::vec4::length_type;
			const length_t i = static_cast<length_t>(vertIndex) & length_t(3);
			posX[i] = vert.pos.x;
			posY[i] = vert.pos.y;
			posZ[i] = vert.pos.z;
			texU[i] = vert.texCoord.x;
			texV[i] = vert.texCoord.y;
		}

		constexpr bool operator ==(const FontQuad& b) const noexcept {
			return (posX == b.posX) &&
				   (posY == b.posY) &&
				   (posZ == b.posZ) &&
				   (texU == b.texU) &&
				   (texV == b.texV) &&
				   (color == b.color) &&
				   (outlineColor == b.outlineColor) &&
				   (outlineSize == b.outlineSize) &&
				   (outlineSpread == b.outlineSpread);
		}
		constexpr bool operator !=(const FontQuad& b) const noexcept {
			return (posX != b.posX) ||
				   (posY != b.posY) ||
				   (posZ != b.posZ) ||
				   (texU != b.texU) ||
				   (texV != b.texV) ||
				   (color != b.color) ||
				   (outlineColor != b.outlineColor) ||
				   (outlineSize != b.outlineSize) ||
				   (outlineSpread != b.outlineSpread);
		}


		// Translation

		template<typename T, glm::qualifier Q, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
		constexpr FontQuad getTranslated(const glm::vec<3, T, Q>& offset) const {
			return {
				{ { posX[0] + offset.x, posY[0] + offset.y, posZ[0] + offset.z }, { texU[0], texV[0] }, },
				{ { posX[1] + offset.x, posY[1] + offset.y, posZ[1] + offset.z }, { texU[1], texV[1] }, },
				{ { posX[2] + offset.x, posY[2] + offset.y, posZ[2] + offset.z }, { texU[2], texV[2] }, },
				{ { posX[3] + offset.x, posY[3] + offset.y, posZ[3] + offset.z }, { texU[3], texV[3] }, },
				color, outlineColor, outlineSize, outlineSpread,
			};
		}
		template<typename T, glm::qualifier Q, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
		FontQuad& translate(const glm::vec<3, T, Q>& offset) {
			posX[0] += offset.x; posY[0] += offset.y; posZ[0] += offset.z;
			posX[1] += offset.x; posY[1] += offset.y; posZ[1] += offset.z;
			posX[2] += offset.x; posY[2] += offset.y; posZ[2] += offset.z;
			posX[3] += offset.x; posY[3] += offset.y; posZ[3] += offset.z;
			return *this;
		}

		// Dilation

		constexpr FontQuad getScaled(const float scale) const noexcept {
			return {
				{ { posX[0] * scale, posY[0] * scale, posZ[0] * scale }, { texU[0], texV[0] }, },
				{ { posX[1] * scale, posY[1] * scale, posZ[1] * scale }, { texU[1], texV[1] }, },
				{ { posX[2] * scale, posY[2] * scale, posZ[2] * scale }, { texU[2], texV[2] }, },
				{ { posX[3] * scale, posY[3] * scale, posZ[3] * scale }, { texU[3], texV[3] }, },
				color, outlineColor, outlineSize, outlineSpread,
			};
		}
		constexpr FontQuad getScaled(const glm::vec3& scale) const noexcept {
			return {
				{ { posX[0] * scale.x, posY[0] * scale.y, posZ[0] * scale.z }, { texU[0], texV[0] } },
				{ { posX[1] * scale.x, posY[1] * scale.y, posZ[1] * scale.z }, { texU[1], texV[1] } },
				{ { posX[2] * scale.x, posY[2] * scale.y, posZ[2] * scale.z }, { texU[2], texV[2] } },
				{ { posX[3] * scale.x, posY[3] * scale.y, posZ[3] * scale.z }, { texU[3], texV[3] } },
				color, outlineColor, outlineSize, outlineSpread,
			};
		}
		FontQuad& scale(const float scale) noexcept {
			posX[0] *= scale; posY[0] *= scale; posZ[0] *= scale;
			posX[1] *= scale; posY[1] *= scale; posZ[1] *= scale;
			posX[2] *= scale; posY[2] *= scale; posZ[2] *= scale;
			posX[3] *= scale; posY[3] *= scale; posZ[3] *= scale;
			return *this;
		}
		FontQuad& scale(const glm::vec3& scale) noexcept {
			posX[0] *= scale.x; posY[0] *= scale.y; posZ[0] *= scale.z;
			posX[1] *= scale.x; posY[1] *= scale.y; posZ[1] *= scale.z;
			posX[2] *= scale.x; posY[2] *= scale.y; posZ[2] *= scale.z;
			posX[3] *= scale.x; posY[3] *= scale.y; posZ[3] *= scale.z;
			return *this;
		}

		// Rotation

		FontQuad getRotated(const float radians, const glm::vec3& axis) const {
			return getTransformed(glm::rotate(radians, axis));
		}
		FontQuad& rotate(const float radians, const glm::vec3& axis) {
			return this->transform(glm::rotate(radians, axis));
		}

		// Transformation

		FontQuad getTransformed(const glm::mat3& mat) const {
			return {
				{ mat * glm::vec3(posX[0], posY[0], posZ[0]), { texU[0], texV[0] } },
				{ mat * glm::vec3(posX[1], posY[1], posZ[1]), { texU[1], texV[1] } },
				{ mat * glm::vec3(posX[2], posY[2], posZ[2]), { texU[2], texV[2] } },
				{ mat * glm::vec3(posX[3], posY[3], posZ[3]), { texU[3], texV[3] } },
				color, outlineColor, outlineSize, outlineSpread,
			};
		}
		FontQuad getTransformed(const glm::mat4& mat) const {
			return {
				{ glm::vec3(mat * glm::vec4(posX[0], posY[0], posZ[0], 1.0f)), { texU[0], texV[0] } },
				{ glm::vec3(mat * glm::vec4(posX[1], posY[1], posZ[1], 1.0f)), { texU[1], texV[1] } },
				{ glm::vec3(mat * glm::vec4(posX[2], posY[2], posZ[2], 1.0f)), { texU[2], texV[2] } },
				{ glm::vec3(mat * glm::vec4(posX[3], posY[3], posZ[3], 1.0f)), { texU[3], texV[3] } },
				color, outlineColor, outlineSize, outlineSpread,
			};
		}
		FontQuad& transform(const glm::mat3& mat) {
			const glm::vec3 v0 = mat * glm::vec3(posX[0], posY[0], posZ[0]);
			const glm::vec3 v1 = mat * glm::vec3(posX[1], posY[1], posZ[1]);
			const glm::vec3 v2 = mat * glm::vec3(posX[2], posY[2], posZ[2]);
			const glm::vec3 v3 = mat * glm::vec3(posX[3], posY[3], posZ[3]);
			posX[0] = v0.x; posY[0] = v0.y; posZ[0] = v0.z;
			posX[1] = v1.x; posY[1] = v1.y; posZ[1] = v1.z;
			posX[2] = v2.x; posY[2] = v2.y; posZ[2] = v2.z;
			posX[3] = v3.x; posY[3] = v3.y; posZ[3] = v3.z;
			return *this;
		}
		FontQuad& transform(const glm::mat4& mat) {
			const glm::vec4 v0 = mat * glm::vec4(posX[0], posY[0], posZ[0], 1.0f);
			const glm::vec4 v1 = mat * glm::vec4(posX[1], posY[1], posZ[1], 1.0f);
			const glm::vec4 v2 = mat * glm::vec4(posX[2], posY[2], posZ[2], 1.0f);
			const glm::vec4 v3 = mat * glm::vec4(posX[3], posY[3], posZ[3], 1.0f);
			posX[0] = v0.x; posY[0] = v0.y; posZ[0] = v0.z;
			posX[1] = v1.x; posY[1] = v1.y; posZ[1] = v1.z;
			posX[2] = v2.x; posY[2] = v2.y; posZ[2] = v2.z;
			posX[3] = v3.x; posY[3] = v3.y; posZ[3] = v3.z;
			return *this;
		}

		// Translation

		template<typename T, glm::qualifier Q, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
		FontQuad& operator +=(const glm::vec<3, T, Q>& offset) { return this->translate(offset); }
		template<typename T, glm::qualifier Q, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
		FontQuad& operator -=(const glm::vec<3, T, Q>& offset) { return this->translate(-offset); }

		template<typename T, glm::qualifier Q, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
		FontQuad operator +(const glm::vec<3, T, Q>& offset) const { return getTranslated(offset); }
		template<typename T, glm::qualifier Q, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
		FontQuad operator -(const glm::vec<3, T, Q>& offset) const { return getTranslated(-offset); }
	};
	template<typename T, glm::qualifier Q, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	FontQuad operator +(const glm::vec<3, T, Q>& offset, const FontQuad& quad) { return quad.getTranslated(offset); }

	const inline VertexFormat FontQuad::format {
		sizeof(FontQuad), {
			{ "posX", VertexAttribType::FLOAT, 4, },
			{ "posY", VertexAttribType::FLOAT, 4, },
			{ "posZ", VertexAttribType::FLOAT, 4, },
			{ "texU", VertexAttribType::FLOAT, 4, },
			{ "texV", VertexAttribType::FLOAT, 4, },
			{ "color", VertexAttribType::UINT8, 4, VertexAttribShaderType::FLOAT_NORMALIZED },
			{ "outlineColor", VertexAttribType::UINT8, 4, VertexAttribShaderType::FLOAT_NORMALIZED },
			{ "outlineSize", VertexAttribType::FLOAT, 1 },
			{ "outlineSpread", VertexAttribType::FLOAT, 1 },
		}
	};


}