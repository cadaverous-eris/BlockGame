#include "AtlasSprite.h"

// TODO: remove
#define __cpp_lib_bitops 201907L // needed in order to make the <bit> header work

#include <limits>
#include <bit>
#include <filesystem>
#include <map>
#include <utility>

#include "TextureAtlas.h"
#include "util/math/math.h"
#include "util/Color.h"
#include "util/IOUtils.h"
#include "util/jonk/Jonk.h"

namespace jonk {
	template<>
	struct JonkTypeCompat<eng::AtlasSprite::AnimationFrame> {
		using AnimationFrame = eng::AtlasSprite::AnimationFrame;
		static AnimationFrame fromJonk(const Jonk& jonk) {
			if (jonk.isObject()) {
				const JonkObject& j = jonk.asObject();
				if (j.hasKey<size_t>("index"))
					return { j.get<size_t>("index"), eng::vmax(j.getOrDefault<size_t>("time"), 1u) };
			} else if (jonk.is<size_t>()) {
				return { jonk.get<size_t>(), 1u };
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			if (jonk.isObject()) {
				const JonkObject& j = jonk.asObject();
				return j.hasKey<size_t>("index") && (!j.hasKey("time") || (j.hasKey<std::size_t>("time") && (j.get<size_t>("time") > 0)));
			}
			return jonk.is<size_t>();
		}
	};
	template<>
	struct JonkTypeCompat<eng::AtlasSprite::AnimationConfig> {
		using AnimationConfig = eng::AtlasSprite::AnimationConfig;
		using AnimationFrame = eng::AtlasSprite::AnimationFrame;
		using svec2 = eng::AtlasSprite::svec2;
		static AnimationConfig fromJonk(const Jonk& jonk) {
			if (jonk.isObject()) {
				const JonkObject& j = jonk.asObject();
				svec2 frameSize = svec2(0, 0);
				if (j.hasKey("frameSize")) {
					const auto& fj = j.at("frameSize");
					if (fj.is<svec2>()) frameSize = fj.get<svec2>();
					else if (fj.is<size_t>()) frameSize = svec2(fj.get<size_t>());
				}
				return { frameSize, j.getOrDefault<std::vector<AnimationFrame>>("frames") };
			}
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			// properties of the animation config can be defaulted, so it doesn't matter if they're present in the jonk object
			return jonk.isObject();
		}
	};
}

namespace eng {

	// returns the size of an image at a given mipmap level
	// mipmaps sizes are rounded up (unlike in opengl, which rounds down)
	static glm::vec<2, size_t> getMipmapSize(glm::vec<2, size_t> texSize, int level) noexcept {
		texSize = vmax(texSize, glm::vec<2, size_t>(1, 1));
		if (level == 0) return texSize;
		if (level < 0) return { texSize.x * (1 - level), texSize.y * (1 - level) };
		for (int i = 0; i < level; i++) {
			if (texSize.x > 1)
				texSize.x = (texSize.x + static_cast<size_t>((texSize.x % 2) == 1)) / 2;
			if (texSize.y > 1)
				texSize.y = (texSize.y + static_cast<size_t>((texSize.y % 2) == 1)) / 2;
		}
		return texSize;
	}

	// returns the highest mipmap level that an atlas sprite could have based on it's size
	static constexpr size_t getMaxMipmapLevel(size_t texSize) {
		constexpr auto digits = std::numeric_limits<size_t>::digits;
		if (texSize == 0) return 0;
		if ((texSize & (texSize - 1)) == 0) texSize -= 1;
		return digits - std::countl_zero(texSize);
	}
	// returns the highest mipmap level that an atlas sprite could have based on it's size
	static inline constexpr size_t getMaxMipmapLevel(const glm::vec<2, size_t>& texSize) {
		return vmin(getMaxMipmapLevel(texSize.x), getMaxMipmapLevel(texSize.y));
	}

	const Image4 missingTextureImage = ([]() -> Image4 {
		Image4 img = Image4(nullptr, 16u, 16u);
		for (size_t y = 0; y < img.getHeight(); y++) {
			for (size_t x = 0; x < img.getWidth(); x++) {
				const bool b = (x < (img.getWidth() / 2u)) == (y < (img.getHeight() / 2u));
				img.setPixel(x, y, b ? color::black : color::magenta);
			}
		}
		return img;
	})();

	static const std::filesystem::path baseSpritePath = std::filesystem::absolute("res/textures");
	static std::optional<AtlasSprite::AnimationConfig> loadAnimationConfig(const std::string& spriteName) {
		namespace fs = std::filesystem;
		const std::filesystem::path path = (baseSpritePath / spriteName).replace_extension("jonk");
		if (fs::exists(path) && fs::is_regular_file(path) && !fs::is_empty(path)) {
			using namespace jonk;
			const std::string configFile = readTextFile(path.string());
			Jonk configJonk = nullptr;
			try {
				configJonk = jonk::parseJonk(configFile);
			} catch (const jonk::ParseError& e) {
				std::cerr << "Error parsing sprite config for " << spriteName << ":\n" << e.what() << '\n';
			}
			if (configJonk.isObject())
				return configJonk.asObject().getOptional<AtlasSprite::AnimationConfig>("animation");
		}
		return std::nullopt;
	}
	static Image4 loadSpriteImage(const std::string& spriteName) {
		const std::filesystem::path path = (baseSpritePath / spriteName).replace_extension("png");
		try {
			return loadImage<4>(path.string());
		} catch (const std::runtime_error& err) {
			std::cerr << err.what() << '\n';
		}
		return missingTextureImage;
	}

	AtlasSprite::AtlasSprite(TextureAtlas& atlas, const std::string& name) : name(name), atlas(&atlas) {
		animConfig = loadAnimationConfig(name);
		Image4 img = loadSpriteImage(name);
		const svec2 imgSize { img.getWidth(), img.getHeight() };
		svec2 frameSize = imgSize;
		std::vector<size_t> frameIndexMap {};
		if (animConfig) {
			if (animConfig->frameSize.x > 0) frameSize.x = animConfig->frameSize.x;
			if (animConfig->frameSize.y > 0) frameSize.y = animConfig->frameSize.y;
			else frameSize.y = frameSize.x;

			if (!animConfig->frames.empty()) {
				std::multimap<size_t, size_t> usedFrameMap {};
				for (size_t i = 0; i < animConfig->frames.size(); i++)
					usedFrameMap.insert(std::make_pair(animConfig->frames[i].index, i));
				for (auto it = usedFrameMap.begin(); it != usedFrameMap.end();) {
					const auto& [key, val] = *it;
					const auto endIt = usedFrameMap.upper_bound(key);
					const size_t vecIndex = frameIndexMap.size();
					frameIndexMap.push_back(key);
					for (auto rangeIt = it; rangeIt != endIt; rangeIt++)
						animConfig->frames[rangeIt->second].index = vecIndex;
					it = endIt;
				}
				numFrames = frameIndexMap.size();
			} else {
				numFrames = vmax(1u, img.getHeight() / frameSize.y);
				animConfig->frames.resize(numFrames);
				frameIndexMap.resize(numFrames);
				for (size_t i = 0; i < numFrames; i++) {
					animConfig->frames[i] = { i, 1u };
					frameIndexMap[i] = i;
				}
			}
		} else {
			numFrames = 1;
		}

		const auto maxMipmapLevel = getMaxMipmapLevel(frameSize);
		size_t frameScale = 1;
		if (maxMipmapLevel < atlas.getMaxMipmapLevel())
			frameScale = TextureAtlas::getMipmapScale(atlas.getMaxMipmapLevel() - maxMipmapLevel);

		// initialize the array of frames
		this->frames = std::make_unique<Image4[]>(numFrames * atlas.getMipmapLevels());

		for (size_t frame = 0; frame < numFrames; frame++) {
			const auto l0FrameIndex = frame * atlas.getMipmapLevels();
			if ((frameSize == imgSize) && !isAnimated()) {
				if (frameScale > 1) frames[l0FrameIndex] = img.scaleNearest(frameScale);
				else frames[l0FrameIndex] = ((frame + 1u) == numFrames) ? std::move(img) : img;
			} else {
				const svec2 frameOffset { 0, frameSize.y * frameIndexMap[frame] };
				if (frameOffset.y >= imgSize.y) {
					Image4 frameImg = missingTextureImage.resizeLinear(frameSize.x, frameSize.y);
					frames[l0FrameIndex] = (frameScale > 1) ? frameImg.scaleNearest(frameScale) : std::move(frameImg);
				} else {
					Image4 frameImg = img.getSubData(frameOffset, vmin(frameSize.x, imgSize.x), vmin(frameSize.y, imgSize.y - frameOffset.y));
					if (svec2(frameImg.getWidth(), frameImg.getHeight()) == frameSize) {
						frames[l0FrameIndex] = (frameScale > 1) ? frameImg.scaleNearest(frameScale) : std::move(frameImg);
					} else {
						if (frameScale > 1) frameImg = frameImg.scaleNearest(frameScale);
						frames[l0FrameIndex] = Image4(frameSize.x, frameSize.y);
						frames[l0FrameIndex].setSubData(frameImg, { 0, 0 });
					}
				}
			}
			for (size_t level = 1; level <= atlas.getMaxMipmapLevel(); level++) {
				const svec2 level0Size { frames[l0FrameIndex].getWidth(), frames[l0FrameIndex].getHeight() };
				const auto mipmapSize = getMipmapSize(level0Size, level);
				frames[l0FrameIndex + level] = frames[l0FrameIndex].resizeNearest(mipmapSize.x, mipmapSize.y);
			}
		}
	}

	AtlasSprite::AtlasSprite(TextureAtlas& atlas, const std::string& name, const Image4& image) : name(name), atlas(&atlas) {
		const Image4* img = &image;
		svec2 imgSize { img->getWidth(), img->getHeight() };
		if ((imgSize.x == 0) || (imgSize.y == 0)) {
			img = &missingTextureImage;
			imgSize = { img->getWidth(), img->getHeight() };
		}
		const auto maxMipmapLevel = getMaxMipmapLevel(imgSize);
		numFrames = 1;

		size_t frameScale = 1;
		if (maxMipmapLevel < atlas.getMaxMipmapLevel())
			frameScale = TextureAtlas::getMipmapScale(atlas.getMaxMipmapLevel() - maxMipmapLevel);

		// initialize the array of frames
		this->frames = std::make_unique<Image4[]>(atlas.getMipmapLevels());
		
		frames[0] = (frameScale > 1) ? img->scaleNearest(frameScale) : *img;
		for (size_t level = 1; level <= atlas.getMaxMipmapLevel(); level++) {
			const svec2 level0Size { frames[0].getWidth(), frames[0].getHeight() };
			const auto mipmapSize = getMipmapSize(level0Size, level);
			frames[level] = frames[0].resizeNearest(mipmapSize.x, mipmapSize.y);
		}
	}

	AtlasSprite AtlasSprite::createMissingTextureSprite(TextureAtlas& atlas) {
		return AtlasSprite(atlas, TextureAtlas::missing_texture_name, missingTextureImage);
	}

	Image4& AtlasSprite::getFrame(size_t frame, size_t level) noexcept {
		frame %= numFrames;
		level = std::min(level, atlas->getMaxMipmapLevel());
		return frames[(frame * atlas->getMipmapLevels()) + level];
	}
	const Image4& AtlasSprite::getFrame(size_t frame, size_t level) const noexcept {
		frame %= numFrames;
		level = std::min(level, atlas->getMaxMipmapLevel());
		return frames[(frame * atlas->getMipmapLevels()) + level];
	}

	AtlasSprite::svec2 AtlasSprite::getAlignment() const {
		const auto maxLevel = atlas->getMaxMipmapLevel();
		return getMipmapSize(getFrameSize(), maxLevel) * TextureAtlas::getMipmapScale(maxLevel);
	}

	void AtlasSprite::updateAnimation() {
		animFrameTime++;
		if ((animFrameTime % animConfig->frames.at(animFrame).time) == 0) {
			animFrame = (animFrame + 1) % animConfig->frames.size();
			animFrameTime = 0;
			for (size_t level = 0; level <= atlas->getMaxMipmapLevel(); level++)
				atlas->atlasTexture.setSubData(level, getFrame(getAnimationFrameIndex(), level), getPosition(level));
		}
	}

	glm::vec2 AtlasSprite::getMinUV() const noexcept {
		return static_cast<glm::vec2>(position) / glm::vec2(atlas->getWidth(), atlas->getHeight());
	}
	glm::vec2 AtlasSprite::getMaxUV() const noexcept {
		return getMinUV() + glm::vec2(getWidth(), getHeight()) / glm::vec2(atlas->getWidth(), atlas->getHeight());
	}

}