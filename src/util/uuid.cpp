#include "uuid.h"

#include <random>
//#include <iomanip>
//#include <sstream>

#include <fmt/core.h>

namespace eng {

    static std::random_device uuid4Rng;
	static std::uniform_int_distribution<uint64_t> dist(0, ~uint64_t{0});
	uuid uuid::v4() {
		const uint64_t ab = dist(uuid4Rng);
		const uint64_t cd = dist(uuid4Rng);
		return uuid {
			((ab & 0xFFFFFFFFFFFF0FFFull) | 0x0000000000004000ull),
			((cd & 0x3FFFFFFFFFFFFFFFull) | 0x8000000000000000ull)
		};
	}

    std::string uuid::toString() const {
		// xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
		return fmt::format(
			"{:0>8x}-{:0>4x}-{:0>4x}-{:0>4x}-{:0>12x}",
			(ab >> 32),
			((ab >> 16) & 0xFFFF),
			(ab & 0xFFFF),
			(cd >> 48),
			(cd & 0xFFFFFFFFFFFFull)
		);
		//std::ostringstream sstr;
		//sstr << std::hex << std::nouppercase << std::setfill('0');
		//sstr << std::setw(8) << (ab >> 32);
		//sstr << '-';
		//sstr << std::setw(4) << ((ab >> 16) & 0xFFFF);
		//sstr << '-';
		//sstr << std::setw(4) << (ab & 0xFFFF);
		//sstr << '-';
		//sstr << std::setw(4) << (cd >> 48);
		//sstr << '-';
		//sstr << std::setw(12) << (cd & 0xFFFFFFFFFFFFull);
		//return sstr.str();
	}

	std::ostream& operator <<(std::ostream& os, const uuid& id) {
		return os << id.toString();
	}

}