#pragma once

#include <cstdint>
#include <vector>
#include <span>
#include <cassert>

#include "Renderer.h"
#include "util/type_traits.h"

namespace eng {

	// TODO: add variants that use uint8_t and uint16_t instead of uint32_t as the index type
	class IndexBuffer {
	public:
		using DrawHint = BufferDrawHint;
		using Index = uint32_t;
		static constexpr size_t index_size = sizeof(Index);
	private:
		uint32_t id;
		DrawHint usage;
		size_t count = 0;
		static uint32_t boundEBO;
	public:
		IndexBuffer() noexcept;

		~IndexBuffer() noexcept;

		IndexBuffer(IndexBuffer&&) noexcept;
		IndexBuffer& operator =(IndexBuffer&&) noexcept;

		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator =(const IndexBuffer&) = delete;

		void setData(const Index* data, size_t count) noexcept;
		void setData(const Index* data, size_t count, DrawHint usage) noexcept;
		template<size_t Extent>
		void setData(const std::span<const Index, Extent> dataSpan) noexcept {
			if (dataSpan.empty()) setData(nullptr, 0);
			else setData((void*) dataSpan.data(), dataSpan.size());
		}
		template<size_t Extent>
		void setData(const std::span<const Index, Extent> dataSpan, DrawHint usage) noexcept {
			if (!dataSpan.empty()) setData(nullptr, 0, usage);
			else setData((void*) dataSpan.data, dataSpan.size(), usage);
		}

		// start is the offset in indices, data is the data to upload, count is the number of elements in the array
		void subData(size_t start, const Index* data, size_t count) noexcept;
		template<size_t Extent>
		void subData(size_t start, const std::span<const Index, Extent> dataSpan) noexcept {
			if (!dataSpan.empty())
				subData(start, (void*) dataSpan.data(), dataSpan.size());
		}

		void orphan() noexcept;

		inline size_t getCount() const noexcept { return count; }
		inline bool isEmpty() const noexcept { return count == 0; }

		void bind() const noexcept;
		static void unbind() noexcept;
	};

}