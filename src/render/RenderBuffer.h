#pragma once

#include <glm/vec2.hpp>

#include "Renderer.h"
#include "render/texture/Texture.h"
#include "util/type_traits.h"

namespace eng {

	class FrameBuffer;

	class RenderBuffer {
		friend class FrameBuffer;
	public:
		using InternalFormat = TextureInternalFormat;
	private:
		uint32_t id;
		InternalFormat internalFormat;
		size_t width = 0, height = 0;
		static uint32_t boundRBO;
	public:
		RenderBuffer(const InternalFormat internalFormat);
		RenderBuffer(const InternalFormat internalFormat, const size_t width, const size_t height);
		RenderBuffer(const InternalFormat internalFormat, const glm::vec<2, size_t>& size);

		~RenderBuffer();

		RenderBuffer(const RenderBuffer&) = delete;
		RenderBuffer& operator =(const RenderBuffer&) = delete;

		RenderBuffer(RenderBuffer&&);
		RenderBuffer& operator =(RenderBuffer&&);

		InternalFormat getInternalFormat() const;

		size_t getWidth() const;
		size_t getHeight() const;
		glm::vec<2, size_t> getSize() const;

		RenderBuffer& setSize(const size_t width, const size_t height) &;
		const RenderBuffer& setSize(const size_t width, const size_t height) const&;
		RenderBuffer&& setSize(const size_t width, const size_t height) &&;
		RenderBuffer& setSize(const glm::vec<2, size_t>& size) &;
		const RenderBuffer& setSize(const glm::vec<2, size_t>& size) const&;
		RenderBuffer&& setSize(const glm::vec<2, size_t>& size) &&;

		inline bool isBound() const { return id && (id == boundRBO); }

		inline void bind() const { bind(*this); }

		static void bind(const RenderBuffer& renderBuffer);
		static void unbind();

	private:
		static inline std::underlying_type_t<InternalFormat> getGLInternalFormat(const InternalFormat internalFormat) {
			return to_underlying(internalFormat);
		}
	};

}