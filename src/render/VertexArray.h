#pragma once

#include <cstdint>
#include <span>

#include "Renderer.h"
#include "VertexFormat.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace eng {

	class VertexArray {
	private:
		uint32_t id;
		static uint32_t boundVAO;
	public:
		VertexArray() noexcept;

		~VertexArray() noexcept;

		VertexArray(const VertexArray&) = delete;
		VertexArray& operator =(const VertexArray&) = delete;

		VertexArray(VertexArray&&) noexcept;
		VertexArray& operator =(VertexArray&&) noexcept;

		void setVertexFormat(const VertexFormat& format) const;

		// draws using an index buffer object
		void draw(DrawMode mode, const IndexBuffer& ebo) const noexcept;
		// draws using an index buffer object
		// start specifies the starting position within the ebo, count specifies the number of elements to render
		void draw(DrawMode mode, const IndexBuffer& ebo, size_t start, size_t count) const noexcept;
		// draws vertices from the currently bound vbo
		// start specifies index of the first vertex to draw, count specifies the number of vertices to draw
		void draw(DrawMode mode, size_t start, size_t count) const noexcept;
		// draws vertices from the currently bound vbo using a span of indices instead of an ebo
		template<size_t Extent>
		void draw(DrawMode mode, const std::span<const IndexBuffer::Index, Extent> indices) const noexcept {
			bind();
			IndexBuffer::unbind();
			if (!indices.empty())
				glDrawElements(static_cast<GLenum>(mode), indices.size(), GL_UNSIGNED_INT, (const void*) indices.data());
		}

		void bind() const noexcept;
		static void unbind() noexcept;
	};

}