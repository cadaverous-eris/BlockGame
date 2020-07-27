#pragma once

#include <optional>
#include <string>

#include "jonk_types.h"

namespace jonk {

	class Jonk;

}

namespace jonk::parsing {

	inline constexpr std::string_view null_string = "null";

	inline constexpr std::string_view true_string = "true";
	inline constexpr std::string_view false_string = "false";

	inline constexpr std::string_view infinity_string = "Infinity";
	inline constexpr std::string_view nan_string = "NaN";

	struct ParseError : std::runtime_error {
		ParseError(const std::string_view str, const size_t pos, const std::string& what);
	};


	Jonk parseJonk(std::string_view str);

}