#pragma once

#include <array>

#include <glm/trigonometric.hpp>

#include "blocks/Block.h"

namespace eng {
	/*
	class SimpleCubeModel {
	protected:
		const std::array<BlockQuad, 6> faceQuads;
		const RenderLayer layer;
		const bool colorize;

	public:

		SimpleCubeModel(const BlockQuad& q0, const BlockQuad& q1, const BlockQuad& q2, const BlockQuad& q3, const BlockQuad& q4, const BlockQuad& q5, RenderLayer layer = RenderLayer::Opaque, bool colorize = false) noexcept :
				faceQuads({ q0, q1, q2, q3, q4, q5 }), layer(layer), colorize(colorize) {}

		SimpleCubeModel(size_t texIndexD, size_t texIndexU, size_t texIndexN, size_t texIndexS, size_t texIndexW, size_t texIndexE, RenderLayer layer = RenderLayer::Opaque, bool colorize = false) noexcept :
				SimpleCubeModel(
					createSimpleTexturedQuad({ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, texIndexD), // bottom
					createSimpleTexturedQuad({ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, texIndexU), // top
					createSimpleTexturedQuad({ 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, texIndexN), // north
					createSimpleTexturedQuad({ 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, texIndexS), // south
					createSimpleTexturedQuad({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, texIndexW), // west
					createSimpleTexturedQuad({ 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, texIndexE), // east
					layer
				) {}

		SimpleCubeModel(size_t texIndexD, size_t texIndexU, size_t texIndexNSWE, RenderLayer layer = RenderLayer::Opaque, bool colorize = false) noexcept :
				SimpleCubeModel(texIndexD, texIndexU, texIndexNSWE, texIndexNSWE, texIndexNSWE, texIndexNSWE, layer, colorize) {}

		SimpleCubeModel(size_t texIndexDU, size_t texIndexNSWE, RenderLayer layer = RenderLayer::Opaque, bool colorize = false) noexcept :
				SimpleCubeModel(texIndexDU, texIndexDU, texIndexNSWE, texIndexNSWE, texIndexNSWE, texIndexNSWE, layer, colorize) {}

		SimpleCubeModel(size_t texIndex, RenderLayer layer = RenderLayer::Opaque, bool colorize = false) noexcept :
				SimpleCubeModel(texIndex, texIndex, texIndex, texIndex, texIndex, texIndex, layer, colorize) {}

		SimpleCubeModel getRotated(const Axis axis, const int rotations) const {
			constexpr glm::vec3 origin { 0.5f, 0.5f, 0.5f };
			const glm::mat4 matrix = glm::translate(glm::rotate(
				glm::translate(glm::mat4(1.0f), origin),
				glm::radians((rotations & 3) * 90.0f),
				direction::toVector<glm::vec3>(direction::getDirection(axis, AxisDirection::POSITIVE))
			), -origin);
			return {
				faceQuads[direction::getIndex(direction::rotateAround(direction::fromIndex(0), axis, RotationDirection::C_CW))].getTransformed(matrix),
				faceQuads[direction::getIndex(direction::rotateAround(direction::fromIndex(1), axis, RotationDirection::C_CW))].getTransformed(matrix),
				faceQuads[direction::getIndex(direction::rotateAround(direction::fromIndex(2), axis, RotationDirection::C_CW))].getTransformed(matrix),
				faceQuads[direction::getIndex(direction::rotateAround(direction::fromIndex(3), axis, RotationDirection::C_CW))].getTransformed(matrix),
				faceQuads[direction::getIndex(direction::rotateAround(direction::fromIndex(4), axis, RotationDirection::C_CW))].getTransformed(matrix),
				faceQuads[direction::getIndex(direction::rotateAround(direction::fromIndex(5), axis, RotationDirection::C_CW))].getTransformed(matrix),
				layer
			};
		}

	};
	*/
}