#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <iostream>

#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "Renderer.h"
#include "util/Color.h"
#include "util/type_traits.h"

namespace eng {

	class RenderBuffer;
	class Texture;

	class FrameBuffer {
	public:
		enum class Target : GLenum {
			FRAMEBUFFER = GL_FRAMEBUFFER,
			DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER,
			READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER,
		};
		enum class Attachment : GLenum {
			COLOR_0 = GL_COLOR_ATTACHMENT0,
			COLOR_1 = GL_COLOR_ATTACHMENT1,
			COLOR_2 = GL_COLOR_ATTACHMENT2,
			COLOR_3 = GL_COLOR_ATTACHMENT3,
			COLOR_4 = GL_COLOR_ATTACHMENT4,
			COLOR_5 = GL_COLOR_ATTACHMENT5,
			COLOR_6 = GL_COLOR_ATTACHMENT6,
			COLOR_7 = GL_COLOR_ATTACHMENT7,
			COLOR_8 = GL_COLOR_ATTACHMENT8,
			COLOR_9 = GL_COLOR_ATTACHMENT9,
			COLOR_10 = GL_COLOR_ATTACHMENT10,
			COLOR_11 = GL_COLOR_ATTACHMENT11,
			COLOR_12 = GL_COLOR_ATTACHMENT12,
			COLOR_13 = GL_COLOR_ATTACHMENT13,
			COLOR_14 = GL_COLOR_ATTACHMENT14,
			COLOR_15 = GL_COLOR_ATTACHMENT15,
			COLOR_16 = GL_COLOR_ATTACHMENT16,
			COLOR_17 = GL_COLOR_ATTACHMENT17,
			COLOR_18 = GL_COLOR_ATTACHMENT18,
			COLOR_19 = GL_COLOR_ATTACHMENT19,
			COLOR_20 = GL_COLOR_ATTACHMENT20,
			COLOR_21 = GL_COLOR_ATTACHMENT21,
			COLOR_22 = GL_COLOR_ATTACHMENT22,
			COLOR_23 = GL_COLOR_ATTACHMENT23,
			COLOR_24 = GL_COLOR_ATTACHMENT24,
			COLOR_25 = GL_COLOR_ATTACHMENT25,
			COLOR_26 = GL_COLOR_ATTACHMENT26,
			COLOR_27 = GL_COLOR_ATTACHMENT27,
			COLOR_28 = GL_COLOR_ATTACHMENT28,
			COLOR_29 = GL_COLOR_ATTACHMENT29,
			COLOR_30 = GL_COLOR_ATTACHMENT30,
			COLOR_31 = GL_COLOR_ATTACHMENT31,
			DEPTH = GL_DEPTH_ATTACHMENT,
			STENCIL = GL_STENCIL_ATTACHMENT,
			DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT,
		};
		enum class Status : GLenum {
			COMPLETE = GL_FRAMEBUFFER_COMPLETE,
			UNDEFINED = GL_FRAMEBUFFER_UNDEFINED,
			INCOMPLETE_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
			INCOMPLETE_MISSING_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
			INCOMPLETE_DRAW_BUFFER = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
			INCOMPLETE_READ_BUFFER = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
			UNSUPPORTED = GL_FRAMEBUFFER_UNSUPPORTED,
			INCOMPLETE_MULTISAMPLE = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
			INCOMPLETE_LAYER_TARGETS = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,
			ERROR = 0,
		};
		enum class FilterMode : GLenum {
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR, // linear filtering only works for blit operations if the buffer mask only includes the color buffer
		};
		static constexpr auto ColorAttachments = std::to_array({
			Attachment::COLOR_0, Attachment::COLOR_1, Attachment::COLOR_2, Attachment::COLOR_3, Attachment::COLOR_4, Attachment::COLOR_5, Attachment::COLOR_6, Attachment::COLOR_7, Attachment::COLOR_8, Attachment::COLOR_9, Attachment::COLOR_10, Attachment::COLOR_11, Attachment::COLOR_12, Attachment::COLOR_13, Attachment::COLOR_14, Attachment::COLOR_15, Attachment::COLOR_16, Attachment::COLOR_17, Attachment::COLOR_18, Attachment::COLOR_19, Attachment::COLOR_20, Attachment::COLOR_21, Attachment::COLOR_22, Attachment::COLOR_23, Attachment::COLOR_24, Attachment::COLOR_25, Attachment::COLOR_26, Attachment::COLOR_27, Attachment::COLOR_28, Attachment::COLOR_29, Attachment::COLOR_30, Attachment::COLOR_31,
		});
	private:
		uint32_t id;
		static uint32_t boundReadFBO, boundDrawFBO;
	public:
		FrameBuffer();

		~FrameBuffer();

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator =(const FrameBuffer&) = delete;

		FrameBuffer(FrameBuffer&&);
		FrameBuffer& operator =(FrameBuffer&&);

		Status getStatus(const Target target) const;

		bool isComplete(const Target target) const;

		void attachRenderBuffer(const Target target, const Attachment attachment, const RenderBuffer* const rbo) const;
		void attachTexture(const Target target, const Attachment attachment, const Texture* const tex, const int32_t level = 0) const;

		void clearColorBuffer(const glm::vec4& value, size_t bufferIndex = 0) const;
		void clearColorBuffer(const glm::ivec4& value, size_t bufferIndex = 0) const;
		void clearColorBuffer(const glm::uvec4& value, size_t bufferIndex = 0) const;
		void clearColorBuffer(const Color& value, size_t bufferIndex = 0) const;
		void clearDepthBuffer(const float depth) const;
		void clearStencil(const int32_t stencil) const;
		void clearDepthStencil(const float depth, const int32_t stencil) const;

		static void blit(const FrameBuffer* src, const FrameBuffer* dest, const glm::ivec2& srcSize, const glm::ivec2& destSize, BufferMask mask, FilterMode filter = FilterMode::NEAREST);
		static void blit(const FrameBuffer* src, const FrameBuffer* dest, const glm::ivec2& srcMin, const glm::ivec2& srcMax, const glm::ivec2& destMin, const glm::ivec2& destMax, BufferMask mask, FilterMode filter = FilterMode::NEAREST);

		void bind(const Target target) const;

		static void bindFrameBuffer(const FrameBuffer* fbo, const Target target);

		static void unbind(const Target target);
	};

	using FrameBufferTarget = FrameBuffer::Target;
	using FrameBufferAttachment = FrameBuffer::Attachment;
	using FrameBufferStatus = FrameBuffer::Status;
	using FrameBufferFilterMode = FrameBuffer::FilterMode;

	std::string to_string(const FrameBufferStatus status);
	std::ostream& operator <<(std::ostream& os, const FrameBufferStatus status);

}