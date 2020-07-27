#include "VertexBuffer.h"

#include <utility>

#include "Renderer.h"

namespace eng {

	uint32_t VertexBuffer::boundVBO = 0;

	VertexBuffer::VertexBuffer() noexcept : id(), usage(DrawHint::STATIC) {
		glGenBuffers(1, &id);
	}

	VertexBuffer::~VertexBuffer() noexcept {
		if (id != 0) {
			if (boundVBO == id) boundVBO = 0;
			glDeleteBuffers(1, &id);
		}
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& b) noexcept :
			id(std::exchange(b.id, 0)), usage(b.usage), size(b.size) {}
	VertexBuffer& VertexBuffer::operator =(VertexBuffer&& b) noexcept {
		if (&b != this) {
			std::swap(id, b.id);
			usage = b.usage;
			size = b.size;
		}
		return *this;
	}

	void VertexBuffer::setData(const void* data, size_t size) noexcept {
		this->size = size; // update the buffer size
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<GLenum>(usage));
	}
	void VertexBuffer::setData(const void* data, size_t size, DrawHint usage) noexcept {
		this->usage = usage; // update the buffer usage
		setData(data, size);
	}

	void VertexBuffer::subData(size_t start, const void* data, size_t size) noexcept {
		bind();
		glBufferSubData(GL_ARRAY_BUFFER, start, size, data);
	}

	void VertexBuffer::orphan() noexcept {
		bind();
		if (size > 0)
			glBufferData(GL_ARRAY_BUFFER, size, nullptr, static_cast<GLenum>(usage));
	}

	void VertexBuffer::bind() const noexcept {
		if (boundVBO != id) {
			boundVBO = id;
			glBindBuffer(GL_ARRAY_BUFFER, id);
		}
	}
	void VertexBuffer::unbind() noexcept {
		if (boundVBO != 0) {
			boundVBO = 0;
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

}