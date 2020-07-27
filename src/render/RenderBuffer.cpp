#include "RenderBuffer.h"

#include <utility>

namespace eng {

	uint32_t RenderBuffer::boundRBO = 0;

	RenderBuffer::RenderBuffer(const RenderBuffer::InternalFormat internalFormat) :
			internalFormat(internalFormat) {
		glGenRenderbuffers(1, &id);
	}
	RenderBuffer::RenderBuffer(const RenderBuffer::InternalFormat internalFormat, const size_t width, const size_t height) :
			internalFormat(internalFormat), width(width), height(height) {
		glGenRenderbuffers(1, &id);
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, getGLInternalFormat(internalFormat), width, height);
	}
	RenderBuffer::RenderBuffer(const RenderBuffer::InternalFormat internalFormat, const glm::vec<2, size_t>& size) :
			RenderBuffer(internalFormat, size.x, size.y) {}

	RenderBuffer::~RenderBuffer() {
		if (id != 0) {
			if (id == boundRBO) boundRBO = 0;
			glDeleteRenderbuffers(1, &id);
		}
	}

	RenderBuffer::RenderBuffer(RenderBuffer&& b) :
			id(std::exchange(b.id, 0)),
			internalFormat(b.internalFormat),
			width(b.width), height(b.height) {}
	RenderBuffer& RenderBuffer::operator =(RenderBuffer&& b) {
		if (&b != this) {
			std::swap(id, b.id);
			std::swap(internalFormat, b.internalFormat);
			std::swap(width, b.width);
			std::swap(height, b.height);
		}
		return *this;
	}

	RenderBuffer::InternalFormat RenderBuffer::getInternalFormat() const { return internalFormat; }

	size_t RenderBuffer::getWidth() const { return width; }
	size_t RenderBuffer::getHeight() const { return height; }
	glm::vec<2, size_t> RenderBuffer::getSize() const { return { width, height }; }

	RenderBuffer& RenderBuffer::setSize(const size_t width, const size_t height) & {
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, getGLInternalFormat(internalFormat), width, height);
		return *this;
	}
	const RenderBuffer& RenderBuffer::setSize(const size_t width, const size_t height) const& {
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, getGLInternalFormat(internalFormat), width, height);
		return *this;
	}
	RenderBuffer&& RenderBuffer::setSize(const size_t width, const size_t height) && {
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, getGLInternalFormat(internalFormat), width, height);
		return std::move(*this);
	}
	RenderBuffer& RenderBuffer::setSize(const glm::vec<2, size_t>& size) & {
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, getGLInternalFormat(internalFormat), size.x, size.y);
		return *this;
	}
	const RenderBuffer& RenderBuffer::setSize(const glm::vec<2, size_t>& size) const& {
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, getGLInternalFormat(internalFormat), size.x, size.y);
		return *this;
	}
	RenderBuffer&& RenderBuffer::setSize(const glm::vec<2, size_t>& size) && {
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, getGLInternalFormat(internalFormat), size.x, size.y);
		return std::move(*this);
	}

	void RenderBuffer::bind(const RenderBuffer& renderBuffer) {
		if (renderBuffer.id && (renderBuffer.id != boundRBO)) {
			boundRBO = renderBuffer.id;
			glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer.id);
		}
	}

	void RenderBuffer::unbind() {
		if (boundRBO) {
			boundRBO = 0;
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

}