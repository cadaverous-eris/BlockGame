#pragma once

#include <algorithm>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>

#include "render/VertexFormat.h"
#include "util/math/Plane.h"
#include "util/Color.h"


namespace eng {

	struct BlockVert {
		glm::vec3 pos;
		glm::vec2 texCoord;

		BlockVert() = default;
		BlockVert(const glm::vec3& pos, const glm::vec2& texCoord) noexcept : pos(pos), texCoord(texCoord) {}

		bool operator ==(const BlockVert& b) const noexcept {
			return (pos == b.pos) && (texCoord == b.texCoord);
		}
		bool operator !=(const BlockVert& b) const noexcept {
			return (pos != b.pos) || (texCoord != b.texCoord);
		}

		// Translation

		BlockVert getTranslated(const glm::vec3& offset) const noexcept {
			return { pos + offset, texCoord };
		}
		BlockVert getTranslated(const glm::ivec3& offset) const noexcept {
			return { pos + static_cast<glm::vec3>(offset), texCoord };
		}
		BlockVert& translate(const glm::vec3& offset) noexcept {
			pos += offset;
			return *this;
		}
		BlockVert& translate(const glm::ivec3& offset) noexcept {
			pos += offset;
			return *this;
		}

		// Dilation

		BlockVert getScaled(const float scale) const noexcept {
			return { pos * scale, texCoord };
		}
		BlockVert getScaled(const glm::vec3& scale) const noexcept {
			return { { pos.x * scale.x, pos.y * scale.y, pos.z * scale.z }, texCoord };
		}
		BlockVert& scale(const float scale) noexcept {
			pos *= scale;
			return *this;
		}
		BlockVert& scale(const glm::vec3& scale) noexcept {
			pos.x *= scale.x;
			pos.y *= scale.y;
			pos.z *= scale.z;
			return *this;
		}

		// Rotation

		BlockVert getRotated(const float radians, const glm::vec3& axis) const {
			return { glm::mat3(glm::rotate(radians, axis)) * pos, texCoord };
		}
		BlockVert& rotate(const float radians, const glm::vec3& axis) {
			pos = glm::mat3(glm::rotate(radians, axis)) * pos;
			return *this;
		}

		// Transformation

		BlockVert getTransformed(const glm::mat3& mat) const {
			return { mat * pos, texCoord };
		}
		BlockVert getTransformed(const glm::mat4& mat) const {
			return { glm::vec3(mat * glm::vec4(pos, 1.0f)), texCoord };
		}
		BlockVert& transform(const glm::mat3& mat) {
			pos = mat * pos;
			return *this;
		}
		BlockVert& transform(const glm::mat4& mat) {
			pos = glm::vec3(mat * glm::vec4(pos, 1.0f));
			return *this;
		}

		// Translation

		BlockVert& operator +=(const glm::vec3& offset) noexcept { return this->translate(offset); }
		BlockVert& operator +=(const glm::ivec3& offset) noexcept { return this->translate(offset); }
		BlockVert& operator -=(const glm::vec3& offset) noexcept { return this->translate(-offset); }
		BlockVert& operator -=(const glm::ivec3& offset) noexcept { return this->translate(-offset); }

		BlockVert operator +(const glm::vec3& offset) const noexcept { return getTranslated(offset); }
		BlockVert operator +(const glm::ivec3& offset) const noexcept { return getTranslated(offset); }
		BlockVert operator -(const glm::vec3& offset) const noexcept { return getTranslated(-offset); }
		BlockVert operator -(const glm::ivec3& offset) const noexcept { return getTranslated(-offset); }
		friend BlockVert operator +(const glm::vec3& offset, const BlockVert& vert) noexcept { return vert.getTranslated(offset); }
		friend BlockVert operator +(const glm::ivec3& offset, const BlockVert& vert) noexcept { return vert.getTranslated(offset); }
	};


	struct BlockQuad {
		static const VertexFormat format;

		glm::vec4 posX, posY, posZ;
		glm::vec4 texU, texV;
		glm::u8vec3 color;

		BlockQuad() = default; // for containers of quads

		BlockQuad(const BlockVert& v0, const BlockVert& v1, const BlockVert& v2, const BlockVert& v3, const glm::u8vec3& color = { 0xFF, 0xFF, 0xFF }) noexcept : 
				posX(v0.pos.x, v1.pos.x, v2.pos.x, v3.pos.x),
				posY(v0.pos.y, v1.pos.y, v2.pos.y, v3.pos.y),
				posZ(v0.pos.z, v1.pos.z, v2.pos.z, v3.pos.z),
				texU(v0.texCoord.x, v1.texCoord.x, v2.texCoord.x, v3.texCoord.x),
				texV(v0.texCoord.y, v1.texCoord.y, v2.texCoord.y, v3.texCoord.y),
				color(color) {}
		BlockQuad(const BlockVert& v0, const BlockVert& v1, const BlockVert& v2, const BlockVert& v3, const Color& color) noexcept :
				posX(v0.pos.x, v1.pos.x, v2.pos.x, v3.pos.x),
				posY(v0.pos.y, v1.pos.y, v2.pos.y, v3.pos.y),
				posZ(v0.pos.z, v1.pos.z, v2.pos.z, v3.pos.z),
				texU(v0.texCoord.x, v1.texCoord.x, v2.texCoord.x, v3.texCoord.x),
				texV(v0.texCoord.y, v1.texCoord.y, v2.texCoord.y, v3.texCoord.y),
				color(static_cast<glm::u8vec3>(color)) {}


		BlockVert getVertex(const size_t vertIndex) const noexcept {
			const size_t i = vertIndex & 3;
			return { { posX[i], posY[i], posZ[i] }, { texU[i], texV[i] } };
		}
		void setVertex(const size_t vertIndex, const BlockVert& vert) noexcept {
			const size_t i = vertIndex & 3;
			posX[i] = vert.pos.x;
			posY[i] = vert.pos.y;
			posZ[i] = vert.pos.z;
			texU[i] = vert.texCoord.x;
			texV[i] = vert.texCoord.y;
		}

		glm::vec3 getCenter() const noexcept {
			return {
				(posX[0] + posX[1] + posX[2] + posX[3]) / 4.0f,
				(posY[0] + posY[1] + posY[2] + posY[3]) / 4.0f,
				(posZ[0] + posZ[1] + posZ[2] + posZ[3]) / 4.0f,
			};
		}

		bool isFlat() const {
			return PlaneF::fromPoints(
				{ posX[0], posY[0], posZ[0] },
				{ posX[1], posY[1], posZ[1] },
				{ posX[2], posY[2], posZ[2] }
			).intersects({ posX[3], posY[3], posZ[3] });
		}

		BlockQuad getBackQuad() const noexcept {
			return {
				{ { posX[2], posY[2], posZ[2] }, { texU[2], texV[2] } },
				{ { posX[3], posY[3], posZ[3] }, { texU[3], texV[3] } },
				{ { posX[0], posY[0], posZ[0] }, { texU[0], texV[0] } },
				{ { posX[1], posY[1], posZ[1] }, { texU[1], texV[1] } },
				color,
			};
		}

		BlockQuad operator -() const noexcept { return getBackQuad(); }

		bool operator ==(const BlockQuad& b) const noexcept {
			return (posX == b.posX) &&
				   (posY == b.posY) &&
				   (posZ == b.posZ) &&
				   (texU == b.texU) &&
				   (texV == b.texV) &&
			       (color == b.color);
		}
		bool operator !=(const BlockQuad& b) const noexcept {
			return (posX != b.posX) ||
				   (posY != b.posY) ||
				   (posZ != b.posZ) ||
				   (texU != b.texU) ||
				   (texV != b.texV) ||
				   (color != b.color);
		}

		// Translation

		BlockQuad getTranslated(const glm::vec3& offset) const noexcept {
			return {
				{ { posX[0] + offset.x, posY[0] + offset.y, posZ[0] + offset.z }, { texU[0], texV[0] } },
				{ { posX[1] + offset.x, posY[1] + offset.y, posZ[1] + offset.z }, { texU[1], texV[1] } },
				{ { posX[2] + offset.x, posY[2] + offset.y, posZ[2] + offset.z }, { texU[2], texV[2] } },
				{ { posX[3] + offset.x, posY[3] + offset.y, posZ[3] + offset.z }, { texU[3], texV[3] } },
				color,
			};
		}
		BlockQuad getTranslated(const glm::ivec3& offset) const noexcept {
			return {
				{ { posX[0] + offset.x, posY[0] + offset.y, posZ[0] + offset.z }, { texU[0], texV[0] } },
				{ { posX[1] + offset.x, posY[1] + offset.y, posZ[1] + offset.z }, { texU[1], texV[1] } },
				{ { posX[2] + offset.x, posY[2] + offset.y, posZ[2] + offset.z }, { texU[2], texV[2] } },
				{ { posX[3] + offset.x, posY[3] + offset.y, posZ[3] + offset.z }, { texU[3], texV[3] } },
				color,
			};
		}
		BlockQuad& translate(const glm::vec3& offset) noexcept {
			posX[0] += offset.x; posY[0] += offset.y; posZ[0] += offset.z;
			posX[1] += offset.x; posY[1] += offset.y; posZ[1] += offset.z;
			posX[2] += offset.x; posY[2] += offset.y; posZ[2] += offset.z;
			posX[3] += offset.x; posY[3] += offset.y; posZ[3] += offset.z;
			return *this;
		}
		BlockQuad& translate(const glm::ivec3& offset) noexcept {
			posX[0] += offset.x; posY[0] += offset.y; posZ[0] += offset.z;
			posX[1] += offset.x; posY[1] += offset.y; posZ[1] += offset.z;
			posX[2] += offset.x; posY[2] += offset.y; posZ[2] += offset.z;
			posX[3] += offset.x; posY[3] += offset.y; posZ[3] += offset.z;
			return *this;
		}

		// Dilation
		
		BlockQuad getScaled(const float scale) const noexcept {
			return {
				{ { posX[0] * scale, posY[0] * scale, posZ[0] * scale }, { texU[0], texV[0] } },
				{ { posX[1] * scale, posY[1] * scale, posZ[1] * scale }, { texU[1], texV[1] } },
				{ { posX[2] * scale, posY[2] * scale, posZ[2] * scale }, { texU[2], texV[2] } },
				{ { posX[3] * scale, posY[3] * scale, posZ[3] * scale }, { texU[3], texV[3] } },
				color,
			};
		}
		BlockQuad getScaled(const glm::vec3& scale) const noexcept {
			return {
				{ { posX[0] * scale.x, posY[0] * scale.y, posZ[0] * scale.z }, { texU[0], texV[0] } },
				{ { posX[1] * scale.x, posY[1] * scale.y, posZ[1] * scale.z }, { texU[1], texV[1] } },
				{ { posX[2] * scale.x, posY[2] * scale.y, posZ[2] * scale.z }, { texU[2], texV[2] } },
				{ { posX[3] * scale.x, posY[3] * scale.y, posZ[3] * scale.z }, { texU[3], texV[3] } },
				color,
			};
		}
		BlockQuad& scale(const float scale) noexcept {
			posX[0] *= scale; posY[0] *= scale; posZ[0] *= scale;
			posX[1] *= scale; posY[1] *= scale; posZ[1] *= scale;
			posX[2] *= scale; posY[2] *= scale; posZ[2] *= scale;
			posX[3] *= scale; posY[3] *= scale; posZ[3] *= scale;
			return *this;
		}
		BlockQuad& scale(const glm::vec3& scale) noexcept {
			posX[0] *= scale.x; posY[0] *= scale.y; posZ[0] *= scale.z;
			posX[1] *= scale.x; posY[1] *= scale.y; posZ[1] *= scale.z;
			posX[2] *= scale.x; posY[2] *= scale.y; posZ[2] *= scale.z;
			posX[3] *= scale.x; posY[3] *= scale.y; posZ[3] *= scale.z;
			return *this;
		}

		// Rotation

		inline BlockQuad getRotated(const float radians, const glm::vec3& axis) const {
			return getTransformed(glm::rotate(radians, axis));
		}
		inline BlockQuad& rotate(const float radians, const glm::vec3& axis) {
			return this->transform(glm::rotate(radians, axis));
		}

		// Transformation

		BlockQuad getTransformed(const glm::mat3& mat) const {
			return {
				{ mat * glm::vec3(posX[0], posY[0], posZ[0]), { texU[0], texV[0] } },
				{ mat * glm::vec3(posX[1], posY[1], posZ[1]), { texU[1], texV[1] } },
				{ mat * glm::vec3(posX[2], posY[2], posZ[2]), { texU[2], texV[2] } },
				{ mat * glm::vec3(posX[3], posY[3], posZ[3]), { texU[3], texV[3] } },
				color,
			};
		}
		BlockQuad getTransformed(const glm::mat4& mat) const {
			return {
				{ glm::vec3(mat * glm::vec4(posX[0], posY[0], posZ[0], 1.0f)), { texU[0], texV[0] } },
				{ glm::vec3(mat * glm::vec4(posX[1], posY[1], posZ[1], 1.0f)), { texU[1], texV[1] } },
				{ glm::vec3(mat * glm::vec4(posX[2], posY[2], posZ[2], 1.0f)), { texU[2], texV[2] } },
				{ glm::vec3(mat * glm::vec4(posX[3], posY[3], posZ[3], 1.0f)), { texU[3], texV[3] } },
				color,
			};
		}
		BlockQuad& transform(const glm::mat3& mat) {
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
		BlockQuad& transform(const glm::mat4& mat) {
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

		// Coloring

		// create a copy of the quad with it's color multiplied by another Color
		BlockQuad getColorized(const Color& c) const noexcept {
			return {
				{ { posX[0], posY[0], posZ[0] }, { texU[0], texV[0] } },
				{ { posX[1], posY[1], posZ[1] }, { texU[1], texV[1] } },
				{ { posX[2], posY[2], posZ[2] }, { texU[2], texV[2] } },
				{ { posX[3], posY[3], posZ[3] }, { texU[3], texV[3] } },
				Color(color) * c,
			};
		}
		BlockQuad withColor(const Color& color) const noexcept {
			return {
				{ { posX[0], posY[0], posZ[0] }, { texU[0], texV[0] } },
				{ { posX[1], posY[1], posZ[1] }, { texU[1], texV[1] } },
				{ { posX[2], posY[2], posZ[2] }, { texU[2], texV[2] } },
				{ { posX[3], posY[3], posZ[3] }, { texU[3], texV[3] } },
				color,
			};
		}
		// multiply the quad's current color by another Color
		BlockQuad& colorize(const Color& color) noexcept {
			this->color = static_cast<glm::u8vec3>(Color(this->color) * color);
			return *this;
		}
		BlockQuad& setColor(const Color& color) noexcept {
			this->color = static_cast<glm::u8vec3>(color);
			return *this;
		}

		// Translation Operators

		inline BlockQuad& operator +=(const glm::vec3& offset) noexcept { return this->translate(offset); }
		inline BlockQuad& operator +=(const glm::ivec3& offset) noexcept { return this->translate(offset); }
		inline BlockQuad& operator -=(const glm::vec3& offset) noexcept { return this->translate(-offset); }
		inline BlockQuad& operator -=(const glm::ivec3& offset) noexcept { return this->translate(-offset); }

		inline BlockQuad operator +(const glm::vec3& offset) const noexcept { return getTranslated(offset); }
		inline BlockQuad operator +(const glm::ivec3& offset) const noexcept { return getTranslated(offset); }
		inline BlockQuad operator -(const glm::vec3& offset) const noexcept { return getTranslated(-offset); }
		inline BlockQuad operator -(const glm::ivec3& offset) const noexcept { return getTranslated(-offset); }
		inline friend BlockQuad operator +(const glm::vec3& offset, const BlockQuad& quad) noexcept { return quad.getTranslated(offset); }
		inline friend BlockQuad operator +(const glm::ivec3& offset, const BlockQuad& quad) noexcept { return quad.getTranslated(offset); }
	
		// Dilation Operators

		inline BlockQuad& operator *=(const float scale) noexcept { return this->scale(scale); }
		inline BlockQuad& operator *=(const glm::vec3& scale) noexcept { return this->scale(scale); }
		inline BlockQuad& operator /=(const float scale) noexcept { return this->scale(1.0f / scale); }
		inline BlockQuad& operator /=(const glm::vec3& scale) noexcept { return this->scale(1.0f / scale); }

		inline BlockQuad operator *(const float scale) const noexcept { return getScaled(scale); }
		inline BlockQuad operator *(const glm::vec3& scale) const noexcept { return getScaled(scale); }
		inline BlockQuad operator /(const float scale) const noexcept { return getScaled( 1.0f / scale); }
		inline BlockQuad operator /(const glm::vec3& scale) const noexcept { return getScaled(1.0f / scale); }
		inline friend BlockQuad operator *(const float scale, const BlockQuad& quad) noexcept { return quad.getScaled(scale); }
		inline friend BlockQuad operator *(const glm::vec3& scale, const BlockQuad& quad) noexcept { return quad.getScaled(scale); }

		// Coloring Operators

		inline BlockQuad& operator *=(const Color& c) noexcept { return this->colorize(c); }
		inline BlockQuad operator *(const Color& c) const noexcept { return getColorized(c); }
		inline friend BlockQuad operator *(const Color& c, const BlockQuad& quad) noexcept { return quad.getColorized(c); }
		
	};

	const inline VertexFormat BlockQuad::format {
		sizeof(BlockQuad), {
			{ "posX", VertexAttribType::FLOAT, 4, },
			{ "posY", VertexAttribType::FLOAT, 4, },
			{ "posZ", VertexAttribType::FLOAT, 4, },
			{ "texU", VertexAttribType::FLOAT, 4, },
			{ "texV", VertexAttribType::FLOAT, 4, },
			{ "color", VertexAttribType::UINT8, 3, VertexAttribShaderType::FLOAT_NORMALIZED },
		}
	};

}