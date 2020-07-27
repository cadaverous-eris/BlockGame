#pragma once

#include <array>
#include <type_traits>
#include <string>
#include <iostream>

#include <glm/vec3.hpp>
#include <glm/common.hpp>

#include "util/math/math.h"

namespace eng {

	namespace direction {

		enum class Direction {
			DOWN = 0, UP = 1, NORTH = 2, SOUTH = 3, WEST = 4, EAST = 5, UNDEFINED = 6,
		};

		constexpr std::array<Direction, 7> direction_values {
			Direction::DOWN, Direction::UP, Direction::NORTH, Direction::SOUTH, Direction::WEST, Direction::EAST, Direction::UNDEFINED,
		};
		constexpr std::array<Direction, 6> directions {
			Direction::DOWN, Direction::UP, Direction::NORTH, Direction::SOUTH, Direction::WEST, Direction::EAST,
		};
		constexpr std::array<Direction, 4> cardinal_directions {
			Direction::SOUTH, Direction::WEST, Direction::NORTH, Direction::EAST,
		};

		constexpr size_t getIndex(Direction d) {
			switch (d) {
				case Direction::DOWN:
				case Direction::UP:
				case Direction::NORTH:
				case Direction::SOUTH:
				case Direction::WEST:
				case Direction::EAST:
					return static_cast<size_t>(d);
				default:
					return static_cast<size_t>(Direction::UNDEFINED);
			}
		}

		constexpr Direction fromIndex(size_t i) {
			if (i < direction_values.size()) {
				return direction_values[i];
			}
			return Direction::UNDEFINED;
		}

		namespace axis {

			enum class Axis {
				X = 0, Y = 1, Z = 2, UNDEFINED = 3,
			};

			constexpr std::array<Axis, 4> axis_values {
				Axis::X, Axis::Y, Axis::Z, Axis::UNDEFINED,
			};
			constexpr std::array<Axis, 3> axes {
				Axis::X, Axis::Y, Axis::Z,
			};

			constexpr size_t getIndex(Axis a) {
				switch (a) {
					case Axis::X: return static_cast<size_t>(a);
					case Axis::Y: return static_cast<size_t>(a);
					case Axis::Z: return static_cast<size_t>(a);
					default: return static_cast<size_t>(Axis::UNDEFINED);
				}
			}

			constexpr Axis fromIndex(size_t i) {
				if (i < axis_values.size()) {
					return axis_values[i];
				}
				return Axis::UNDEFINED;
			}

			constexpr Axis getAxis(Direction d) {
				switch (d) {
				case Direction::DOWN:
				case Direction::UP:
					return Axis::Y;
				case Direction::NORTH:
				case Direction::SOUTH:
					return Axis::Z;
				case Direction::WEST:
				case Direction::EAST:
					return Axis::X;
				default:
					return Axis::UNDEFINED;
				}
			}

		}
		using namespace axis;

		namespace axis_direction {

			enum class AxisDirection {
				NEGATIVE = -1, POSITIVE = 1, UNDEFINED = 0,
			};

			constexpr std::array<AxisDirection, 3> axis_direction_values {
				AxisDirection::NEGATIVE, AxisDirection::POSITIVE, AxisDirection::UNDEFINED,
			};
			constexpr std::array<AxisDirection, 2> axis_directions {
				AxisDirection::NEGATIVE, AxisDirection::POSITIVE,
			};

			constexpr size_t getIndex(AxisDirection ad) {
				switch (ad) {
					case AxisDirection::NEGATIVE: return 0;
					case AxisDirection::POSITIVE: return 1;
					default: return 2;
				}
			}

			constexpr AxisDirection fromIndex(size_t i) {
				if (i < axis_direction_values.size()) {
					return axis_direction_values[i];
				}
				return AxisDirection::UNDEFINED;
			}

			constexpr AxisDirection getAxisDirection(Direction d) {
				switch (d) {
				case Direction::DOWN:
				case Direction::NORTH:
				case Direction::WEST:
					return AxisDirection::NEGATIVE;
				case Direction::SOUTH:
				case Direction::UP:
				case Direction::EAST:
					return AxisDirection::POSITIVE;
				default:
					return AxisDirection::UNDEFINED;
				}
			}

			constexpr AxisDirection getOpposite(AxisDirection ad) {
				switch (ad) {
					case AxisDirection::NEGATIVE: return AxisDirection::POSITIVE;
					case AxisDirection::POSITIVE: return AxisDirection::NEGATIVE;
					default: return AxisDirection::UNDEFINED;
				}
			}

			constexpr int getSign(AxisDirection ad) {
				return static_cast<int>(ad);
			}

		}
		using namespace axis_direction;

		namespace rotation_direction {

			enum class RotationDirection {
				COUNTER_CLOCKWISE = -1, CLOCKWISE = 1, C_CW = COUNTER_CLOCKWISE, CW = CLOCKWISE, UNDEFINED = 0,
			};

			constexpr std::array<RotationDirection, 3> rotation_direction_values {
				RotationDirection::COUNTER_CLOCKWISE, RotationDirection::CLOCKWISE, RotationDirection::UNDEFINED,
			};
			
			constexpr std::array<RotationDirection, 2> rotation_directions {
				RotationDirection::COUNTER_CLOCKWISE, RotationDirection::CLOCKWISE,
			};
			
			constexpr size_t getIndex(RotationDirection rd) {
				switch (rd) {
					case RotationDirection::COUNTER_CLOCKWISE: return 0;
					case RotationDirection::CLOCKWISE: return 1;
					default: return 2;
				}
			}

			constexpr RotationDirection fromIndex(size_t i) {
				if (i < rotation_direction_values.size()) {
					return rotation_direction_values[i];
				}
				return RotationDirection::UNDEFINED;
			}

			constexpr RotationDirection getOpposite(RotationDirection rd) {
				switch (rd) {
					case RotationDirection::COUNTER_CLOCKWISE: return RotationDirection::CLOCKWISE;
					case RotationDirection::CLOCKWISE: return RotationDirection::COUNTER_CLOCKWISE;
					default: return RotationDirection::UNDEFINED;
				}
			}

			constexpr int getSign(RotationDirection rd) {
				const int s = static_cast<int>(rd);
				return (s < 0) ? -1 : ((s > 0) ? 1 : 0);
			}

		}
		using namespace rotation_direction;

		constexpr Direction getDirection(axis::Axis axis, axis_direction::AxisDirection ad) {
			switch (axis) {
			case Axis::X:
				if (ad == AxisDirection::NEGATIVE) return Direction::WEST;
				if (ad == AxisDirection::POSITIVE) return Direction::EAST;
				return Direction::UNDEFINED;
			case Axis::Y:
				if (ad == AxisDirection::NEGATIVE) return Direction::DOWN;
				if (ad == AxisDirection::POSITIVE) return Direction::UP;
				return Direction::UNDEFINED;
			case Axis::Z:
				if (ad == AxisDirection::NEGATIVE) return Direction::NORTH;
				if (ad == AxisDirection::POSITIVE) return Direction::SOUTH;
				return Direction::UNDEFINED;
			default:
				return Direction::UNDEFINED;
			}
		}

		template<typename Vec, std::enable_if_t<std::is_same_v<Vec, glm::vec3> || std::is_same_v<Vec, glm::ivec3> || std::is_same_v<Vec, glm::dvec3>>* = nullptr>
		constexpr Direction getDirection(const Vec& vec) {
			const typename Vec::value_type yAbs = glm::abs(vec.y);
			const typename Vec::value_type zAbs = glm::abs(vec.z);
			const typename Vec::value_type xAbs = glm::abs(vec.x);

			if (yAbs >= xAbs && yAbs >= zAbs) {
				if (vec.y < 0) return Direction::DOWN;
				if (vec.y > 0) return Direction::UP;
				return Direction::UNDEFINED;
			}
			if (zAbs >= xAbs && zAbs >= yAbs) {
				if (vec.z < 0) return Direction::NORTH;
				if (vec.z > 0) return Direction::SOUTH;
				return Direction::UNDEFINED;
			}
			if (xAbs >= yAbs && xAbs >= zAbs) {
				if (vec.x < 0) return Direction::WEST;
				if (vec.x > 0) return Direction::EAST;
				return Direction::UNDEFINED;
			}
			return Direction::UNDEFINED;
		}

		template<typename Vec, std::enable_if_t<std::is_same_v<Vec, glm::vec3> || std::is_same_v<Vec, glm::ivec3> || std::is_same_v<Vec, glm::dvec3>>* = nullptr>
		constexpr Vec toVector(const Axis a, const AxisDirection ad = AxisDirection::POSITIVE) {
			switch (a) {
			case Axis::X: return { getSign(ad), 0, 0 };
			case Axis::Y: return { 0, getSign(ad), 0 };
			case Axis::Z: return { 0, 0, getSign(ad) };
			default: return { 0, 0, 0 };
			}
		}
		template<typename Vec, std::enable_if_t<std::is_same_v<Vec, glm::vec3> || std::is_same_v<Vec, glm::ivec3> || std::is_same_v<Vec, glm::dvec3>>* = nullptr>
		inline constexpr Vec toVector(const Direction d) {
			return toVector<Vec>(getAxis(d), getAxisDirection(d));
		}

		template<typename Vec, std::enable_if_t<std::is_same_v<Vec, glm::vec3> || std::is_same_v<Vec, glm::ivec3> || std::is_same_v<Vec, glm::dvec3>>* = nullptr>
		inline constexpr Vec offsetVector(const Vec& v, const Direction dir, const int offset) {
			return v + (toVector<Vec>(dir) * offset);
		}
		template<typename Vec, std::enable_if_t<std::is_same_v<Vec, glm::vec3> || std::is_same_v<Vec, glm::ivec3> || std::is_same_v<Vec, glm::dvec3>>* = nullptr>
		inline constexpr Vec offsetVector(const Vec& v, const Direction dir) {
			return v + toVector<Vec>(dir);
		}

		// returns closest Direction to an angle (degrees) on the xz-plane
		constexpr Direction fromHorizontalAngle(double degrees) {
			return cardinal_directions[static_cast<int>(eng::floor((degrees / 90.0) + 0.5)) & 3];
		}
		constexpr Direction fromHorizontalAngle(float degrees) {
			return cardinal_directions[static_cast<int>(eng::floor((degrees / 90.0f) + 0.5f)) & 3];
		}

		// returns the angle (degrees) of the Direction on the xz-plane
		constexpr double getHorizontalAngle(Direction d) {
			switch (d) {
				case Direction::SOUTH: return 0;
				case Direction::WEST: return 90;
				case Direction::NORTH: return 180;
				case Direction::EAST: return 270;
				default: return 0;
			}
		}

		constexpr Direction getOpposite(const Direction d) {
			switch (d) {
			case Direction::DOWN: return Direction::UP;
			case Direction::UP: return Direction::DOWN;
			case Direction::NORTH: return Direction::SOUTH;
			case Direction::SOUTH: return Direction::NORTH;
			case Direction::WEST: return Direction::EAST;
			case Direction::EAST: return Direction::WEST;
			default: return Direction::UNDEFINED;
			}
		}

		constexpr Direction rotateAround(const Direction dir, const Axis axis, const RotationDirection rotationDir = RotationDirection::CW, const int rotations = 1) {
			if (dir == Direction::UNDEFINED || axis == Axis::UNDEFINED || rotationDir == RotationDirection::UNDEFINED)
				return Direction::UNDEFINED;
			if (axis::getAxis(dir) == axis) return dir;

			const int r = (rotations * rotation_direction::getSign(rotationDir)) & 3;
			if (r == 0) return dir;
			if (r == 2) return getOpposite(dir);

			switch (axis) {
			case Axis::Y:
				switch (dir) {
				case Direction::SOUTH: return (r == 1 ? Direction::EAST : Direction::WEST);
				case Direction::WEST: return (r == 1 ? Direction::SOUTH : Direction::NORTH);
				case Direction::NORTH: return (r == 1 ? Direction::WEST : Direction::EAST);
				case Direction::EAST: return (r == 1 ? Direction::NORTH : Direction::SOUTH);
				default: return dir;
				}
			case Axis::X:
				switch (dir) {
				case Direction::DOWN: return (r == 1 ? Direction::NORTH : Direction::SOUTH);
				case Direction::SOUTH: return (r == 1 ? Direction::DOWN : Direction::UP);
				case Direction::UP: return (r == 1 ? Direction::SOUTH : Direction::NORTH);
				case Direction::NORTH: return (r == 1 ? Direction::UP : Direction::DOWN);
				default: return dir;
				}
			case Axis::Z:
				switch (dir) {
				case Direction::DOWN: return (r == 1 ? Direction::EAST : Direction::WEST);
				case Direction::WEST: return (r == 1 ? Direction::DOWN : Direction::UP);
				case Direction::UP: return (r == 1 ? Direction::WEST : Direction::EAST);
				case Direction::EAST: return (r == 1 ? Direction::UP : Direction::DOWN);
				default: return dir;
				}
			default:
				return dir;
			}
		}

		constexpr Axis rotateAround(const Axis axis, const Axis rotationAxis, const RotationDirection rotationDir = RotationDirection::CW, const int rotations = 1) {
			if (axis == Axis::UNDEFINED || rotationAxis == Axis::UNDEFINED || rotationDir == RotationDirection::UNDEFINED)
				return Axis::UNDEFINED;
			if (axis == rotationAxis) return axis;

			const int r = (rotations * rotation_direction::getSign(rotationDir)) & 3;
			if ((r % 2) == 0) return axis;

			switch (rotationAxis) {
			case Axis::Y:
				switch (axis) {
				case Axis::X: return Axis::Z;
				case Axis::Z: return Axis::X;
				default: return axis;
				}
			case Axis::X:
				switch (axis) {
				case Axis::Y: return Axis::Z;
				case Axis::Z: return Axis::Y;
				default: return axis;
				}
			case Axis::Z:
				switch (axis) {
				case Axis::X: return Axis::Y;
				case Axis::Y: return Axis::X;
				default: return axis;
				}
			default:
				return axis;
			}
		}

		
		// TODO: implement conversion from string to Direction, Axis, AxisDirection, and RotationDirection

	}

	namespace axis = direction::axis;
	namespace axis_direction = direction::axis_direction;
	namespace rotation_direction = direction::rotation_direction;

	using Direction = direction::Direction;
	using Axis = axis::Axis;
	using AxisDirection = axis_direction::AxisDirection;
	using RotationDirection = rotation_direction::RotationDirection;

	
	std::string to_string(const Direction d);
	std::ostream& operator <<(std::ostream& stream, const Direction d);
	std::string to_string(const Axis a);
	std::ostream& operator <<(std::ostream& stream, const Axis a);
	std::string to_string(const AxisDirection ad);
	std::ostream& operator <<(std::ostream& stream, const AxisDirection ad);
	std::string to_string(const RotationDirection rd);
	std::ostream& operator <<(std::ostream& stream, const RotationDirection rd);

}