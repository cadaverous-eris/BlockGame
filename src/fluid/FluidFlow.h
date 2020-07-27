#pragma once

#include <cstdint>
#include <type_traits>
#include <array>
#include <string>
#include <iostream>

#include <glm/vec3.hpp>

#include "util/direction.h"

namespace eng {

	using FluidFlowId = uint8_t;

	class FluidFlow {
	private:
		uint8_t x : 2;
		uint8_t y : 2;
		uint8_t z : 2;

		constexpr FluidFlow(const uint8_t x, const uint8_t y, const uint8_t z) noexcept : x(x), y(y), z(z) {}

		static constexpr uint8_t axisDirToBits(const AxisDirection d) noexcept {
			switch (d) {
				case AxisDirection::NEGATIVE: return 0b01;
				case AxisDirection::POSITIVE: return 0b10;
				default: return 0b00;
			}
		}
		static constexpr uint8_t axisBitsOr(uint8_t a, uint8_t b) noexcept {
			a |= b;
			return !((a & 0b01) && (a & 0b10)) * a;
		}
		static constexpr uint8_t axisBitsXor(uint8_t a, uint8_t b) noexcept {
			a ^= b;
			return !((a & 0b01) && (a & 0b10)) * a;
		}
		inline static constexpr uint8_t axisBitsFlip(uint8_t a) noexcept {
			return static_cast<bool>((a & 0b01) || (a & 0b10)) * ~a;
		}
		static constexpr AxisDirection axisDirFromBits(uint8_t a) noexcept {
			const bool n = (a & 0b01);
			const bool p = (a & 0b10);
			if (n == p) return AxisDirection::UNDEFINED;
			return n ? AxisDirection::NEGATIVE : AxisDirection::POSITIVE;
		}

	public:

		constexpr FluidFlow() noexcept : x(0), y(0), z(0) {}

		constexpr FluidFlow(const AxisDirection x, const AxisDirection y, const AxisDirection z) noexcept :
				FluidFlow(axisDirToBits(x), axisDirToBits(y), axisDirToBits(z)) {}

		constexpr /*explicit*/ FluidFlow(const Direction dir) noexcept : FluidFlow(fromDirection(dir)) {}


		static constexpr FluidFlow fromDirection(const Direction dir) noexcept {
			switch (dir) {
				case Direction::DOWN: return FluidFlow::Down;
				case Direction::UP: return FluidFlow::Up;
				case Direction::NORTH: return FluidFlow::North;
				case Direction::SOUTH: return FluidFlow::South;
				case Direction::WEST: return FluidFlow::West;
				case Direction::EAST: return FluidFlow::East;
				default: return FluidFlow::Still;
			}
		}
		static constexpr FluidFlow fromAxisDirection(const Axis axis, const AxisDirection axisDir) noexcept {
			switch (axis) {
			case Axis::X:
				switch (axisDir) {
					case AxisDirection::NEGATIVE: return FluidFlow::West;
					case AxisDirection::POSITIVE: return FluidFlow::East;
					default: return FluidFlow::Still;
				}
				break;
			case Axis::Y:
				switch (axisDir) {
					case AxisDirection::NEGATIVE: return FluidFlow::Down;
					case AxisDirection::POSITIVE: return FluidFlow::Up;
					default: return FluidFlow::Still;
				}
				break;
			case Axis::Z:
				switch (axisDir) {
					case AxisDirection::NEGATIVE: return FluidFlow::North;
					case AxisDirection::POSITIVE: return FluidFlow::South;
					default: return FluidFlow::Still;
				}
				break;
			default:
				return FluidFlow::Still;
			}
		}

		FluidFlow& operator |=(const FluidFlow b) noexcept {
			x = axisBitsOr(x, b.x);
			y = axisBitsOr(y, b.y);
			z = axisBitsOr(z, b.z);
			return *this;
		}
		FluidFlow& operator &=(const FluidFlow b) noexcept {
			x &= b.x;
			y &= b.y;
			z &= b.z;
			return *this;
		}
		FluidFlow& operator ^=(const FluidFlow b) noexcept {
			x = axisBitsXor(x, b.x);
			y = axisBitsXor(y, b.y);
			z = axisBitsXor(z, b.z);
			return *this;
		}

		inline constexpr bool operator ==(const FluidFlow b) const noexcept {
			return (x == b.x) && (y == b.y) && (z == b.z);
		}
		inline constexpr bool operator !=(const FluidFlow b) const noexcept {
			return (x != b.x) || (y != b.y) || (z != b.z);
		}

		inline constexpr FluidFlow operator |(const FluidFlow b) const noexcept {
			return { axisBitsOr(x, b.x), axisBitsOr(y, b.y), axisBitsOr(z, b.z) };
		}
		inline constexpr FluidFlow operator &(const FluidFlow b) const noexcept {
			return { static_cast<uint8_t>(x & b.x), static_cast<uint8_t>(y & b.y), static_cast<uint8_t>(z & b.z) };
		}
		inline constexpr FluidFlow operator ^(const FluidFlow b) const noexcept {
			return { axisBitsXor(x, b.x), axisBitsXor(y, b.y), axisBitsXor(z, b.z) };
		}

		inline constexpr operator bool() const noexcept {
			return static_cast<bool>(x | y | z);
		}

		inline constexpr FluidFlow operator ~() const noexcept {
			return { axisBitsFlip(x), axisBitsFlip(y), axisBitsFlip(z) };
		}

		inline constexpr bool flowsInDirection(const Direction dir) const noexcept {
			return (dir != Direction::UNDEFINED) && (getAxisFlowDirection(axis::getAxis(dir)) == axis_direction::getAxisDirection(dir));
		}

		constexpr FluidFlow getAxisFluidFlow(const Axis axis) const noexcept {
			switch (axis) {
				case Axis::X: return *this ^ FluidFlow::East;
				case Axis::Y: return *this ^ FluidFlow::Up;
				case Axis::Z: return *this ^ FluidFlow::South;
				default: return FluidFlow::Still;
			}
		}

		constexpr AxisDirection getAxisFlowDirection(const Axis axis) const noexcept {
			switch (axis) {
				case Axis::X: return axisDirFromBits(x);
				case Axis::Y: return axisDirFromBits(y);
				case Axis::Z: return axisDirFromBits(z);
				default: return AxisDirection::UNDEFINED;
			}
		}

		inline constexpr const glm::ivec3& getFlowVec() const noexcept {
			return flowVectors[getId()];
		}
		inline constexpr const glm::vec3& getFlowVecNormalized() const noexcept {
			return flowNormals[getId()];
		}

		// Allows use in switch statements
		inline constexpr operator FluidFlowId() const noexcept { return getId(*this); }

		inline constexpr FluidFlowId getId() const noexcept { return getId(*this); }
		static constexpr FluidFlowId getId(const FluidFlow flow) noexcept;
		static constexpr FluidFlow fromId(const FluidFlowId id);

		static const FluidFlow Still;
		static const FluidFlow Down;
		static const FluidFlow Up;
		static const FluidFlow North;
		static const FluidFlow South;
		static const FluidFlow West;
		static const FluidFlow East;
		static const FluidFlow DownNorth;
		static const FluidFlow DownSouth;
		static const FluidFlow DownWest;
		static const FluidFlow DownEast;
		static const FluidFlow UpNorth;
		static const FluidFlow UpSouth;
		static const FluidFlow UpWest;
		static const FluidFlow UpEast;
		static const FluidFlow NorthWest;
		static const FluidFlow NorthEast;
		static const FluidFlow SouthWest;
		static const FluidFlow SouthEast;
		static const FluidFlow DownNorthWest;
		static const FluidFlow DownNorthEast;
		static const FluidFlow DownSouthWest;
		static const FluidFlow DownSouthEast;
		static const FluidFlow UpNorthWest;
		static const FluidFlow UpNorthEast;
		static const FluidFlow UpSouthWest;
		static const FluidFlow UpSouthEast;

		static const std::array<FluidFlow, 27> values;

	private:
		static const std::array<glm::ivec3, 27> flowVectors;
		static const std::array<glm::vec3, 27> flowNormals;
	};

	std::string to_string(const FluidFlow flow);
	std::ostream& operator <<(std::ostream& stream, const FluidFlow flow);


	inline constexpr const FluidFlow FluidFlow::Still { AxisDirection::UNDEFINED, AxisDirection::UNDEFINED, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::Down { AxisDirection::UNDEFINED, AxisDirection::NEGATIVE, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::Up { AxisDirection::UNDEFINED, AxisDirection::POSITIVE, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::North { AxisDirection::UNDEFINED, AxisDirection::UNDEFINED, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::South { AxisDirection::UNDEFINED, AxisDirection::UNDEFINED, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::West { AxisDirection::NEGATIVE, AxisDirection::UNDEFINED, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::East { AxisDirection::POSITIVE, AxisDirection::UNDEFINED, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::DownNorth { AxisDirection::UNDEFINED, AxisDirection::NEGATIVE, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::DownSouth { AxisDirection::UNDEFINED, AxisDirection::NEGATIVE, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::DownWest { AxisDirection::NEGATIVE, AxisDirection::NEGATIVE, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::DownEast { AxisDirection::POSITIVE, AxisDirection::NEGATIVE, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::UpNorth { AxisDirection::UNDEFINED, AxisDirection::POSITIVE, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::UpSouth { AxisDirection::UNDEFINED, AxisDirection::POSITIVE, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::UpWest { AxisDirection::NEGATIVE, AxisDirection::POSITIVE, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::UpEast { AxisDirection::POSITIVE, AxisDirection::POSITIVE, AxisDirection::UNDEFINED };
	inline constexpr const FluidFlow FluidFlow::NorthWest { AxisDirection::NEGATIVE, AxisDirection::UNDEFINED, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::NorthEast { AxisDirection::POSITIVE, AxisDirection::UNDEFINED, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::SouthWest { AxisDirection::NEGATIVE, AxisDirection::UNDEFINED, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::SouthEast { AxisDirection::POSITIVE, AxisDirection::UNDEFINED, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::DownNorthWest { AxisDirection::NEGATIVE, AxisDirection::NEGATIVE, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::DownNorthEast { AxisDirection::POSITIVE, AxisDirection::NEGATIVE, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::DownSouthWest { AxisDirection::NEGATIVE, AxisDirection::NEGATIVE, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::DownSouthEast { AxisDirection::POSITIVE, AxisDirection::NEGATIVE, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::UpNorthWest { AxisDirection::NEGATIVE, AxisDirection::POSITIVE, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::UpNorthEast { AxisDirection::POSITIVE, AxisDirection::POSITIVE, AxisDirection::NEGATIVE };
	inline constexpr const FluidFlow FluidFlow::UpSouthWest { AxisDirection::NEGATIVE, AxisDirection::POSITIVE, AxisDirection::POSITIVE };
	inline constexpr const FluidFlow FluidFlow::UpSouthEast { AxisDirection::POSITIVE, AxisDirection::POSITIVE, AxisDirection::POSITIVE };

	inline constexpr const std::array<FluidFlow, 27> FluidFlow::values {
		Still,
		Down,
		Up,
		North,
		South,
		West,
		East,
		DownNorth,
		DownSouth,
		DownWest,
		DownEast,
		UpNorth,
		UpSouth,
		UpWest,
		UpEast,
		NorthWest,
		NorthEast,
		SouthWest,
		SouthEast,
		DownNorthWest,
		DownNorthEast,
		DownSouthWest,
		DownSouthEast,
		UpNorthWest,
		UpNorthEast,
		UpSouthWest,
		UpSouthEast,
	};

	constexpr FluidFlowId FluidFlow::getId(const FluidFlow flow) noexcept {
		if (flow == FluidFlow::Down) return 1;
		else if (flow == FluidFlow::Up) return 2;
		else if (flow == FluidFlow::North) return 3;
		else if (flow == FluidFlow::South) return 4;
		else if (flow == FluidFlow::West) return 5;
		else if (flow == FluidFlow::East) return 6;
		else if (flow == FluidFlow::DownNorth) return 7;
		else if (flow == FluidFlow::DownSouth) return 8;
		else if (flow == FluidFlow::DownWest) return 9;
		else if (flow == FluidFlow::DownEast) return 10;
		else if (flow == FluidFlow::UpNorth) return 11;
		else if (flow == FluidFlow::UpSouth) return 12;
		else if (flow == FluidFlow::UpWest) return 13;
		else if (flow == FluidFlow::UpEast) return 14;
		else if (flow == FluidFlow::NorthWest) return 15;
		else if (flow == FluidFlow::NorthEast) return 16;
		else if (flow == FluidFlow::SouthWest) return 17;
		else if (flow == FluidFlow::SouthEast) return 18;
		else if (flow == FluidFlow::DownNorthWest) return 19;
		else if (flow == FluidFlow::DownNorthEast) return 20;
		else if (flow == FluidFlow::DownSouthWest) return 21;
		else if (flow == FluidFlow::DownSouthEast) return 22;
		else if (flow == FluidFlow::UpNorthWest) return 23;
		else if (flow == FluidFlow::UpNorthEast) return 24;
		else if (flow == FluidFlow::UpSouthWest) return 25;
		else if (flow == FluidFlow::UpSouthEast) return 26;
		return 0;
	}

	inline constexpr FluidFlow FluidFlow::fromId(const FluidFlowId id) {
		return FluidFlow::values.at(id);
	}

	inline constexpr const std::array<glm::ivec3, 27> FluidFlow::flowVectors {
		glm::vec3( 0,  0,  0), // Still
		glm::vec3( 0, -1,  0), // Down
		glm::vec3( 0,  1,  0), // Up
		glm::vec3( 0,  0, -1), // North
		glm::vec3( 0,  0,  1), // South
		glm::vec3(-1,  0,  0), // West
		glm::vec3( 1,  0,  0), // East
		glm::vec3( 0, -1, -1), // DownNorth
		glm::vec3( 0, -1,  1), // DownSouth
		glm::vec3(-1, -1,  0), // DownWest
		glm::vec3( 1, -1,  0), // DownEast
		glm::vec3( 0,  1, -1), // UpNorth
		glm::vec3( 0,  1,  1), // UpSouth
		glm::vec3(-1,  1,  0), // UpWest
		glm::vec3( 1,  1,  0), // UpEast
		glm::vec3(-1,  0, -1), // NorthWest
		glm::vec3( 1,  0, -1), // NorthEast
		glm::vec3(-1,  0,  1), // SouthWest
		glm::vec3( 1,  0,  1), // SouthEast
		glm::vec3(-1, -1, -1), // DownNorthWest
		glm::vec3( 1, -1, -1), // DownNorthEast
		glm::vec3(-1, -1,  1), // DownSouthWest
		glm::vec3( 1, -1,  1), // DownSouthEast
		glm::vec3(-1,  1, -1), // UpNorthWest
		glm::vec3( 1,  1, -1), // UpNorthEast
		glm::vec3(-1,  1,  1), // UpSouthWest
		glm::vec3( 1,  1,  1), // UpSouthEast
	};
	//sqrt(1 / 3) = 0.57735026918962576450914878050196
	//sqrt(1 / 2) = 0.70710678118654752440084436210485
	inline constexpr const std::array<glm::vec3, 27> FluidFlow::flowNormals {
		glm::vec3(0,  0,  0), // Still
		glm::vec3(0, -1,  0), // Down
		glm::vec3(0,  1,  0), // Up
		glm::vec3(0,  0, -1), // North
		glm::vec3(0,  0,  1), // South
		glm::vec3(-1,  0,  0), // West
		glm::vec3(1,  0,  0), // East
		glm::vec3(0, -0.70711, -0.70711), // DownNorth
		glm::vec3(0, -0.70711,  0.70711), // DownSouth
		glm::vec3(-0.70711, -0.70711,  0), // DownWest
		glm::vec3(0.70711, -0.70711,  0), // DownEast
		glm::vec3(0,  0.70711, -0.70711), // UpNorth
		glm::vec3(0,  0.70711,  0.70711), // UpSouth
		glm::vec3(-0.70711,  0.70711,  0), // UpWest
		glm::vec3(0.70711,  0.70711,  0), // UpEast
		glm::vec3(-0.70711,  0, -0.70711), // NorthWest
		glm::vec3(0.70711,  0, -0.70711), // NorthEast
		glm::vec3(-0.70711,  0,  0.70711), // SouthWest
		glm::vec3(0.70711,  0,  0.70711), // SouthEast
		glm::vec3(-0.57735, -0.57735, -0.57735), // DownNorthWest
		glm::vec3(0.57735, -0.57735, -0.57735), // DownNorthEast
		glm::vec3(-0.57735, -0.57735,  0.57735), // DownSouthWest
		glm::vec3(0.57735, -0.57735,  0.57735), // DownSouthEast
		glm::vec3(-0.57735,  0.57735, -0.57735), // UpNorthWest
		glm::vec3(0.57735,  0.57735, -0.57735), // UpNorthEast
		glm::vec3(-0.57735,  0.57735,  0.57735), // UpSouthWest
		glm::vec3(0.57735,  0.57735,  0.57735), // UpSouthEast
	};


	/*enum class FluidFlow : uint8_t {
		// no fluid flow
		Still = 0,
		// towards block faces
		Down, Up, North, South, West, East,
		// towards block edges
		DownNorth, DownSouth, DownWest, DownEast,
		// towards block edges
		UpNorth, UpSouth, UpWest, UpEast,
		// towards block edges
		NorthWest, NorthEast, SouthWest, SouthEast,
		// towards block corners
		DownNorthWest, DownNorthEast, DownSouthWest, DownSouthEast,
		// towards block corners
		UpNorthWest, UpNorthEast, UpSouthWest, UpSouthEast,
	};
	using FluidFlowId = std::underlying_type_t<FluidFlow>;*/

}