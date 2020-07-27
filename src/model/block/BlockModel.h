#pragma once

#include <cmath>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <string>
#include <optional>
#include <unordered_map>

#include <glm/common.hpp>
#include <glm/vec3.hpp>

#include "BlockQuad.h"
#include "render/world/RenderLayer.h"
#include "render/world/chunk/MeshingWorldView.h"
#include "util/direction.h"

namespace jonk {
	class JonkObject;
}

namespace eng {

	class TextureAtlas;
	class AtlasSprite;
	class BakedBlockModel;

	class BlockModelLoader;

	BlockQuad createTexturedBlockQuad(BlockVert v0, BlockVert v1, BlockVert v2, BlockVert v3, const AtlasSprite& texture, const Color& color = Color(0xFF, 0xFF, 0xFF));
	BlockQuad createSimpleTexturedBlockQuad(const glm::vec3& pos0, const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3, const AtlasSprite& texture, const Color& color = Color(0xFF, 0xFF, 0xFF)) noexcept;

	class BlockModel {
		friend class BakedBlockModel;
		//friend class BlockModelLoader;
	public:
		struct QuadDefinition {
			std::string texture;
			std::array<BlockVert, 4> verts;
			Color color { 0xFF, 0xFF, 0xFF };
			std::optional<uint8_t> colorIndex = std::nullopt;
			Direction cullFace = Direction::UNDEFINED;
			RenderLayer renderLayer = RenderLayer::Opaque;
			bool uvLock = false;
			bool doubleSided = false;
			
			// returns true if the first tri (verts 1, 2, and 3) is a valid triangle (all 3 vertices are at different positions)
			inline bool hasTri1() const noexcept {
				return (verts[0].pos != verts[1].pos) && (verts[0].pos != verts[2].pos) && (verts[1].pos != verts[2].pos);
			}
			// returns true if the second tri (verts 3, 2, and 4) is a valid triangle (all 3 vertices are at different positions)
			inline bool hasTri2() const noexcept {
				return (verts[2].pos != verts[1].pos) && (verts[2].pos != verts[3].pos) && (verts[1].pos != verts[3].pos);
			}
			// returns true if either tri is a valid triangle (all 3 vertices  are at different positions)
			inline operator bool() const noexcept { return hasTri1() || hasTri2(); }
			// Returns the normal vector of the first tri (verts 1, 2, and 3).
			// Result can be used as the vertex normal for verts 1 and 3.
			glm::vec3 getNormal1() const;
			// Returns the normal vector of the first tri (verts 3, 2, and 4).
			// Result can be used as the vertex normal for verts 1 and 3.
			glm::vec3 getNormal2() const;
			// returns the normalized average of the normals of both tris
			glm::vec3 getAvgNormal() const;
		};

	private:
		std::string name;
		std::optional<std::string> parent;
		std::map<std::string, std::string> textureVariables;
		std::set<std::string> textureLocations;
		std::vector<QuadDefinition> quadDefs;
		bool inheritQuads = false;
	
	public:
		static const std::string missing_model_name;
		static const BlockModel missing_model;

		inline static constexpr char texture_variable_prefix = '$';

		inline const std::string& getName() const noexcept { return name; }
		inline const std::optional<std::string>& getParentName() const noexcept { return parent; }
		inline const std::set<std::string>& getTextureLocations() const noexcept { return textureLocations; }

		inline const std::vector<QuadDefinition>& getQuadDefs() const noexcept { return quadDefs; }
		inline std::vector<QuadDefinition>& getQuadDefs() noexcept { return quadDefs; }

		BlockModel() = default;

		BlockModel(const std::string& name, const std::optional<std::string>& parent, const std::map<std::string, std::string>& textureVariables, const std::vector<QuadDefinition>& quadDefs);
		BlockModel(const std::string& name, const std::string& parent, const std::map<std::string, std::string>& textureVariables);

		BlockModel(const BlockModel&) = default;
		BlockModel(BlockModel&&) = default;
		BlockModel& operator =(const BlockModel&) = default;
		BlockModel& operator =(BlockModel&&) = default;

		static BlockModel fromJonk(const std::string& name, const jonk::JonkObject& jonkObj);

		// should be called after all parent models have been loaded, and all models have been sorted
		void resolveInheritedProperties(const std::unordered_map<std::string, BlockModel>& models);
		// Should be called after all parent models have been loaded and inherited properties have been propagated from parents.
		// Resolves texture locations of all quads
		void resolveTextureLocations();

		BlockModel getRotated(const Axis axis, const int rotations) const;
		BlockModel getTranslated(const glm::vec3& offset) const;

		BakedBlockModel bake(const TextureAtlas& textureAtlas) const;

	};

}