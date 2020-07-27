#include "ResourceManager.h"

#include <iostream>

#include "Game.h"
#include "block/BlockRegistry.h"
#include "fluid/FluidRegistry.h"

namespace eng {

	std::unique_ptr<ResourceManager> ResourceManager::resourceManagerInstance = nullptr;

	ResourceManager::ResourceManager(Game& game) : game(&game), blockTextures(3) {}

	void ResourceManager::loadResources() {
		std::cout << "\nLoading resources\n";

		blockModels.loadModels();

		auto textureNames = blockModels.getTextureLocations();
		for (auto& fluid : fluid_registry)
			fluid.addTextureNamesToSet(textureNames);
		blockTextures.initAtlas(std::begin(textureNames), std::end(textureNames));
		blockTextures.dumpAtlasImages();
		
		for (auto& block : block_registry)
			block.onResourceLoadComplete();
		for (auto& fluid : fluid_registry)
			fluid.onResourceLoadComplete();

		std::cout << "\nResource loading complete\n\n";
	}

}