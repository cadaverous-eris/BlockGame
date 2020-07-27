#pragma once


#include "util/math/math.h"

namespace eng {

	inline constexpr size_t chunk_width = 32; // width, depth, and height of a chunk in blocks
	static_assert(is_pow2(chunk_width), "Chunk size must be a power of 2");

	inline constexpr size_t chunk_layer_size = chunk_width * chunk_width; // number of blocks per 2d slice of the chunk
	inline constexpr size_t chunk_volume = chunk_layer_size * chunk_width; // total number of blocks in a chunk
	inline constexpr size_t chunk_log2_width = constexpr_floor_log2(chunk_width);

}