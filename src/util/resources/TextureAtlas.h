#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/common.hpp>

#include "AtlasSprite.h"
#include "render/texture/Texture.h"
#include "util/type_traits.h"
#include "util/RectPacker.h"

namespace eng {

	class TextureAtlas {
		friend class AtlasSprite;
	public:
		using svec2 = AtlasSprite::svec2;

		const static std::string missing_texture_name;
	private:

		std::unordered_map<std::string, AtlasSprite> sprites;
		std::vector<AtlasSprite*> animatedSprites;

		Texture atlasTexture;

		size_t maxMipmap = 0; // maximum mipmap level of the atlas texture

		AtlasSprite* missingTextureSprite;

	public:

		TextureAtlas(size_t maxMipmap);

		TextureAtlas(const TextureAtlas&) = delete;
		TextureAtlas(TextureAtlas&&) = delete;
		TextureAtlas& operator =(const TextureAtlas&) = delete;
		TextureAtlas& operator =(TextureAtlas&&) = delete;

		template<typename Iter, typename T = typename std::iterator_traits<Iter>::value_type, std::enable_if_t<std::is_convertible_v<T, std::string_view>, int> = 0>
		void initAtlas(const Iter begin, const Iter end) {
			clearSprites();
			sprites.try_emplace(missing_texture_name, AtlasSprite::createMissingTextureSprite(*this));
			for (auto it = begin; it != end; it++)
				addSprite(*it);
			for (auto& entry : sprites) // add animated sprites to list after all sprites have been added so that the pointers don't change
				if (entry.second.isAnimated()) animatedSprites.push_back(&entry.second);
			packSprites();
			missingTextureSprite = &sprites.at(missing_texture_name);
		}

		const AtlasSprite& getSprite(const std::string& name) const {
			const auto it = sprites.find(name);
			return (it == sprites.end()) ? *missingTextureSprite : it->second;
		}
		const AtlasSprite& operator [](const std::string& name) const {
			const auto it = sprites.find(name);
			return (it == sprites.end()) ? *missingTextureSprite : it->second;
		}

		inline const AtlasSprite& getMissingTexture() const noexcept { return *missingTextureSprite; }

		inline bool hasTexture(const std::string& textureName) const {
			return /*(textureName == missing_texture_name) || */(sprites.find(textureName) != sprites.end());
		}

		inline size_t getWidth() const noexcept { return atlasTexture.getWidth(); }
		inline size_t getHeight() const noexcept { return atlasTexture.getHeight(); }
		inline svec2 getAtlasImageSize() const noexcept { return { atlasTexture.getWidth(), atlasTexture.getHeight() }; }

		// returns the number of mipmap levels the atlas has
		inline size_t getMipmapLevels() const noexcept { return maxMipmap + size_t{1}; }
		// returns the maximum mipmap level of the atlas texture
		inline size_t getMaxMipmapLevel() const noexcept { return maxMipmap; }

		void updateAnimations();

		void dumpAtlasImages();

		inline void bindTexture() const noexcept { atlasTexture.bind(); }

		inline constexpr static size_t getMipmapScale(const size_t mipmapLevel) noexcept {
			return size_t{1} << mipmapLevel;
		}

	private:
		
		inline void clearSprites() noexcept {
			sprites.clear();
			animatedSprites.clear();
			missingTextureSprite = nullptr;
		}

		void addSprite(std::string_view name);

		void packSprites();

	};

}