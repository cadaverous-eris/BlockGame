#pragma once

#include <memory>

#include "TextureAtlas.h"
#include "BlockModelManager.h"

namespace eng {

	class Game;

	class ResourceManager {
	private:
		static std::unique_ptr<ResourceManager> resourceManagerInstance;

		Game* game;

		TextureAtlas blockTextures;
		BlockModelManager blockModels;
		// TODO: manage fonts
		// TODO: manage shaders
		// TODO: manage sounds

		ResourceManager(Game& game);


	public:

		inline static void initInstance(Game& game) {
			if (!resourceManagerInstance)
				resourceManagerInstance = std::unique_ptr<ResourceManager>(new ResourceManager(game));
		}
		inline static void destroyInstance() {
			if (resourceManagerInstance)
				resourceManagerInstance = nullptr;
		}

		void loadResources();

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator =(const ResourceManager&) = delete;
		ResourceManager& operator =(ResourceManager&&) = delete;

		inline static ResourceManager& instance() noexcept { return *resourceManagerInstance; }
		
		inline const TextureAtlas& getBlockTextures() const noexcept { return blockTextures; }

		inline const BlockModelManager& getBlockModels() const noexcept { return blockModels; }

		//void reloadResources();

		inline void updateBlockAnimations() {
			blockTextures.updateAnimations();
		}

	};

}