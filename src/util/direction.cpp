#include "direction.h"

namespace eng {

	std::string to_string(const Direction d) {
		switch (d) {
		case Direction::DOWN:  return "Down";
		case Direction::UP:    return "Up";
		case Direction::NORTH: return "North";
		case Direction::SOUTH: return "South";
		case Direction::WEST:  return "West";
		case Direction::EAST:  return "East";
		default:               return "Undefined Direction";
		}
	}
	std::ostream& operator <<(std::ostream& stream, const Direction d) { return stream << to_string(d); }

	std::string to_string(const Axis a) {
		switch (a) {
		case Axis::X: return "X";
		case Axis::Y: return "Y";
		case Axis::Z: return "Z";
		default:      return "Undefined Axis";
		}
	}
	std::ostream& operator <<(std::ostream& stream, const Axis a) { return stream << to_string(a); }

	std::string to_string(const AxisDirection ad) {
		switch (ad) {
		case AxisDirection::NEGATIVE: return "Negative";
		case AxisDirection::POSITIVE: return "Positive";
		default:                      return "Undefined AxisDirection";
		}
	}
	std::ostream& operator <<(std::ostream& stream, const AxisDirection ad) { return stream << to_string(ad); }

	std::string to_string(const RotationDirection rd) {
		switch (rd) {
		case RotationDirection::CLOCKWISE:         return "Clockwise";
		case RotationDirection::COUNTER_CLOCKWISE: return "Counter Clockwise";
		default:                                   return "Undefined RotationDirection";
		}
	}
	std::ostream& operator <<(std::ostream& stream, const RotationDirection rd) { return stream << to_string(rd); }

}