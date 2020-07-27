#include "BlockGrass.h"

#include "util/math/NoiseGen.h"

namespace eng {

	static NoiseGen colorNoise1 { RNG::randomSeed(), { 43.0, 1.0, 1.0, 1 } };
	static NoiseGen colorNoise2 { RNG::randomSeed(), { 33.0, 1.0, 1.0, 1 } };
	static NoiseGen colorNoise3 { RNG::randomSeed(), { 23.0, 1.0, 1.0, 1 } };

	Color getGrassColor(const glm::ivec3& blockPos) {
		return Color::hsl {
			static_cast<float>(colorNoise1.getNoise(blockPos)) * 360.0f,
			glm::mix(0.2f, 0.65f, static_cast<float>(colorNoise2.getNoise(blockPos))),
			glm::mix(0.4f, 0.8f, static_cast<float>(colorNoise3.getNoise(blockPos))),
		};
	}


	/*static BlockQuad createGrassSideHangQuad(size_t turns, size_t texIndex) {
		auto nQuad = BlockModel::createSimpleTexturedQuad({ 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, texIndex);
		constexpr glm::vec3 tOrig { 0.5f, 1.0f, 0.0f };
		nQuad.transform(glm::translate(glm::rotate(
			glm::translate(glm::mat4(1.0f), tOrig),
			glm::radians(15.0f),
			direction::toVector<glm::vec3>(direction::getDirection(Axis::X, AxisDirection::POSITIVE))
		), -tOrig));
		constexpr glm::vec3 rOrig { 0.5f, 0.5f, 0.5f };
		nQuad.transform(glm::translate(glm::rotate(
			glm::translate(glm::mat4(1.0f), rOrig),
			glm::radians((turns % 4) * 90.0f),
			direction::toVector<glm::vec3>(direction::getDirection(Axis::Y, AxisDirection::POSITIVE))
		), -rOrig));
		return nQuad;
	}

	GrassBlockModel::GrassBlockModel(size_t texIndexD, size_t texIndexU, size_t texIndexNSWE) : quads {
				createSimpleTexturedQuad({ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, texIndexD), // bottom
				createSimpleTexturedQuad({ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, texIndexU), // top
				createSimpleTexturedQuad({ 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, texIndexNSWE), // north
				createSimpleTexturedQuad({ 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, texIndexNSWE), // south
				createSimpleTexturedQuad({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, texIndexNSWE), // west
				createSimpleTexturedQuad({ 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, texIndexNSWE), // east
				createSimpleTexturedQuad({ 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 21), // north overlay
				createSimpleTexturedQuad({ 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, 21), // south overlay
				createSimpleTexturedQuad({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, 21), // west overlay
				createSimpleTexturedQuad({ 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 21), // east overlay
				createGrassSideHangQuad(0, 22), // north hanging side
				createGrassSideHangQuad(2, 22), // south hanging side
				createGrassSideHangQuad(1, 22), // west hanging side
				createGrassSideHangQuad(3, 22), // east hanging side
	} {}

	void GrassBlockModel::addFaceQuadsToBuffer(const BlockState& blockState, const glm::ivec3& blockPos, const Direction face, const RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const {
		if (face != Direction::UNDEFINED) {
			if ((layer == render_layer::Opaque)) {
				if (face == Direction::UP) {
					const auto color = blockState.getBlock().getBlockColor(worldView, blockState, blockPos, 0);
					buffer.push_back((quads[direction::getIndex(face)] + offset) * color);
				} else {
					buffer.push_back(quads[direction::getIndex(face)] + offset);
				}
			} else if ((layer == render_layer::Cutout) && (direction::getAxis(face) != Axis::Y)) {
				const auto color = blockState.getBlock().getBlockColor(worldView, blockState, blockPos, 0);
				buffer.push_back((quads[direction::getIndex(face) + 6 - 2] + offset) * color);
				//buffer.push_back((quads[direction::getIndex(face) + 10 - 2] + offset) * color);
			}
		}
	}
	*/

}