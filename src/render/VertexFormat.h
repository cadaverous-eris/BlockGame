#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <initializer_list>

#include "Renderer.h"
#include "util/type_traits.h"

namespace eng {

	enum class VertexAttribType : GLenum {
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE,
		INT8 = GL_BYTE,
		UINT8 = GL_UNSIGNED_BYTE,
		INT16 = GL_SHORT,
		UINT16 = GL_UNSIGNED_SHORT,
		INT32 = GL_INT,
		UINT32 = GL_UNSIGNED_INT,
		INT_2_10_10_10_REV = GL_INT_2_10_10_10_REV,
		UINT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
	};
	enum class VertexAttribShaderType { FLOAT, INTEGER, FLOAT_NORMALIZED };

	constexpr GLenum getVertexAttribGLType(const VertexAttribType type) noexcept { return to_underlying(type); }
	constexpr size_t getVertexAttribTypeSize(const VertexAttribType type) noexcept {
		switch (type) {
			case VertexAttribType::FLOAT: return 4;
			case VertexAttribType::DOUBLE: return 8;
			case VertexAttribType::INT8: return 1;
			case VertexAttribType::UINT8: return 1;
			case VertexAttribType::INT16: return 2;
			case VertexAttribType::UINT16: return 2;
			case VertexAttribType::INT32: return 4;
			case VertexAttribType::UINT32: return 4;
			case VertexAttribType::INT_2_10_10_10_REV: return 4;
			case VertexAttribType::UINT_2_10_10_10_REV: return 4;
			default: return 0;
		}
	}
	// TODO: use to validate vertex attributes?
	constexpr bool isIntVertexAttribType(VertexAttribType type) noexcept {
		switch (type) {
			case VertexAttribType::INT8:
			case VertexAttribType::UINT8:
			case VertexAttribType::INT16:
			case VertexAttribType::UINT16:
			case VertexAttribType::INT32:
			case VertexAttribType::UINT32:
				return true;
			default:
				return false;
		}
	}
	// TODO: use to validate vertex attributes?
	constexpr bool isNormalizableVertexAttribType(VertexAttribType type) noexcept {
		switch (type) {
			case VertexAttribType::INT8:
			case VertexAttribType::UINT8:
			case VertexAttribType::INT16:
			case VertexAttribType::UINT16:
			case VertexAttribType::INT32:
			case VertexAttribType::UINT32:
			case VertexAttribType::INT_2_10_10_10_REV:
			case VertexAttribType::UINT_2_10_10_10_REV:
				return true;
			default:
				return false;
		}
	}


	class VertexAttribute {
		friend class VertexFormat;
	public:
		using Type = VertexAttribType;
		using ShaderType = VertexAttribShaderType;

	private:
		std::string name;
		Type type;
		int count;
		ShaderType shaderType;
		size_t offset = 0;

	public:
		VertexAttribute(const std::string& name, Type type, int count, ShaderType shaderType = ShaderType::FLOAT) :
				name(name), type(type), count(count), shaderType(shaderType) {}

		const std::string& getName() const noexcept { return name; }
		int getCount() const noexcept { return count; };
		size_t getElementSize() const noexcept { return getVertexAttribTypeSize(type); };
		size_t getSizeBytes() const noexcept { return getVertexAttribTypeSize(type) * count; };
		bool isNormalized() const noexcept { return shaderType == ShaderType::FLOAT_NORMALIZED; }
		ShaderType getShaderType() const noexcept { return shaderType; }
		size_t getOffset() const noexcept { return offset; }
		GLenum getGLType() const noexcept { return getVertexAttribGLType(type); }
	};

	class VertexFormat {
	private:
		using attribList_t = std::vector<VertexAttribute>;

		attribList_t attributes;
		size_t stride;

	public:
		using const_iterator = attribList_t::const_iterator;

		explicit VertexFormat(std::initializer_list<VertexAttribute> attributes);
		VertexFormat(const size_t stride, std::initializer_list<VertexAttribute> attributes);

		size_t getStride() const noexcept { return stride; }

		const_iterator begin() const noexcept { return attributes.begin(); }
		const_iterator end() const noexcept { return attributes.end(); }
		const_iterator cbegin() const noexcept { return attributes.cbegin(); }
		const_iterator cend() const noexcept { return attributes.cend(); }
	};

}