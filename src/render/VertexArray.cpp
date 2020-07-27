#include "VertexArray.h"

#include <utility>
#include <iostream>

namespace eng {

	uint32_t VertexArray::boundVAO = 0;

	VertexArray::VertexArray() noexcept : id() {
		glGenVertexArrays(1, &id); // generate VAO (vertex array object)
		bind();
	}

	VertexArray::~VertexArray() noexcept {
		if (id != 0) {
			if (boundVAO == id) boundVAO = 0;
			glDeleteVertexArrays(1, &id);
		}
	}

	VertexArray::VertexArray(VertexArray&& b) noexcept :
			id(std::exchange(b.id, 0)) {}
	VertexArray& VertexArray::operator =(VertexArray&& b) noexcept {
		if (&b != this)
			std::swap(id, b.id);
		return *this;
	}

	void VertexArray::setVertexFormat(const VertexFormat& format) const {
		bind();
		std::size_t i = 0;
		for (const VertexAttribute& attribute : format) {
			if (attribute.getShaderType() == VertexAttribShaderType::INTEGER) {
				glVertexAttribIPointer(i, attribute.getCount(), attribute.getGLType(), format.getStride(), (void*) attribute.getOffset());
			} else {
				glVertexAttribPointer(i, attribute.getCount(), attribute.getGLType(), attribute.isNormalized() ? GL_TRUE : GL_FALSE, format.getStride(), (void*) attribute.getOffset());
			}
			glEnableVertexAttribArray(i);
			i++;
		}
	}

	void VertexArray::draw(DrawMode mode, const IndexBuffer& ebo) const noexcept {
		bind();
		ebo.bind();
		glDrawElements(static_cast<GLenum>(mode), ebo.getCount(), GL_UNSIGNED_INT, nullptr);
	}
	void VertexArray::draw(DrawMode mode, const IndexBuffer& ebo, size_t start, size_t count) const noexcept {
		const auto eboSize = ebo.getCount();
		if ((start >= eboSize) || (count >= (eboSize - start))) return;
		bind();
		ebo.bind();
		glDrawElements(static_cast<GLenum>(mode), count, GL_UNSIGNED_INT, (const void*) start);
	}
	void VertexArray::draw(DrawMode mode, size_t start, size_t count) const noexcept {
		bind();
		glDrawArrays(static_cast<GLenum>(mode), start, count);
	}

	void VertexArray::bind() const noexcept {
		if (boundVAO != id) {
			boundVAO = id;
			glBindVertexArray(id);
		}
	}
	void VertexArray::unbind() noexcept {
		if (boundVAO != 0) {
			boundVAO = 0;
			glBindVertexArray(0);
		}
	}

}