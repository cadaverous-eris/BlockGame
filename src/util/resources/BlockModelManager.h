#pragma once

#include <unordered_map>
#include <set>
#include <string>

#include "model/block/BlockModel.h"
#include "model/block/BakedBlockModel.h"

namespace eng {

	class BlockModelManager;

	class BlockModelLoader {
		/*
		Model loading process:
		- create ordered list of model names to load
		- phase 1 loading of every model in the list
			- skip if already loaded
			- parse model jonk file
			- if parent has not been loaded, add to end of loading queue
		- phase 2 loading of every model
			- check every model for circular dependencies
				- if a circular dependency is found, replace all models in the inheritance heirarchy to the missing model
			- sort model list so that every model is at a higher index than it's parent
			- resolve inherited properties
				- if model has errors, replace with missing model
				- if parent model is missing model, replace with missing model
			- resolve texture locations
		*/
		friend class BlockModelManager;

		std::set<std::string> modelNames {};
		std::vector<std::string> loadingQueue {};
		std::unordered_map<std::string, BlockModel> loadedModels {};
	
		BlockModelLoader() = default;

		void doLoadingPhase1();
		void doLoadingPhase2();
	
	public:
		BlockModelLoader(const BlockModelLoader&) = delete;
		BlockModelLoader(BlockModelLoader&&) = default;
		BlockModelLoader& operator =(const BlockModelLoader&) = delete;
		BlockModelLoader& operator =(BlockModelLoader&&) = default;

		// TODO: add a method for manually inserting a BlockModel object after loading phase 1?

	};

	class BlockModelManager {
	private:

		std::unordered_map<std::string, BlockModel> models {};

	public:

		BlockModelManager() = default;
		BlockModelManager(const BlockModelManager&) = delete;
		BlockModelManager(BlockModelManager&&) = delete;
		BlockModelManager& operator =(const BlockModelManager&) = delete;
		BlockModelManager& operator =(BlockModelManager&&) = delete;

		inline bool hasModel(const std::string& modelName) const {
			return (models.find(modelName) != models.end());
		}

		const BlockModel& getModel(const std::string& modelName) const;

		BakedBlockModel createBakedModel(const std::string& modelName) const;

		std::set<std::string> getTextureLocations() const;


		void loadModels();

	};

}