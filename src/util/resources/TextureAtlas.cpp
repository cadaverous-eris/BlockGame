#include "TextureAtlas.h"

#include <sstream>
#include <iostream>

namespace eng {

	const std::string TextureAtlas::missing_texture_name = "missing";

	TextureAtlas::TextureAtlas(const size_t maxMipmap) :
			maxMipmap(maxMipmap), missingTextureSprite(nullptr) {}

	void TextureAtlas::addSprite(std::string_view name) {
		const auto [it, inserted] = sprites.try_emplace(std::string(name), *this, std::string(name));
		if (!inserted) {
			std::cout << "Tried to override existing atlas sprite \"" << name << "\"\n";
		}
	}

	void TextureAtlas::updateAnimations() {
		for (auto sprite : animatedSprites)
			sprite->updateAnimation();
	}

	void TextureAtlas::dumpAtlasImages() {
		for (size_t level = 0; level <= maxMipmap; level++) {
			std::stringstream sstr {};
			sstr << "texture_atlas_" << level << ".png";
			const auto atlasData = atlasTexture.getData<TextureFormat::RGBA, TextureDataType::UINT8>(level);
			ImageRGBA(atlasData.data.get(), atlasData.width, atlasData.height).writeImage(sstr.str());
		}
	}

	void TextureAtlas::packSprites() {
		RectPacker<int16_t> packer { glm::i16vec2{ 256, 256 } };
		for (auto& entry : sprites) {
			auto& sprite = entry.second;
			// pack the sprite into the atlas
			const auto& packedPos = packer.packRect(static_cast<glm::i16vec2>(sprite.getAlignment()));
			sprite.position = static_cast<svec2>(packedPos);
		}
		svec2 atlasSize = static_cast<svec2>(packer.getMinPackedBinSize());
		for (size_t i = 0; i < 2; i++) {
			const auto alignment = size_t{1} << (maxMipmap + 1);
			const auto mod = atlasSize[i] % alignment;
			if (mod > 0u) atlasSize[i] += (alignment - mod);
		}
		const auto scaleMode = (getMaxMipmapLevel() > 0) ? Texture::ScaleMode::NEAREST_MIPMAP_NEAREST : Texture::ScaleMode::NEAREST;
		atlasTexture.setMaxMipmapLevel(getMaxMipmapLevel()).setScaleMode(scaleMode);
		for (size_t level = 0; level <= maxMipmap; level++) { // initialize texture data
			const auto atlasLevelSize = atlasSize / getMipmapScale(level);
			atlasTexture.setData(level, Image4(nullptr, atlasLevelSize.x, atlasLevelSize.y));
		}
		// set texture data from sprites
		for (const auto& entry : sprites) {
			auto& sprite = entry.second;
			for (size_t level = 0; level <= maxMipmap; level++) {
				const auto frameIndex = sprite.getAnimationFrameIndex();
				atlasTexture.setSubData(level, sprite.getFrame(frameIndex, level), sprite.getPosition(level));
			}
		}
	}

}