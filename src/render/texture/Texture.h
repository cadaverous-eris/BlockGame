#pragma once

#include <string>
#include <array>
#include <memory>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <iostream>

#include <glad/glad.h>

#include "util/type_traits.h"
#include "util/math/math.h"
#include "util/resources/Image.h" // TODO: remove, and forward declare instead?

namespace eng {

	/*enum class TextureType : GLenum {
		TEXTURE_1D            = GL_TEXTURE_1D,
		TEXTURE_2D            = GL_TEXTURE_2D,
		TEXTURE_3D            = GL_TEXTURE_3D,
		TEXTURE_CUBEMAP       = GL_TEXTURE_CUBE_MAP,
		TEXTURE_1D_ARRAY      = GL_TEXTURE_1D_ARRAY,
		TEXTURE_2D_ARRAY      = GL_TEXTURE_2D_ARRAY,
		TEXTURE_CUBEMAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
		TEXTURE_BUFFER        = GL_TEXTURE_BUFFER,
	};*/

	enum class TextureInternalFormat : GLint {
		U2x4   = GL_RGBA2,        // 4 normalized 2-bit uints
		U4x3   = GL_RGB4,         // 3 normalized 4-bit uints
		U4x4   = GL_RGBA4,        // 4 normalized 4-bit uints
		U5x3   = GL_RGB5,         // 3 normalized 5-bit uints
		U8x1   = GL_R8,           // 1 normalized 8-bit uint
		U8x2   = GL_RG8,          // 2 normalized 8-bit uints
		U8x3   = GL_RGB8,         // 3 normalized 8-bit uints
		U8x4   = GL_RGBA8,        // 4 normalized 8-bit uints
		U10x3  = GL_RGB10,        // 3 normalized 10-bit uints
		U12x3  = GL_RGB12,        // 3 normalized 12-bit uints
		U12x4  = GL_RGBA12,       // 4 normalized 12-bit uints
		U16x1  = GL_R16,          // 1 normalized 16-bit uint
		U16x2  = GL_RG16,         // 2 normalized 16-bit uints
		U16x3  = GL_RGB16,        // 3 normalized 16-bit uints
		U16x4  = GL_RGBA16,       // 4 normalized 16-bit uints
		S8x1   = GL_R8_SNORM,     // 1 normalized 8-bit int
		S8x2   = GL_RG8_SNORM,    // 2 normalized 8-bit ints
		S8x3   = GL_RGB8_SNORM,   // 3 normalized 8-bit ints
		S8x4   = GL_RGBA8_SNORM,  // 4 normalized 8-bit ints
		S16x1  = GL_R16_SNORM,    // 1 normalized 16-bit int
		S16x2  = GL_RG16_SNORM,   // 2 normalized 16-bit ints
		S16x3  = GL_RGB16_SNORM,  // 3 normalized 16-bit ints
		S16x4  = GL_RGBA16_SNORM, // 4 normalized 16-bit ints
		UINT8x1    = GL_R8UI,     // 1 8-bit uint
		UINT8x2    = GL_RG8UI,    // 2 8-bit uints
		UINT8x3    = GL_RGB8UI,   // 3 8-bit uints
		UINT8x4    = GL_RGBA8UI,  // 4 8-bit uints
		UINT16x1   = GL_R16UI,    // 1 16-bit uint
		UINT16x2   = GL_RG16UI,   // 2 16-bit uints
		UINT16x3   = GL_RGB16UI,  // 3 16-bit uints
		UINT16x4   = GL_RGBA16UI, // 4 16-bit uints
		UINT32x1   = GL_R32UI,    // 1 32-bit uint
		UINT32x2   = GL_RG32UI,   // 1 32-bit uints
		UINT32x3   = GL_RGB32UI,  // 3 32-bit uints
		UINT32x4   = GL_RGBA32UI, // 4 32-bit uints
		SINT8x1    = GL_R8I,      // 1 8-bit int
		SINT8x2    = GL_RG8I,     // 2 8-bit ints
		SINT8x3    = GL_RGB8I,    // 3 8-bit ints
		SINT8x4    = GL_RGBA8I,   // 4 8-bit ints
		SINT16x1   = GL_R16I,     // 1 16-bit int
		SINT16x2   = GL_RG16I,	  // 2 16-bit ints
		SINT16x3   = GL_RGB16I,	  // 3 16-bit ints
		SINT16x4   = GL_RGBA16I,  // 4 16-bit ints
		SINT32x1   = GL_R32I,     // 1 32-bit int
		SINT32x2   = GL_RG32I,	  // 2 32-bit ints
		SINT32x3   = GL_RGB32I,	  // 3 32-bit ints
		SINT32x4   = GL_RGBA32I,  // 4 32-bit ints
		FLOAT16x1  = GL_R16F,     // 1 16-bit float
		FLOAT16x2  = GL_RG16F,    // 2 16-bit floats
		FLOAT16x3  = GL_RGB16F,   // 3 16-bit floats
		FLOAT16x4  = GL_RGBA16F,  // 4 16-bit floats
		FLOAT32x1  = GL_R32F,     // 1 32-bit float
		FLOAT32x2  = GL_RG32F,    // 2 32-bit floats
		FLOAT32x3  = GL_RGB32F,   // 3 32-bit floats
		FLOAT32x4  = GL_RGBA32F,  // 4 32-bit floats
		// Special Sized Formats
		U3_3_2         = GL_R3_G3_B2,       // 2 normalized 3-bit uints + 1 normalized 2-bit uint
		U5_5_5_1       = GL_RGB5_A1,        // 3 normalized 5-bit uints + 1 normalized 1-bit uint
		U10_10_10_2    = GL_RGB10_A2,       // 3 normalized 10-bit uints + 1 normalized 2-bit uint
		//U5_6_5         = GL_RGB565,         // 1 normalized 5-bit uint + 1 normalized 6-bit uint + 1 normalized 5-bit uint
		UINT10_10_10_2 = GL_RGB10_A2UI,     // 3 10-bit uints + 1 2-bit uint
		FLOAT11_11_10  = GL_R11F_G11F_B10F, // 2 11-bit floats + 1 10-bit float
		FLOAT9x3_E5    = GL_RGB9_E5,        // 3 9-bit floats that all share a 5-bit exponent
		// Depth Component Formats
		DEPTH_COMPONENT_U16     = GL_DEPTH_COMPONENT16,  // normalized 16-bit uint depth value
		DEPTH_COMPONENT_U24     = GL_DEPTH_COMPONENT24,  // normalized 24-bit uint depth value
		DEPTH_COMPONENT_U32     = GL_DEPTH_COMPONENT32,  // normalized 32-bit uint depth value
		DEPTH_COMPONENT_FLOAT32 = GL_DEPTH_COMPONENT32F, // 32-bit float depth value
		// Depth + Stencil Formats
		DEPTH_U24_STENCIL_U8     = GL_DEPTH24_STENCIL8,  // normalized 24-bit uint depth value + 8-bit uint stencil value
		DEPTH_FLOAT32_STENCIL_U8 = GL_DEPTH32F_STENCIL8, // 32-bit float depth value + 8-bit uint stencil value
		// Normalized Unsigned Char Format Aliases
		BYTEx1 = U8x1, // 1 normalized unsigned char
		BYTEx2 = U8x2, // 2 normalized unsigned chars
		BYTEx3 = U8x3, // 3 normalized unsigned chars
		BYTEx4 = U8x4, // 4 normalized unsigned chars
	};
	enum class TextureFormat : GLenum {
		RED = GL_RED,   // single-channel color value
		RG = GL_RG,     // 2-channel color value
		RGB = GL_RGB,   // 3-channel color value
		BGR = GL_BGR,   // 3-channel color value with RGB components reversed
		RGBA = GL_RGBA, // 4-channel color value
		BGRA = GL_BGRA, // 4-channel color value with RGB components reversed
		STENCIL_INDEX = GL_STENCIL_INDEX,     // stencil value
		DEPTH_COMPONENT = GL_DEPTH_COMPONENT, // depth value
		DEPTH_STENCIL = GL_DEPTH_STENCIL,     // depth value + stencil value
		GREEN = GL_GREEN,
		BLUE = GL_BLUE,
		RED_INT = GL_RED_INTEGER,
		GREEN_INT = GL_GREEN_INTEGER,
		BLUE_INT = GL_BLUE_INTEGER,
		RG_INT = GL_RG_INTEGER,
		RGB_INT = GL_RGB_INTEGER,
		BGR_INT = GL_BGR_INTEGER,
		RGBA_INT = GL_RGBA_INTEGER,
		BGRA_INT = GL_BGRA_INTEGER,
	};
	enum class TextureDataType : GLenum {
		UINT8 = GL_UNSIGNED_BYTE,
		SINT8 = GL_BYTE,
		UINT16 = GL_UNSIGNED_SHORT,
		SINT16 = GL_SHORT,
		UINT32 = GL_UNSIGNED_INT,
		SINT32 = GL_INT,
		FLOAT32 = GL_FLOAT,
		UINT3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
		UINT2_3_3_REV = GL_UNSIGNED_BYTE_2_3_3_REV,
		UINT5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
		UINT5_6_5_REV = GL_UNSIGNED_SHORT_5_6_5_REV,
		UINT4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
		UINT4_4_4_4_REV = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		UINT5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
		UINT1_5_5_5_REV = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		UINT8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
		UINT8_8_8_8_REV = GL_UNSIGNED_INT_8_8_8_8_REV,
		UINT10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
		UINT2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
		UINT24_8 = GL_UNSIGNED_INT_24_8,
		FLOAT32_UINT24_8_REV = GL_FLOAT_32_UNSIGNED_INT_24_8_REV, // 1 32-bit float + 24 bits of padding + 1 8-bit uint
		UINT10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV, // 1 10-bit float + 2 11-bit floats packed into a 32-bit uint
		UINT5_9_9_9_REV = GL_UNSIGNED_INT_5_9_9_9_REV,
	};

	// struct used for pixel data of the FLOAT32_UINT24_8_REV (GL_FLOAT_32_UNSIGNED_INT_24_8_REV) type
	struct Float32_Uint24_8 {
		float f = 0;
		uint32_t u24 : 24;
		uint32_t u8 : 8;
	};


	template<size_t S>
	using size_t_constant = std::integral_constant<size_t, S>;

	[[nodiscard]] constexpr size_t getTextureInternalFormatChannels(const TextureInternalFormat internalFormat) noexcept {
		switch (internalFormat) {
		case TextureInternalFormat::U2x4: return 4;
		case TextureInternalFormat::U4x3: return 3;
		case TextureInternalFormat::U4x4: return 4;
		case TextureInternalFormat::U5x3: return 3;
		case TextureInternalFormat::U8x1: return 1;
		case TextureInternalFormat::U8x2: return 2;
		case TextureInternalFormat::U8x3: return 3;
		case TextureInternalFormat::U8x4: return 4;
		case TextureInternalFormat::U10x3: return 3;
		case TextureInternalFormat::U12x3: return 3;
		case TextureInternalFormat::U12x4: return 4;
		case TextureInternalFormat::U16x1: return 1;
		case TextureInternalFormat::U16x2: return 2;
		case TextureInternalFormat::U16x3: return 3;
		case TextureInternalFormat::U16x4: return 4;
		case TextureInternalFormat::S8x1: return 1;
		case TextureInternalFormat::S8x2: return 2;
		case TextureInternalFormat::S8x3: return 3;
		case TextureInternalFormat::S8x4: return 4;
		case TextureInternalFormat::S16x1: return 1;
		case TextureInternalFormat::S16x2: return 2;
		case TextureInternalFormat::S16x3: return 3;
		case TextureInternalFormat::S16x4: return 4;
		case TextureInternalFormat::UINT8x1: return 1;
		case TextureInternalFormat::UINT8x2: return 2;
		case TextureInternalFormat::UINT8x3: return 3;
		case TextureInternalFormat::UINT8x4: return 4;
		case TextureInternalFormat::UINT16x1: return 1;
		case TextureInternalFormat::UINT16x2: return 2;
		case TextureInternalFormat::UINT16x3: return 3;
		case TextureInternalFormat::UINT16x4: return 4;
		case TextureInternalFormat::UINT32x1: return 1;
		case TextureInternalFormat::UINT32x2: return 2;
		case TextureInternalFormat::UINT32x3: return 3;
		case TextureInternalFormat::UINT32x4: return 4;
		case TextureInternalFormat::SINT8x1: return 1;
		case TextureInternalFormat::SINT8x2: return 2;
		case TextureInternalFormat::SINT8x3: return 3;
		case TextureInternalFormat::SINT8x4: return 4;
		case TextureInternalFormat::SINT16x1: return 1;
		case TextureInternalFormat::SINT16x2: return 2;
		case TextureInternalFormat::SINT16x3: return 3;
		case TextureInternalFormat::SINT16x4: return 4;
		case TextureInternalFormat::SINT32x1: return 1;
		case TextureInternalFormat::SINT32x2: return 2;
		case TextureInternalFormat::SINT32x3: return 3;
		case TextureInternalFormat::SINT32x4: return 4;
		case TextureInternalFormat::FLOAT16x1: return 1;
		case TextureInternalFormat::FLOAT16x2: return 2;
		case TextureInternalFormat::FLOAT16x3: return 3;
		case TextureInternalFormat::FLOAT16x4: return 4;
		case TextureInternalFormat::FLOAT32x1: return 1;
		case TextureInternalFormat::FLOAT32x2: return 2;
		case TextureInternalFormat::FLOAT32x3: return 3;
		case TextureInternalFormat::FLOAT32x4: return 4;
		case TextureInternalFormat::U3_3_2: return 3;
		case TextureInternalFormat::U5_5_5_1: return 4;
		case TextureInternalFormat::U10_10_10_2: return 4;
		//case TextureInternalFormat::U5_6_5: return 3;
		case TextureInternalFormat::UINT10_10_10_2: return 4;
		case TextureInternalFormat::FLOAT11_11_10: return 3;
		case TextureInternalFormat::FLOAT9x3_E5: return 3;
		case TextureInternalFormat::DEPTH_COMPONENT_U16: return 1;
		case TextureInternalFormat::DEPTH_COMPONENT_U24: return 1;
		case TextureInternalFormat::DEPTH_COMPONENT_U32: return 1;
		case TextureInternalFormat::DEPTH_COMPONENT_FLOAT32: return 1;
		case TextureInternalFormat::DEPTH_U24_STENCIL_U8: return 2;
		case TextureInternalFormat::DEPTH_FLOAT32_STENCIL_U8: return 2;
		default: return 0;
		}
	}
	template<TextureInternalFormat F, typename = void> struct tex_internal_format_channels {};
	template<TextureInternalFormat F> struct tex_internal_format_channels<F, std::enable_if_t<
		(getTextureInternalFormatChannels(F) > 0)
	>> : size_t_constant<getTextureInternalFormatChannels(F)> {};
	template<TextureInternalFormat F> constexpr size_t tex_internal_format_channels_v = tex_internal_format_channels<F>::value;

	[[nodiscard]] constexpr size_t getTextureFormatChannels(const TextureFormat format) noexcept {
		switch (format) {
		case TextureFormat::RED: return 1;
		case TextureFormat::RG: return 2;
		case TextureFormat::RGB: return 3;
		case TextureFormat::BGR: return 3;
		case TextureFormat::RGBA: return 4;
		case TextureFormat::BGRA: return 4;
		case TextureFormat::STENCIL_INDEX: return 1;
		case TextureFormat::DEPTH_COMPONENT: return 1;
		case TextureFormat::DEPTH_STENCIL: return 2;
		case TextureFormat::GREEN: return 1;
		case TextureFormat::BLUE: return 1;
		case TextureFormat::RED_INT: return 1;
		case TextureFormat::GREEN_INT: return 1;
		case TextureFormat::BLUE_INT: return 1;
		case TextureFormat::RG_INT: return 2;
		case TextureFormat::RGB_INT: return 3;
		case TextureFormat::BGR_INT: return 3;
		case TextureFormat::RGBA_INT: return 4;
		case TextureFormat::BGRA_INT: return 4;
		default: return 0;
		}
	}
	template<TextureFormat F, typename = void> struct tex_format_channels {};
	template<TextureFormat F> struct tex_format_channels<F, std::enable_if_t<
		(getTextureFormatChannels(F) > 0)
	>> : size_t_constant<getTextureFormatChannels(F)> {};
	template<TextureFormat F> constexpr size_t tex_format_channels_v = tex_format_channels<F>::value;

	template<TextureDataType T>
	using tex_data_type_constant = std::integral_constant<TextureDataType, T>;
	template<typename T> struct tex_data_type_enum {};
	template<> struct tex_data_type_enum<uint8_t> : tex_data_type_constant<TextureDataType::UINT8> {};
	template<> struct tex_data_type_enum<int8_t> : tex_data_type_constant<TextureDataType::SINT8> {};
	template<> struct tex_data_type_enum<uint16_t> : tex_data_type_constant<TextureDataType::UINT16> {};
	template<> struct tex_data_type_enum<int16_t> : tex_data_type_constant<TextureDataType::SINT16> {};
	template<> struct tex_data_type_enum<uint32_t> : tex_data_type_constant<TextureDataType::UINT32> {};
	template<> struct tex_data_type_enum<int32_t> : tex_data_type_constant<TextureDataType::SINT32> {};
	template<> struct tex_data_type_enum<float> : tex_data_type_constant<TextureDataType::FLOAT32> {};
	template<typename T> constexpr TextureDataType tex_data_type_enum_v = tex_data_type_enum<T>::value;

	template<TextureDataType T> struct tex_data_type_type {};
	template<> struct tex_data_type_type<TextureDataType::UINT8> { using type = uint8_t; };
	template<> struct tex_data_type_type<TextureDataType::SINT8> { using type = int8_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT16> { using type = uint16_t; };
	template<> struct tex_data_type_type<TextureDataType::SINT16> { using type = int16_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT32> { using type = uint32_t; };
	template<> struct tex_data_type_type<TextureDataType::SINT32> { using type = int32_t; };
	template<> struct tex_data_type_type<TextureDataType::FLOAT32> { using type = float; };
	template<> struct tex_data_type_type<TextureDataType::UINT3_3_2> { using type = uint8_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT2_3_3_REV> { using type = uint8_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT5_6_5> { using type = uint16_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT5_6_5_REV> { using type = uint16_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT4_4_4_4> { using type = uint16_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT4_4_4_4_REV> { using type = uint16_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT5_5_5_1> { using type = uint16_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT1_5_5_5_REV> { using type = uint16_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT8_8_8_8> { using type = uint32_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT8_8_8_8_REV> { using type = uint32_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT10_10_10_2> { using type = uint32_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT2_10_10_10_REV> { using type = uint32_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT24_8> { using type = uint32_t; };
	template<> struct tex_data_type_type<TextureDataType::FLOAT32_UINT24_8_REV> { using type = Float32_Uint24_8; };
	template<> struct tex_data_type_type<TextureDataType::UINT10F_11F_11F_REV> { using type = uint32_t; };
	template<> struct tex_data_type_type<TextureDataType::UINT5_9_9_9_REV> { using type = uint32_t; };
	template<TextureDataType T>
	using tex_data_type_type_t = typename tex_data_type_type<T>::type;

	[[nodiscard]] constexpr size_t getTextureDataTypeSize(const TextureDataType dataType) noexcept {
		switch (dataType) {
		case TextureDataType::UINT8: return 1;
		case TextureDataType::SINT8: return 1;
		case TextureDataType::UINT16: return 2;
		case TextureDataType::SINT16: return 2;
		case TextureDataType::UINT32: return 4;
		case TextureDataType::SINT32: return 4;
		case TextureDataType::FLOAT32: return 4;
		case TextureDataType::UINT3_3_2: return 1;
		case TextureDataType::UINT2_3_3_REV: return 1;
		case TextureDataType::UINT5_6_5: return 2;
		case TextureDataType::UINT5_6_5_REV: return 2;
		case TextureDataType::UINT4_4_4_4: return 2;
		case TextureDataType::UINT4_4_4_4_REV: return 2;
		case TextureDataType::UINT5_5_5_1: return 2;
		case TextureDataType::UINT1_5_5_5_REV: return 2;
		case TextureDataType::UINT8_8_8_8: return 4;
		case TextureDataType::UINT8_8_8_8_REV: return 4;
		case TextureDataType::UINT10_10_10_2: return 4;
		case TextureDataType::UINT2_10_10_10_REV: return 4;
		case TextureDataType::UINT24_8: return 4;
		case TextureDataType::FLOAT32_UINT24_8_REV: return 8;
		case TextureDataType::UINT10F_11F_11F_REV: return 4;
		case TextureDataType::UINT5_9_9_9_REV: return 4;
		default: return 0;
		}
	}
	template<TextureDataType T, typename = void> struct tex_data_type_size {};
	template<TextureDataType T> struct tex_data_type_size<T, std::enable_if_t<
		(getTextureDataTypeSize(T) > 0)
	>> : size_t_constant<getTextureDataTypeSize(T)> {};
	template<TextureDataType T> constexpr size_t tex_data_type_size_v = tex_data_type_size<T>::value;

	[[nodiscard]] constexpr size_t getTexturePixelSize(const TextureFormat format, TextureDataType type) noexcept {
		if ((
			(format == TextureFormat::RED) || (format == TextureFormat::GREEN) ||
			(format == TextureFormat::BLUE) || (format == TextureFormat::RG) ||
			(format == TextureFormat::RGB) || (format == TextureFormat::BGR) ||
			(format == TextureFormat::RGBA) || (format == TextureFormat::BGRA)
		) && (
			(type == TextureDataType::UINT8) || (type == TextureDataType::SINT8) ||
			(type == TextureDataType::UINT16) || (type == TextureDataType::SINT16) ||
			(type == TextureDataType::UINT32) || (type == TextureDataType::SINT32) ||
			(type == TextureDataType::FLOAT32)
		)) return getTextureFormatChannels(format) * getTextureDataTypeSize(type);
		if ((
			(format == TextureFormat::RED_INT) || (format == TextureFormat::GREEN_INT) ||
			(format == TextureFormat::BLUE_INT) || (format == TextureFormat::RG_INT) ||
			(format == TextureFormat::RGB_INT) || (format == TextureFormat::BGR_INT) ||
			(format == TextureFormat::RGBA_INT) || (format == TextureFormat::BGRA_INT)
		) && (
			(type == TextureDataType::UINT8) || (type == TextureDataType::SINT8) ||
			(type == TextureDataType::UINT16) || (type == TextureDataType::SINT16) ||
			(type == TextureDataType::UINT32) || (type == TextureDataType::SINT32)
		)) return getTextureFormatChannels(format) * getTextureDataTypeSize(type);
		if ((format == TextureFormat::STENCIL_INDEX) && (
			(type == TextureDataType::UINT8) || (type == TextureDataType::UINT16) ||
			(type == TextureDataType::UINT32)
		)) return getTextureDataTypeSize(type);
		if ((format == TextureFormat::DEPTH_COMPONENT) && (
			(type == TextureDataType::UINT16) || (type == TextureDataType::UINT32) ||
			(type == TextureDataType::FLOAT32)
		)) return getTextureDataTypeSize(type);
		if ((format == TextureFormat::DEPTH_STENCIL) && (
			(type == TextureDataType::UINT24_8) || (type == TextureDataType::FLOAT32_UINT24_8_REV)
		)) return getTextureDataTypeSize(type);
		if (((format == TextureFormat::RGB) || (format == TextureFormat::BGR)) && (
			(type == TextureDataType::UINT3_3_2) || (type == TextureDataType::UINT2_3_3_REV) ||
			(type == TextureDataType::UINT5_6_5) || (type == TextureDataType::UINT5_6_5_REV) ||
			(type == TextureDataType::UINT10F_11F_11F_REV) || (type == TextureDataType::UINT5_9_9_9_REV)
		)) return getTextureDataTypeSize(type);
		if (((format == TextureFormat::RGBA) || (format == TextureFormat::BGRA)) && (
			(type == TextureDataType::UINT4_4_4_4) || (type == TextureDataType::UINT4_4_4_4_REV) ||
			(type == TextureDataType::UINT5_5_5_1) || (type == TextureDataType::UINT1_5_5_5_REV) ||
			(type == TextureDataType::UINT8_8_8_8) || (type == TextureDataType::UINT8_8_8_8_REV) ||
			(type == TextureDataType::UINT10_10_10_2) || (type == TextureDataType::UINT2_10_10_10_REV) ||
			(type == TextureDataType::UINT5_9_9_9_REV)
		)) return getTextureDataTypeSize(type);
		if (((format == TextureFormat::RGBA_INT) || (format == TextureFormat::BGRA_INT)) && (
			(type == TextureDataType::UINT10_10_10_2) || (type == TextureDataType::UINT2_10_10_10_REV) ||
			(type == TextureDataType::UINT8_8_8_8) || (type == TextureDataType::UINT8_8_8_8_REV)
		)) return getTextureDataTypeSize(type);
		return 0;
	}

	template<TextureFormat F, TextureDataType T, typename = void> struct tex_pixel_size {};
	template<TextureFormat F, TextureDataType T> struct tex_pixel_size<F, T, std::enable_if_t<
		(getTexturePixelSize(F, T) > 0)
	>> : size_t_constant<getTexturePixelSize(F, T)> {};
	template<TextureFormat F, TextureDataType T> constexpr size_t tex_pixel_size_v = tex_pixel_size<F, T>::value;


	enum class TextureScaleMode : GLint {
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
	};

	constexpr TextureScaleMode getMagMode(const TextureScaleMode scaleMode) noexcept {
		switch (scaleMode) {
		case TextureScaleMode::NEAREST_MIPMAP_NEAREST:
		case TextureScaleMode::NEAREST_MIPMAP_LINEAR:
			return TextureScaleMode::NEAREST;
		case TextureScaleMode::LINEAR_MIPMAP_NEAREST:
		case TextureScaleMode::LINEAR_MIPMAP_LINEAR:
			return TextureScaleMode::LINEAR;
		default:
			return scaleMode;
		}
	}

	enum class TextureWrapMode : GLint {
		CLAMP_EDGE = GL_CLAMP_TO_EDGE,
		CLAMP_BORDER = GL_CLAMP_TO_BORDER,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		REPEAT = GL_REPEAT,
	};


	class FrameBuffer;

	// TODO: template by TextureType?
	class [[nodiscard]] Texture {
		friend class FrameBuffer;
	public:
		using Format = TextureFormat;
		using InternalFormat = TextureInternalFormat;
		using DataType = TextureDataType;
		using ScaleMode = TextureScaleMode;
		using WrapMode = TextureWrapMode;
	private:
		unsigned int id = 0;
		size_t width = 0, height = 0;

	public:
		[[nodiscard]] static Texture loadTexture(std::string_view path, int desiredChannels = 0) {
			std::string texPath = "res/textures/" + std::string(path);
			if (desiredChannels == 0) {
				const auto imgVar = loadImage(texPath);
				return std::visit([](const auto& img) {
					return Texture{ img }.scaleNearest();
				}, imgVar);
			} else if (desiredChannels == 1) {
				return Texture{ loadImage<1>(texPath) }.scaleNearest();
			} else if (desiredChannels == 2) {
				return Texture{ loadImage<2>(texPath) }.scaleNearest();
			} else if (desiredChannels == 3) {
				return Texture{ loadImage<3>(texPath) }.scaleNearest();
			} else if (desiredChannels == 4) {
				return Texture{ loadImage<4>(texPath) }.scaleNearest();
			} else {
				throw std::runtime_error("desiredChannels argument of Texture::loadTexture() must be in the range [0, 4)");
			}
		}

		[[nodiscard]] Texture() noexcept {
			if (id == 0) glGenTextures(1, &id);
		}

		template<size_t C>
		[[nodiscard]] explicit Texture(const Image<C, unsigned char>& data) : Texture() {
			setData(data);
		}

		~Texture() {
			if (id > 0) glDeleteTextures(1, &id);
		}

		Texture(const Texture&) = delete;
		Texture& operator =(const Texture&) = delete;

		[[nodiscard]] Texture(Texture&& b) noexcept :
				id(std::exchange(b.id, 0)), width(b.width), height(b.height) {}
		Texture& operator =(Texture&& b) noexcept {
			if (this != &b) {
				std::swap(id, b.id);
				width = b.width;
				height = b.height;
			}
			return *this;
		}

		inline size_t getWidth() const noexcept { return width; }
		inline size_t getHeight() const noexcept { return height; }
		InternalFormat getInternalFormat(const size_t level) const noexcept {
			bind();
			int glInternalFormat;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, static_cast<int32_t>(level), GL_TEXTURE_INTERNAL_FORMAT, &glInternalFormat);
			return InternalFormat { glInternalFormat };
		}

		// returns the size of the texture at the given mipmap level, or { 0, 0 } if the level is too large
		glm::vec<2, size_t> getMipmapSize(const size_t level) const noexcept {
			glm::vec<2, size_t> size { width, height };
			for (size_t i = 0; i < level; i++) {
				bool xChanged = true, yChanged = true;
				if (size.x > 1) size.x /= 2;
				else xChanged = false;
				if (size.y > 1) size.y /= 2;
				else yChanged = false;
				if (!xChanged && !yChanged) break;
			}
			if ((size.y < 1) || (size.x < 1)) return { 0, 0 };
			return size;
		}

		inline Texture& scaleNearest() & noexcept { return setScaleMode(ScaleMode::NEAREST); }
		inline const Texture& scaleNearest() const& noexcept { return setScaleMode(ScaleMode::NEAREST); }
		[[nodiscard]] inline Texture&& scaleNearest() && noexcept { return std::move(setScaleMode(ScaleMode::NEAREST)); }
		Texture& setScaleMode(const ScaleMode scaleMode) & noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_underlying(scaleMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_underlying(getMagMode(scaleMode)));
			return *this;
		}
		const Texture& setScaleMode(const ScaleMode scaleMode) const& noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_underlying(scaleMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_underlying(getMagMode(scaleMode)));
			return *this;
		}
		[[nodiscard]] Texture&& setScaleMode(const ScaleMode scaleMode) && noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_underlying(scaleMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_underlying(getMagMode(scaleMode)));
			return std::move(*this);
		}
		Texture& setScaleMode(const ScaleMode minMode, const ScaleMode maxMode) & noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_underlying(minMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_underlying(getMagMode(maxMode)));
			return *this;
		}
		const Texture& setScaleMode(const ScaleMode minMode, const ScaleMode maxMode) const& noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_underlying(minMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_underlying(getMagMode(maxMode)));
			return *this;
		}
		[[nodiscard]] Texture&& setScaleMode(const ScaleMode minMode, const ScaleMode maxMode) && noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_underlying(minMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_underlying(getMagMode(maxMode)));
			return std::move(*this);
		}

		Texture& setWrapMode(const WrapMode wrapMode) & noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_underlying(wrapMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_underlying(wrapMode));
			return *this;
		}
		const Texture& setWrapMode(const WrapMode wrapMode) const& noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_underlying(wrapMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_underlying(wrapMode));
			return *this;
		}
		[[nodiscard]] Texture&& setWrapMode(const WrapMode wrapMode) && noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_underlying(wrapMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_underlying(wrapMode));
			return std::move(*this);
		}
		Texture& setWrapMode(const WrapMode uMode, const WrapMode vMode) & noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_underlying(uMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_underlying(vMode));
			return *this;
		}
		const Texture& setWrapMode(const WrapMode uMode, const WrapMode vMode) const& noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_underlying(uMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_underlying(vMode));
			return *this;
		}
		[[nodiscard]] Texture&& setWrapMode(const WrapMode uMode, const WrapMode vMode) && noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_underlying(uMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_underlying(vMode));
			return std::move(*this);
		}

		Texture& genMipmaps() & noexcept {
			bind();
			glGenerateMipmap(GL_TEXTURE_2D);
			return *this;
		}
		const Texture& genMipmaps() const& noexcept {
			bind();
			glGenerateMipmap(GL_TEXTURE_2D);
			return *this;
		}
		[[nodiscard]] Texture&& genMipmaps() && noexcept {
			bind();
			glGenerateMipmap(GL_TEXTURE_2D);
			return std::move(*this);
		}

		Texture& setMaxMipmapLevel(int level) & noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
			return *this;
		}
		const Texture& setMaxMipmapLevel(int level) const& noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
			return *this;
		}
		[[nodiscard]] Texture&& setMaxMipmapLevel(int level) && noexcept {
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
			return std::move(*this);
		}

		template<Format F, DataType T>
		struct [[nodiscard]] GetDataResult {
			friend class Texture;
			static constexpr Format format = F;
			static constexpr DataType dataType = T;
			static constexpr size_t pixelSize = getTexturePixelSize(F, T);
			using data_type = tex_data_type_type_t<T>;
			std::unique_ptr<data_type[]> const data;
			const size_t width, height;
			const size_t size; // number of elements in the data array
			const size_t sizeBytes; // size of the data array in bytes
		private:
			[[nodiscard]] GetDataResult(std::unique_ptr<data_type[]>&& d, size_t w, size_t h, size_t s, size_t b) :
					data(std::move(d)), width(w), height(h), size(s), sizeBytes(b) {};
		};
		template<Format F, DataType T>
		[[nodiscard]] auto getData(size_t level = 0) const -> std::enable_if_t<(getTexturePixelSize(F, T) > 0), GetDataResult<F, T>> {
			using data_type = tex_data_type_type_t<T>;
			constexpr size_t pixelSize = getTexturePixelSize(F, T);
			constexpr size_t pixelChannels = pixelSize / sizeof(data_type);
			static_assert(pixelChannels > 0);
			const auto mipmapSize = getMipmapSize(level);
			const auto w = mipmapSize.x, h = mipmapSize.y;
			const size_t size = w * h * pixelChannels;
			const size_t sizeBytes = w * h * pixelSize;
			auto data = std::make_unique<data_type[]>(size);
			if ((w * h) > 0) {
				bind();
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glGetTexImage(GL_TEXTURE_2D, static_cast<int>(level), getGLFormat(F), getGLDataType(T), data.get());
			}
			return { std::move(data), w, h, size, sizeBytes };
		}

		template<size_t C>
		inline void setData(const Image<C, unsigned char>& texData) noexcept {
			setData(0, texData);
		}
		template<size_t C>
		inline void setData(const size_t level, const Image<C, unsigned char>& texData) noexcept {
			setData(level, texData.get(), texData.getWidth(), texData.getHeight(), getInternalFormatFromImageChannels<C>(), getFormatFromImageChannels<C>(), DataType::UINT8);
		}
		template<typename T, typename std::enable_if_t<(tex_data_type_enum<T>::value), int> = 0>
		inline void setData(const T* texData, const size_t dataWidth, const size_t dataHeight, const InternalFormat internalFormat, const Format format) {
			setData(0, texData, dataWidth, dataHeight, internalFormat, format, tex_data_type_enum_v<T>);
		}
		template<typename T, typename std::enable_if_t<(tex_data_type_enum<T>::value), int> = 0>
		inline void setData(const size_t level, const T* texData, const size_t dataWidth, const size_t dataHeight, const InternalFormat internalFormat, const Format format) {
			setData(level, texData, dataWidth, dataHeight, internalFormat, format, tex_data_type_enum_v<T>);
		}
		inline void setData(const void* texData, const size_t dataWidth, const size_t dataHeight, const InternalFormat internalFormat, const Format format, const DataType type) noexcept {
			setData(0, texData, dataWidth, dataHeight, internalFormat, format, type);
		}
		void setData(const size_t level, const void* texData, const size_t dataWidth, const size_t dataHeight, const InternalFormat internalFormat, const Format format, const DataType type) noexcept {
			if (level == 0) {
				width = dataWidth;
				height = dataHeight;
			}
			bind();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, level, getGLInternalFormat(internalFormat), dataWidth, dataHeight, 0, getGLFormat(format), getGLDataType(type), texData);
		}

		template<size_t C>
		inline void setSubData(const size_t level, const Image<C, unsigned char>& subData, const glm::vec<2, size_t>& offset) {
			setSubData(level, subData.get(), offset, subData.getWidth(), subData.getHeight(), getFormatFromImageChannels<C>(), DataType::UINT8);
		}
		template<typename T, typename std::enable_if_t<(tex_data_type_enum<T>::value), int> = 0>
		inline void setSubData(const size_t level, const T* subData, const glm::vec<2, size_t>& offset, const size_t subWidth, const size_t subHeight, const Format format) {
			setSubData(level, subData, offset, subWidth, subHeight, format, tex_data_type_enum_v<T>);
		}
		void setSubData(const size_t level, const void* subData, const glm::vec<2, size_t>& offset, const size_t subWidth, const size_t subHeight, const Format format, const DataType type) noexcept {
			bind();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexSubImage2D(GL_TEXTURE_2D, level, offset.x, offset.y, subWidth, subHeight, getGLFormat(format), getGLDataType(type), subData);
		}

		inline void bind() const noexcept { bind(*this); }

		static inline void bind(const Texture& texture) noexcept {
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}
		static inline void unbind() noexcept {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

	private:

		static inline constexpr std::underlying_type_t<InternalFormat> getGLInternalFormat(const InternalFormat internalFormat) {
			return to_underlying(internalFormat);
		}
		static inline constexpr std::underlying_type_t<Format> getGLFormat(const Format format) {
			return to_underlying(format);
		}
		static inline constexpr std::underlying_type_t<DataType> getGLDataType(const DataType type) {
			return to_underlying(type);
		}

		template<size_t channels, std::enable_if_t<((channels > 0) && (channels <= 4)), int> = 0>
		static inline constexpr InternalFormat getInternalFormatFromImageChannels() noexcept {
			if constexpr (channels == 1) return InternalFormat::BYTEx1;
			else if constexpr (channels == 2) return InternalFormat::BYTEx2;
			else if constexpr (channels == 3) return InternalFormat::BYTEx3;
			else if constexpr (channels == 4) return InternalFormat::BYTEx4;
			else static_assert((channels > 0) && (channels <= 4));
		}
		template<size_t channels, std::enable_if_t<((channels > 0) && (channels <= 4)), int> = 0>
		static inline constexpr Format getFormatFromImageChannels() noexcept {
			if constexpr (channels == 1) return Format::RED;
			else if constexpr (channels == 2) return Format::RG;
			else if constexpr (channels == 3) return Format::RGB;
			else if constexpr (channels == 4) return Format::RGBA;
			else static_assert((channels > 0) && (channels <= 4));
		}

	};

}