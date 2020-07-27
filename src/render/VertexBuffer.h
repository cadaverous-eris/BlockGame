#pragma once

#include <cstdint>
#include <vector>
#include <span>
#include <cassert>

#include "Renderer.h"
#include "util/type_traits.h"

namespace eng {

	class VertexBuffer {
	public:
		using DrawHint = BufferDrawHint;
	private:
		uint32_t id;
		DrawHint usage;
		size_t size = 0;
		static uint32_t boundVBO;
	public:
		VertexBuffer() noexcept;

		~VertexBuffer() noexcept;

		VertexBuffer(VertexBuffer&&) noexcept;
		VertexBuffer& operator =(VertexBuffer&&) noexcept;

		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator =(const VertexBuffer&) = delete;

		void setData(const void* data, size_t size) noexcept;
		void setData(const void* data, size_t size, DrawHint usage) noexcept;
		template<typename T, size_t Extent>
		void setData(const std::span<T, Extent> dataSpan) noexcept {
			if (dataSpan.empty()) setData(nullptr, 0);
			else setData((void*) dataSpan.data(), dataSpan.size_bytes());
		}
		template<typename T, size_t Extent>
		void setData(const std::span<T, Extent> dataSpan, DrawHint usage) noexcept {
			if (dataSpan.empty()) setData(nullptr, 0, usage);
			else setData((void*) dataSpan.data(), dataSpan.size_bytes(), usage);
		}

		// start is the offset in bytes, data is the data to upload, size is the size of data in bytes
		void subData(size_t start, const void* data, size_t size) noexcept;
		// start is the offset in bytes, first and last are iterators to the data to upload
		template<typename T, size_t Extent>
		void subData(size_t start, const std::span<T, Extent> dataSpan) noexcept {
			if (!dataSpan.empty())
				subData(start, (void*) dataSpan.data(), dataSpan.size_bytes());
		}

		void orphan() noexcept;

		size_t getSize() const noexcept { return size; }
		inline bool isEmpty() const noexcept { return size == 0; }

		void bind() const noexcept;
		static void unbind() noexcept;
	};

}