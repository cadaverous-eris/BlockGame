#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

#include <glm/vec2.hpp>
#include <glm/common.hpp>

#include "util/resources/Image.h"

namespace eng {

	class TextureAtlas;

	extern const Image4 missingTextureImage;

	class AtlasSprite {
	public:
		friend class TextureAtlas;

		using svec2 = glm::vec<2, size_t>;

		struct AnimationFrame {
			size_t index;
			size_t time;
		};
		struct AnimationConfig {
			svec2 frameSize;
			std::vector<AnimationFrame> frames;
		};
	private:

		// frame order: first by frame number, then by mipmap level

		std::string name;
		svec2 position = { 0, 0 }; // position in the atlas
		std::unique_ptr<Image4[]> frames = nullptr; // stores all frames at every mipmap level
		size_t numFrames = 1;

		size_t animFrame = 0, animFrameTime = 0;
		std::optional<AnimationConfig> animConfig = std::nullopt;

		TextureAtlas* atlas;

	public:

		AtlasSprite(TextureAtlas& atlas, const std::string& name);

		AtlasSprite(const AtlasSprite&) = delete;
		AtlasSprite(AtlasSprite&&) = default;

		AtlasSprite& operator =(const AtlasSprite&) = delete;
		AtlasSprite& operator =(AtlasSprite&&) = default;

		inline size_t getWidth() const noexcept { return frames[0].getWidth(); }
		inline size_t getHeight() const noexcept { return frames[0].getHeight(); }
		inline svec2 getFrameSize() const noexcept { return { getWidth(), getHeight() }; }

		inline bool isAnimated() const noexcept { return numFrames > 1; }

		inline size_t getFrameCount() const noexcept { return numFrames; }

		Image4& getFrame(size_t frame = 0, size_t level = 0) noexcept;
		const Image4& getFrame(size_t frame = 0, size_t level = 0) const noexcept;

		svec2 getAlignment() const;

		glm::vec2 getMinUV() const noexcept;
		glm::vec2 getMaxUV() const noexcept;


		void updateAnimation();

		inline size_t getAnimationFrameIndex() const noexcept {
			return (isAnimated()) ? animConfig->frames[animFrame].index : 0;
		}

		inline svec2 getPosition(size_t level = 0) const noexcept {
			return position / (size_t{1} << level);
		}

		static AtlasSprite createMissingTextureSprite(TextureAtlas& atlas);

	private:

		AtlasSprite(TextureAtlas& atlas, const std::string& name, const Image4& image);
		

	};

}