#include "FrameBuffer.h"

#include <utility>
#include <stdexcept>
#include <iostream>

#include "render/texture/Texture.h"
#include "render/RenderBuffer.h"

namespace eng {

	uint32_t FrameBuffer::boundReadFBO = 0;
	uint32_t FrameBuffer::boundDrawFBO = 0;

	FrameBuffer::FrameBuffer() {
		glGenFramebuffers(1, &id);
	}

	FrameBuffer::FrameBuffer(FrameBuffer&& b) :
			id(std::exchange(b.id, 0)) {}
	FrameBuffer& FrameBuffer::operator =(FrameBuffer&& b) {
		if (&b != this)
			std::swap(id, b.id);
		return *this;
	}

	FrameBuffer::~FrameBuffer() {
		if (id != 0) {
			if (boundDrawFBO == id) boundDrawFBO = 0;
			if (boundReadFBO == id) boundReadFBO = 0;
			glDeleteFramebuffers(1, &id);
		}
	}
	
	FrameBuffer::Status FrameBuffer::getStatus(const Target target) const {
		bind((target == Target::READ_FRAMEBUFFER) ? target : Target::DRAW_FRAMEBUFFER);
		return Status { glCheckFramebufferStatus(to_underlying(target)) };
	}

	bool FrameBuffer::isComplete(const Target target) const {
		bind((target == Target::READ_FRAMEBUFFER) ? target : Target::DRAW_FRAMEBUFFER);
		return Status{ glCheckFramebufferStatus(to_underlying(target)) } == Status::COMPLETE;
	}

	void FrameBuffer::attachRenderBuffer(const Target target, const Attachment attachment, const RenderBuffer* const rbo) const {
		bind(target);
		const uint32_t rboId = (rbo) ? rbo->id : 0;
		glFramebufferRenderbuffer(to_underlying(target), to_underlying(attachment), GL_RENDERBUFFER, rboId);
	}
	void FrameBuffer::attachTexture(const Target target, const Attachment attachment, const Texture* const tex, const int32_t level) const {
		bind(target);
		const uint32_t texId = (tex) ? tex->id : 0;
		glFramebufferTexture2D(to_underlying(target), to_underlying(attachment), GL_TEXTURE_2D, texId, level);
	}

	void FrameBuffer::clearColorBuffer(const glm::vec4& value, size_t bufferIndex) const {
		bind(Target::DRAW_FRAMEBUFFER);
		glClearBufferfv(GL_COLOR, bufferIndex, reinterpret_cast<const float*>(&value));
	}
	void FrameBuffer::clearColorBuffer(const glm::ivec4& value, size_t bufferIndex) const {
		bind(Target::DRAW_FRAMEBUFFER);
		glClearBufferiv(GL_COLOR, bufferIndex, reinterpret_cast<const int32_t*>(&value));
	}
	void FrameBuffer::clearColorBuffer(const glm::uvec4& value, size_t bufferIndex) const {
		bind(Target::DRAW_FRAMEBUFFER);
		glClearBufferuiv(GL_COLOR, bufferIndex, reinterpret_cast<const uint32_t*>(&value));
	}
	void FrameBuffer::clearColorBuffer(const Color& value, size_t bufferIndex) const {
		bind(Target::DRAW_FRAMEBUFFER);
		const float v[4] { value.r / 255.0f, value.g / 255.0f, value.b / 255.0f, value.a / 255.0f };
		glClearBufferfv(GL_COLOR, bufferIndex, v);
	}
	void FrameBuffer::clearDepthBuffer(const float depth) const {
		bind(Target::DRAW_FRAMEBUFFER);
		glClearBufferfv(GL_DEPTH, 0, &depth);
	}
	void FrameBuffer::clearStencil(const int32_t stencil) const {
		bind(Target::DRAW_FRAMEBUFFER);
		glClearBufferiv(GL_STENCIL, 0, &stencil);
	}
	void FrameBuffer::clearDepthStencil(const float depth, const int32_t stencil) const {
		bind(Target::DRAW_FRAMEBUFFER);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
	}

	inline static bool blitRegionsOverlap(const glm::ivec2& srcMin, const glm::ivec2& srcMax, const glm::ivec2& destMin, const glm::ivec2& destMax) {
		return !((destMin.x >= srcMax.x) ||
				 (destMax.x <= srcMin.x) ||
				 (destMax.y <= srcMin.y) ||
				 (destMin.y >= srcMax.y));
	}

	void FrameBuffer::blit(const FrameBuffer* src, const FrameBuffer* dest, const glm::ivec2& srcSize, const glm::ivec2& destSize, const BufferMask mask, const FilterMode filter) {
		#ifndef NDEBUG
		if ((srcSize.x <= 0) || (srcSize.y <= 0) || (destSize.x <= 0) || (destSize.y <= 0)) {
			std::cerr << "FrameBuffer blit regions must have positive, nonzero width and height\n";
			throw std::runtime_error("FrameBuffer blit regions must have positive, nonzero width and height");
		}
		if (src == dest) {
			std::cerr << "Cannot blit overlapping regions of the same framebuffer\n";
			throw std::runtime_error("Cannot blit overlapping regions of the same framebuffer");
		}
		if ((filter == FilterMode::LINEAR) && (mask != BufferMask::COLOR)) {
			std::cerr << "Linear filtering can only be used for color buffer blitting\n";
			throw std::runtime_error("Linear filtering can only be used for color buffer blitting");
		}
		#endif
		bindFrameBuffer(src, Target::READ_FRAMEBUFFER);
		bindFrameBuffer(dest, Target::DRAW_FRAMEBUFFER);
		glBlitFramebuffer(0, 0, srcSize.x, srcSize.y, 0, 0, destSize.x, destSize.y, to_underlying(mask), to_underlying(filter));
	}
	void FrameBuffer::blit(const FrameBuffer* src, const FrameBuffer* dest, const glm::ivec2& srcMin, const glm::ivec2& srcMax, const glm::ivec2& destMin, const glm::ivec2& destMax, const BufferMask mask, const FilterMode filter) {
		#ifndef NDEBUG
		if ((srcMax.x <= srcMin.x) || (srcMax.y <= srcMin.y) || (destMax.x <= destMin.x) || (destMax.y <= destMin.y)) {
			std::cerr << "FrameBuffer blit regions must have positive, nonzero width and height\n";
			throw std::runtime_error("FrameBuffer blit regions must have positive, nonzero width and height");
		}
		if ((src == dest) && blitRegionsOverlap(srcMin, srcMax, destMin, destMax)) {
			std::cerr << "Cannot blit overlapping regions of the same framebuffer\n";
			throw std::runtime_error("Cannot blit overlapping regions of the same framebuffer");
		}
		if ((filter == FilterMode::LINEAR) && (mask != BufferMask::COLOR)) {
			std::cerr << "Linear filtering can only be used for color buffer blitting\n";
			throw std::runtime_error("Linear filtering can only be used for color buffer blitting");
		}
		#endif
		bindFrameBuffer(src, Target::READ_FRAMEBUFFER);
		bindFrameBuffer(dest, Target::DRAW_FRAMEBUFFER);
		glBlitFramebuffer(srcMin.x, srcMin.y, srcMax.x, srcMax.y, destMin.x, destMin.y, destMax.x, destMax.y, to_underlying(mask), to_underlying(filter));
	}

	void FrameBuffer::bind(const FrameBuffer::Target target) const {
		switch (target) {
		case Target::FRAMEBUFFER:
			if ((boundDrawFBO != id) || (boundReadFBO != id)) {
				boundDrawFBO = boundReadFBO = id;
				glBindFramebuffer(to_underlying(target), id);
			}
			break;
		case Target::DRAW_FRAMEBUFFER:
			if (boundDrawFBO != id) {
				boundDrawFBO = id;
				glBindFramebuffer(to_underlying(target), id);
			}
			break;
		case Target::READ_FRAMEBUFFER:
			if (boundReadFBO != id) {
				boundReadFBO = id;
				glBindFramebuffer(to_underlying(target), id);
			}
			break;
		}
	}

	void FrameBuffer::bindFrameBuffer(const FrameBuffer* fbo, const Target target) {
		const uint32_t id = (fbo) ? fbo->id : 0;
		switch (target) {
		case Target::FRAMEBUFFER:
			if ((boundDrawFBO != id) || (boundReadFBO != id)) {
				boundDrawFBO = boundReadFBO = id;
				glBindFramebuffer(to_underlying(target), id);
			}
			break;
		case Target::DRAW_FRAMEBUFFER:
			if (boundDrawFBO != id) {
				boundDrawFBO = id;
				glBindFramebuffer(to_underlying(target), id);
			}
			break;
		case Target::READ_FRAMEBUFFER:
			if (boundReadFBO != id) {
				boundReadFBO = id;
				glBindFramebuffer(to_underlying(target), id);
			}
			break;
		}
	}

	void FrameBuffer::unbind(const FrameBuffer::Target target) {
		switch (target) {
		case Target::FRAMEBUFFER:
			if ((boundDrawFBO != 0) || (boundReadFBO != 0)) {
				boundDrawFBO = boundReadFBO = 0;
				glBindFramebuffer(to_underlying(target), 0);
			}
			break;
		case Target::DRAW_FRAMEBUFFER:
			if (boundDrawFBO != 0) {
				boundDrawFBO = 0;
				glBindFramebuffer(to_underlying(target), 0);
			}
			break;
		case Target::READ_FRAMEBUFFER:
			if (boundReadFBO != 0) {
				boundReadFBO = 0;
				glBindFramebuffer(to_underlying(target), 0);
			}
			break;
		}
	}

	std::string to_string(const FrameBufferStatus status) {
		switch (status) {
		case FrameBufferStatus::COMPLETE:                      return "COMPLETE";
		case FrameBufferStatus::UNDEFINED:                     return "UNDEFINED";
		case FrameBufferStatus::INCOMPLETE_ATTACHMENT:         return "INCOMPLETE_ATTACHMENT";
		case FrameBufferStatus::INCOMPLETE_MISSING_ATTACHMENT: return "INCOMPLETE_MISSING_ATTACHMENT";
		case FrameBufferStatus::INCOMPLETE_DRAW_BUFFER:        return "INCOMPLETE_DRAW_BUFFER";
		case FrameBufferStatus::INCOMPLETE_READ_BUFFER:        return "INCOMPLETE_READ_BUFFER";
		case FrameBufferStatus::UNSUPPORTED:                   return "UNSUPPORTED";
		case FrameBufferStatus::INCOMPLETE_MULTISAMPLE:        return "INCOMPLETE_MULTISAMPLE";
		case FrameBufferStatus::INCOMPLETE_LAYER_TARGETS:      return "INCOMPLETE_LAYER_TARGETS";
		default:                                               return "ERROR";
		}
	}
	std::ostream& operator <<(std::ostream& os, const FrameBufferStatus status) {
		return os << to_string(status);
	}

}