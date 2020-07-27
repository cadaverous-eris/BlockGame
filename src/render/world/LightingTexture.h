#pragma once

#include <array>
#include <memory>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <iostream>

#include <glad/glad.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "world/BlockLight.h"
#include "world/chunk/chunk_consts.h"
#include "world/chunk/ChunkData.h"
#include "util/type_traits.h"
#include "util/math/math.h"

namespace eng {

	class LightingTexture {
	public:

	private:
		unsigned int id = 0;
		size_t width = 0;

		// TODO: add offset
		// TODO: add world position?

		static constexpr GLint internalFormat = GL_RGB4;
		static constexpr GLenum uploadFormat = GL_BGRA;
		static constexpr GLenum uploadDataType = GL_UNSIGNED_SHORT_4_4_4_4/*GL_UNSIGNED_SHORT_4_4_4_4_REV*/;

	public:

		[[nodiscard]] LightingTexture(const size_t width) : width(width) {
			if (id == 0) {
				glGenTextures(1, &id);
				bind();
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
				const auto data = std::make_unique<BlockLight[]>(width * width * width);
				std::fill(data.get(), data.get() + (width * width * width), BlockLight(0, 7, 1, 14)); // TODO: change to 0
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, width, width, 0, uploadFormat, uploadDataType, data.get());
			}
		}

		~LightingTexture() {
			if (id > 0) glDeleteTextures(1, &id);
		}

		LightingTexture(const LightingTexture&) = delete;
		LightingTexture& operator =(const LightingTexture&) = delete;

		[[nodiscard]] LightingTexture(LightingTexture&& b) noexcept :
				id(std::exchange(b.id, 0)), width(b.width) {}
		LightingTexture& operator =(LightingTexture&& b) noexcept {
			if (this != &b) {
				std::swap(id, b.id);
				width = b.width;
			}
			return *this;
		}

		inline size_t getWidth() const noexcept { return width; }


		// TODO: add applyOffset method
		// TODO: add setOffset method
		// TODO: add getOffset method
		// TODO: add clearRegion method to clear data within a given 3d region
		// TODO: add clear method to clear all data


		template<size_t W>
		void setSubData(const ChunkData<BlockLight, W>& subData, const glm::vec<3, size_t>& dataOffset) {
			bind();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexSubImage3D(GL_TEXTURE_3D, 0, dataOffset.x, dataOffset.y, dataOffset.z, W, W, W, uploadFormat, uploadDataType, subData.data());
		}


		inline void bind() const noexcept { bind(*this); }
		static inline void bind(const LightingTexture& texture) noexcept {
			glBindTexture(GL_TEXTURE_3D, texture.id);
		}
		// TODO: remove?
		static inline void unbind() noexcept {
			glBindTexture(GL_TEXTURE_3D, 0);
		}

	};

}