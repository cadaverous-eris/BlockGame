#pragma once

#include <optional>
#include <string>
#include <span>
#include <stdexcept>

#include "nbt_types.h"

namespace nbt {

    class NBT;

}

namespace nbt::parsing {

	inline constexpr std::string_view infinity_string = "inf";
	inline constexpr std::string_view nan_string = "nan";

	struct ParseError : std::runtime_error {
		ParseError(const std::string_view str, const size_t pos, const std::string& what);
		ParseError(const std::string& what);
	};


	NBT parseNBT(std::span<const unsigned char> bytes);

	NBT parseSNBT(std::string_view str);

}