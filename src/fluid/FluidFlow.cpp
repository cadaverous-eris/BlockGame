#include "FluidFlow.h"


namespace eng {

	static std::array<std::string, 27> fluidFlowStrings {
		"Still",
		"Down",
		"Up",
		"North",
		"South",
		"West",
		"East",
		"DownNorth",
		"DownSouth",
		"DownWest",
		"DownEast",
		"UpNorth",
		"UpSouth",
		"UpWest",
		"UpEast",
		"NorthWest",
		"NorthEast",
		"SouthWest",
		"SouthEast",
		"DownNorthWest",
		"DownNorthEast",
		"DownSouthWest",
		"DownSouthEast",
		"UpNorthWest",
		"UpNorthEast",
		"UpSouthWest",
		"UpSouthEast",
	};

	std::string to_string(const FluidFlow flow) {
		return fluidFlowStrings[flow.getId()];
	}

	std::ostream& operator <<(std::ostream& stream, const FluidFlow flow) {
		return stream << fluidFlowStrings[flow.getId()];
	}

}