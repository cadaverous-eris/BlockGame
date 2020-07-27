#include "BlockModel.h"

#include <iostream>

#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/normal.hpp>

#include "BakedBlockModel.h"
#include "util/resources/TextureAtlas.h"
#include "util/jonk/Jonk.h"
#include "block/BlockRegistry.h"
#include "block/Block.h"

namespace eng {

	inline static constexpr bool isTextureVariable(const std::string_view textureName) noexcept {
		return textureName.starts_with(BlockModel::texture_variable_prefix);
	}
	inline static std::string getTextureVariableKey(const std::string& textureVariable) {
		if (!isTextureVariable(textureVariable)) return textureVariable;
		return textureVariable.substr(1);
	}

	const std::string BlockModel::missing_model_name = "missing";
	const BlockModel BlockModel::missing_model = []() -> BlockModel {
		constexpr auto makeQuadDef = [](const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const Direction face) -> QuadDefinition {
			return QuadDefinition {
				TextureAtlas::missing_texture_name, std::array<BlockVert, 4> {
					BlockVert(p0, glm::vec2(0, 0)), BlockVert(p1, glm::vec2(0, 1)),
					BlockVert(p2, glm::vec2(1, 0)), BlockVert(p3, glm::vec2(1, 1)),
				}, Color(0xFF, 0xFF, 0xFF), std::nullopt, face, RenderLayer::Opaque, false, false,
			};
		};
		return BlockModel(missing_model_name, std::nullopt, {}, std::vector<QuadDefinition>({
			makeQuadDef({ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, Direction::DOWN),
			makeQuadDef({ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, Direction::UP),
			makeQuadDef({ 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, Direction::NORTH),
			makeQuadDef({ 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, Direction::SOUTH),
			makeQuadDef({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, Direction::WEST),
			makeQuadDef({ 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, Direction::EAST),
		}));
	}();

	BlockQuad createTexturedBlockQuad(BlockVert v0, BlockVert v1, BlockVert v2, BlockVert v3, const AtlasSprite& texture, const Color& color) {
		const auto minUV = texture.getMinUV();
		const auto maxUV = texture.getMaxUV();
		v0.texCoord = glm::mix(minUV, maxUV, v0.texCoord);
		v1.texCoord = glm::mix(minUV, maxUV, v1.texCoord);
		v2.texCoord = glm::mix(minUV, maxUV, v2.texCoord);
		v3.texCoord = glm::mix(minUV, maxUV, v3.texCoord);
		return BlockQuad(v0, v1, v2, v3, color);
	}

	BlockQuad createSimpleTexturedBlockQuad(const glm::vec3& pos0, const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3, const AtlasSprite& texture, const Color& color) noexcept {
		const auto minUV = texture.getMinUV();
		const auto maxUV = texture.getMaxUV();
		return BlockQuad(
			{ pos0, minUV },
			{ pos1, glm::vec2(minUV.x, maxUV.y) },
			{ pos2, glm::vec2(maxUV.x, minUV.y) },
			{ pos3, maxUV },
			color
		);
	}

	BlockModel::BlockModel(const std::string& name, const std::optional<std::string>& parent, const std::map<std::string, std::string>& textureVariables, const std::vector<QuadDefinition>& quadDefs) :
		name(name), parent(parent), textureVariables(textureVariables), textureLocations(), quadDefs(quadDefs), inheritQuads(false) {}
	BlockModel::BlockModel(const std::string& name, const std::string& parent, const std::map<std::string, std::string>& textureVariables) :
		name(name), parent(parent), textureVariables(textureVariables), textureLocations(), quadDefs(), inheritQuads(true) {}


	void BlockModel::resolveInheritedProperties(const std::unordered_map<std::string, BlockModel>& models) {
		if (parent) {
			const bool parentIsMissingModel = (*parent == missing_model_name);
			const auto parentIt = models.find(*parent);
			if ((parentIt == models.end()) || parentIsMissingModel) {
				if (!parentIsMissingModel)
					std::cerr << "Model Error: could not find parent model `" << *parent << "` [in model `" << name << "`]\n";
				*this = missing_model;
				return;
			}
			const BlockModel& parentModel = parentIt->second;
			// merge texture variable maps
			for (const auto& [k, v] : parentModel.textureVariables)
				textureVariables.insert(std::make_pair(k, v));
			// inherit parent quad list
			if (inheritQuads)
				quadDefs = parentModel.quadDefs;
		}
	}

	void BlockModel::resolveTextureLocations() {
		textureLocations.clear();
		for (auto& quadDef : quadDefs) {
			bool missingDefinition = false;
			while (isTextureVariable(quadDef.texture) && !missingDefinition) {
				const auto it = textureVariables.find(getTextureVariableKey(quadDef.texture));
				if (it == textureVariables.end()) {
					//std::cerr << "Could not resolve texture variable `" << quadDef.texture << "` in model `" << name << "`\n";
					missingDefinition = true;
				} else {
					quadDef.texture = it->second;
				}
			}
			if (!missingDefinition) { // add the quad's texture location to this models set of used texture locations
				textureLocations.insert(quadDef.texture);
			}
		}
	}

	BakedBlockModel BlockModel::bake(const TextureAtlas& textureAtlas) const {
		BakedBlockModel::Builder builder {};
		for (const auto& quadDef : quadDefs) {
			if (!textureAtlas.hasTexture(quadDef.texture))
				std::cerr << "Missing texture `" << quadDef.texture << "` in model `" << name << "`\n";
			if (!quadDef) continue; // skip invalid quads
			const auto& tex = textureAtlas.getSprite(quadDef.texture);
			const BlockQuad quad = createTexturedBlockQuad(quadDef.verts[0], quadDef.verts[1], quadDef.verts[2], quadDef.verts[3], tex, quadDef.color);
			if (quadDef.colorIndex) { // colored quad
				builder.addColoredQuad(quad, quadDef.cullFace, quadDef.renderLayer, *quadDef.colorIndex);
				if (quadDef.doubleSided)
					builder.addColoredQuad(quad.getBackQuad(), direction::getOpposite(quadDef.cullFace), quadDef.renderLayer, *quadDef.colorIndex);
			} else {
				builder.addQuad(quad, quadDef.cullFace, quadDef.renderLayer);
				if (quadDef.doubleSided)
					builder.addQuad(quad.getBackQuad(), direction::getOpposite(quadDef.cullFace), quadDef.renderLayer);
			}
		}
		return builder.build();
	}

	glm::vec3 BlockModel::QuadDefinition::getNormal1() const {
		return hasTri1() ? glm::triangleNormal(verts[0].pos, verts[1].pos, verts[2].pos) : glm::vec3(0, 0, 0);
	}
	glm::vec3 BlockModel::QuadDefinition::getNormal2() const {
		return hasTri2() ? glm::triangleNormal(verts[2].pos, verts[1].pos, verts[3].pos) : glm::vec3(0, 0, 0);
	}
	glm::vec3 BlockModel::QuadDefinition::getAvgNormal() const {
		return (*this) ? glm::normalize(getNormal1() + getNormal2()) : glm::vec3(0, 0, 0);
	}


	static int getUVLockRotationDirection(const BlockModel::QuadDefinition& quadDef, const Axis axis) {
		constexpr float threshold = 0.666f;
		switch (axis) {
		case Axis::Y:
		case Axis::X:
		case Axis::Z:
			if (quadDef) {
				const auto dot = glm::dot(direction::toVector<glm::vec3>(axis), quadDef.getAvgNormal());
				if (dot > threshold) return -1;
				if (dot < -threshold) return 1;
			}
		default:
			return 0;
		}
	}
	static int getUVLockSecondaryRotations(const BlockModel::QuadDefinition& quadDef, const Axis axis, int rotations) {
		constexpr float threshold = 0.666f;
		rotations &= 3;
		if (quadDef && ((axis == Axis::X) || (axis == Axis::Z))) {
			const auto avgNormal = quadDef.getAvgNormal();
			const auto dotY = glm::dot(direction::toVector<glm::vec3>(Axis::Y), avgNormal);
			const auto dotZ = glm::dot(direction::toVector<glm::vec3>(Axis::Z), avgNormal);
			const auto dotX = glm::dot(direction::toVector<glm::vec3>(Axis::X), avgNormal);
			if (axis == Axis::Z) {
				if (glm::abs(dotY) > threshold) {
					return ((rotations % 2) == 1) ? -rotations : rotations;
				} else if (glm::abs(dotX) > threshold) {
					return ((rotations % 2) == 1) ? -rotations : rotations;
				}
			} else if (axis == Axis::X) {
				if (glm::abs(dotY) > threshold) {
					if ((rotations % 2) == 1) {
						if ((dotY < -threshold) == (rotations == 1)) return 2;
					}
				} else if (glm::abs(dotZ) > threshold) {
					if ((rotations % 2) == 1) {
						return (dotZ < -threshold) ? 2 : 0;
					} else {
						return rotations;
					}
				}
			}
		}
		return 0;
	}
	static glm::vec2 rotateTexCoord(const glm::vec2& texCoord, int rotations) noexcept {
		rotations &= 3;
		if (rotations == 1)
			return { 1.0f - texCoord.y, texCoord.x };
		if (rotations == 2)
			return { 1.0f - texCoord.x, 1.0f - texCoord.y };
		if (rotations == 3)
			return { texCoord.y, 1.0f - texCoord.x };
		return texCoord;
	}

	BlockModel BlockModel::getRotated(const Axis axis, int rotations) const {
		rotations = rotations & 3;
		constexpr glm::vec3 origin { 0.5f, 0.5f, 0.5f };
		const glm::mat4 matrix = glm::translate(glm::rotate(
			glm::translate(glm::mat4(1.0f), origin),
			glm::radians(rotations * 90.0f),
			direction::toVector<glm::vec3>(axis)
		), -origin);
		BlockModel rotatedModel = *this;
		for (auto& quadDef : rotatedModel.quadDefs) {
			const int uvLockRotationDir = (quadDef.uvLock) ? getUVLockRotationDirection(quadDef, axis) : 0;
			const int secondaryUvLockRotations = (quadDef.uvLock) ? getUVLockSecondaryRotations(quadDef, axis, rotations) : 0;
			quadDef.cullFace = direction::rotateAround(quadDef.cullFace, axis, RotationDirection::CW, rotations);
			for (auto& vert : quadDef.verts) {
				vert.transform(matrix);
				if (uvLockRotationDir != 0) // rotate texCoords if quad has uvLock enabled
					vert.texCoord = rotateTexCoord(vert.texCoord, rotations * uvLockRotationDir);
				if (secondaryUvLockRotations != 0) // rotate texCoords if quad has uvLock enabled
					vert.texCoord = rotateTexCoord(vert.texCoord, secondaryUvLockRotations);
			}
		}
		return rotatedModel;
	}
	BlockModel BlockModel::getTranslated(const glm::vec3& offset) const {
		BlockModel translatedModel = *this;
		for (auto& quadDef : translatedModel.quadDefs) {
			for (auto& vert : quadDef.verts)
				vert += offset;
		}
		return translatedModel;
	}

}

namespace jonk {
	template<>
	struct JonkTypeCompat<eng::BlockVert> {
		using BlockVert = eng::BlockVert;
		static Jonk toJonk(const BlockVert& vert) {
			JonkObject obj {};
			obj.emplace("pos", vert.pos);
			obj.emplace("uv", vert.texCoord);
			return obj;
		}
		static BlockVert fromJonk(const Jonk& jonk) {
			if (jonk.isObject()) {
				const JonkObject& jObj = jonk.asObject();
				if (jObj.hasKey<glm::vec3>("pos") && jObj.hasKey<glm::vec2>("uv"))
					return { jObj.get<glm::vec3>("pos"), jObj.get<glm::vec2>("uv") };
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (!jonk.isObject()) return false;
			const JonkObject& jObj = jonk.asObject();
			return (jObj.hasKey<glm::vec3>("pos") && jObj.hasKey<glm::vec2>("uv"));
		}
	};
	template<>
	struct JonkTypeCompat<eng::BlockModel::QuadDefinition> {
		using QuadDefinition = eng::BlockModel::QuadDefinition;
		static QuadDefinition fromJonk(const Jonk& jonk) {
			using namespace eng;
			if (jonk.isObject()) {
				const JonkObject& jObj = jonk.asObject();
				if (jObj.hasKey<std::string>("texture") && jObj.hasKey<std::array<BlockVert, 4>>("verts")) {
					return QuadDefinition {
						jObj.get<std::string>("texture"),
						jObj.get<std::array<BlockVert, 4>>("verts"),
						jObj.getOrDefault<Color>("color", Color(0xFF, 0xFF, 0xFF)),
						jObj.getOptional<uint8_t>("colorIndex"),
						jObj.getOrDefault<Direction>("cullFace", Direction::UNDEFINED),
						jObj.getOrDefault<RenderLayer>("renderLayer", RenderLayer::Opaque),
						jObj.getOrDefault<bool>("uvLock"),
						jObj.getOrDefault<bool>("doubleSided")
					};
				}
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			using namespace eng;
			if (!jonk.isObject()) return false;
			const JonkObject& jObj = jonk.asObject();
			// require a texture and 4 vertices
			return (jObj.hasKey<std::string>("texture") && !jObj.get<std::string>("texture").empty())
				&& jObj.hasKey<std::array<BlockVert, 4>>("verts");
		}
	};
}

namespace eng {

	BlockModel BlockModel::fromJonk(const std::string& name, const jonk::JonkObject& jonkObj) {
		std::optional<std::string> parent = std::nullopt;
		if (jonkObj.hasKey("parent")) {
			const jonk::Jonk& j = jonkObj.get("parent");
			if (j.isString() && !j.asString().empty())
				parent = j.asString();
			else if (!j.isNull())
				std::cerr << "Model Error: 'parent' must be a non-empty string [in model `" << name << "`]\n";
		}
		std::map<std::string, std::string> textureVariables {};
		if (jonkObj.hasKey("textures")) {
			const jonk::Jonk& j = jonkObj.get("textures");
			if (j.is<std::map<std::string, std::string>>())
				textureVariables = j.get<std::map<std::string, std::string>>();
			else
				std::cerr << "Model Error: 'textures' must be a map of texture variables to texture locations [in model `" << name << "`]\n";
		}
		bool hasQuadList = false;
		std::vector<QuadDefinition> quadDefs {};
		if (jonkObj.hasKey("quads")) {
			const jonk::Jonk& j = jonkObj.get("quads");
			hasQuadList = !j.isNull();
			if (j.isArray() || j.isNull()) {
				if (j.is<std::vector<QuadDefinition>>())
					quadDefs = j.get<std::vector<QuadDefinition>>();
			} else {
				std::cerr << "Model Error: 'quads' must be an array of valid quads [in model `" << name << "`]\n";
				return BlockModel::missing_model;
			}
		}
		if (!hasQuadList && parent)
			return BlockModel(name, *parent, textureVariables);
		else
			return BlockModel(name, parent, textureVariables, quadDefs);
	}

}