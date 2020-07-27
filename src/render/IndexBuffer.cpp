#include "IndexBuffer.h"

#include <utility>

#include "Renderer.h"

namespace eng {

	uint32_t IndexBuffer::boundEBO = 0;

	IndexBuffer::IndexBuffer() noexcept : id(), usage(DrawHint::STATIC) {
		glGenBuffers(1, &id);
	}

	IndexBuffer::~IndexBuffer() noexcept {
		if (id != 0) {
			if (boundEBO == id) boundEBO = 0;
			glDeleteBuffers(1, &id);
		}
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& b) noexcept :
			id(std::exchange(b.id, 0)), usage(b.usage), count(b.count) {}
	IndexBuffer& IndexBuffer::operator =(IndexBuffer&& b) noexcept {
		if (&b != this) {
			std::swap(id, b.id);
			usage = b.usage;
			count = b.count;
		}
		return *this;
	}

	void IndexBuffer::setData(const Index* data, size_t count) noexcept {
		this->count = count; // update the index count
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * index_size, data, static_cast<GLenum>(usage));
	}
	void IndexBuffer::setData(const Index* data, size_t count, DrawHint usage) noexcept {
		this->usage = usage; // update the buffer usage
		setData(data, count);
	}

	void IndexBuffer::subData(size_t start, const Index* data, size_t count) noexcept {
		bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, start * index_size, count * index_size, data);
	}

	void IndexBuffer::orphan() noexcept {
		bind();
		if (count > 0)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * index_size, nullptr, static_cast<GLenum>(usage));
	}

	void IndexBuffer::bind() const noexcept {
		if (boundEBO != id) {
			boundEBO = id;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		}
	}
	void IndexBuffer::unbind() noexcept {
		if (boundEBO != 0) {
			boundEBO = 0;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

}