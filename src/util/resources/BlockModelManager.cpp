#include "BlockModelManager.h"

#include <utility>
#include <algorithm>
#include <map>
#include <iostream>
#include <filesystem>

#include "ResourceManager.h"
#include "block/BlockRegistry.h"
#include "util/IOUtils.h"
#include "util/jonk/Jonk.h"

namespace eng {

	const BlockModel& BlockModelManager::getModel(const std::string& modelName) const {
		if (modelName.empty()) return BlockModel::missing_model;
		const auto it = models.find(modelName);
		return (it == models.end()) ? BlockModel::missing_model : it->second;
	}

	BakedBlockModel BlockModelManager::createBakedModel(const std::string& modelName) const {
		return getModel(modelName).bake(ResourceManager::instance().getBlockTextures());
	}

	std::set<std::string> BlockModelManager::getTextureLocations() const {
		std::set<std::string> textures {};
		for (const auto& [n, model] : models)
			textures.insert(model.getTextureLocations().begin(), model.getTextureLocations().end());
		return textures;
	}

	void BlockModelManager::loadModels() {
		BlockModelLoader loader {};
		loader.doLoadingPhase1();
		// TODO: insert any extra models into the loader here
		loader.doLoadingPhase2();

		models = std::move(loader.loadedModels);
	}



	inline static bool containsModel(const std::string& modelName, const std::unordered_map<std::string, BlockModel>& modelMap) {
		return (modelMap.find(modelName) != modelMap.end());
	}
	inline static bool containsModel(const std::string& modelName, const std::set<std::string>& modelNameSet) {
		return (modelNameSet.find(modelName) != modelNameSet.end());
	}
	// Checks if the model's inheritance heirarchy has any circular dependencies.
	// If any circular dependencies are found, replaces all models in the heirarchy with the missing model.
	// Returns true if a circular dependency was found, otherwise it returns false.
	static bool handleCircularDependencies(const std::string& baseModelName, BlockModel& baseModel, std::unordered_map<std::string, BlockModel>& modelMap) {
		std::map<std::string, BlockModel*> heirarchyModels {}; // set of the names of all models in the heirarchy
		BlockModel* model = &baseModel;
		heirarchyModels.insert(std::make_pair(baseModelName, model));
		while (model->getParentName()) {
			const std::string& parentName = *model->getParentName();
			if (heirarchyModels.find(parentName) != heirarchyModels.end()) { // circular dependency found
				std::cerr << "Model Error: circular dependency in models ";
				bool needsComma = false;
				for (auto& entry : heirarchyModels) {
					*entry.second = BlockModel::missing_model; // replace each model in the heirarchy to the missing model
					if (needsComma) std::cerr << ", ";
					needsComma = true;
					std::cerr << '`' << entry.first << '`';
				}
				std::cerr << '\n';
				return true;
			}
			auto it = modelMap.find(parentName);
			if (it == modelMap.end()) break;
			model = &it->second;
			heirarchyModels.insert(std::make_pair(parentName, model));
		}
		return false;
	}
	static size_t getInheritanceDepth(const BlockModel& baseModel, const std::unordered_map<std::string, BlockModel>& modelMap) {
		size_t depth { 0 };
		const BlockModel* model = &baseModel;
		while (model->getParentName()) {
			depth++;
			auto it = modelMap.find(*model->getParentName());
			if (it == modelMap.end()) break;
			model = &it->second;
		}
		return depth;
	}
	
	static const std::filesystem::path baseModelPath = std::filesystem::absolute("res/models");
	static BlockModel loadModelFile(const std::string& modelName) {
		namespace fs = std::filesystem;
		const std::filesystem::path path = (baseModelPath / modelName).replace_extension("jonk");
		const bool fileExists = fs::exists(path);
		const bool fileIsFile = fileExists && fs::is_regular_file(path);
		const bool fileIsEmpty = fileExists && fs::is_empty(path);
		if (fileExists && fileIsFile && !fileIsEmpty) {
			using namespace jonk;
			const std::string configFile = readTextFile(path.string());
			Jonk modelJonk = nullptr;
			try {
				modelJonk = jonk::parseJonk(configFile);
			} catch (const jonk::ParseError& e) {
				std::cerr << "Error parsing model `" << modelName << "`\n" << e.what() << '\n';
			}
			if (modelJonk.isObject())
				return BlockModel::fromJonk(modelName, modelJonk.asObject());
		} else {
			if (!fileExists || !fileIsFile)
				std::cerr << "Error loading model `" << modelName << "` -- Could not find model file at " << path << '\n';
			else if (fileIsEmpty)
				std::cerr << "Error loading model `" << modelName << "` -- Model file cannot be empty\n";
		}
		return BlockModel::missing_model;
	}

	void BlockModelLoader::doLoadingPhase1() {
		loadedModels = {};
		modelNames = {};
		// create a set of all block model names
		for (const auto& block : block_registry)
			block.addModelNamesToSet(modelNames);
		// remove the global missing model from the set in case for some reason it was present
		modelNames.erase(BlockModel::missing_model_name);
		// add all model names to a vector so that they can be loaded in a known order while appending parent model names to the end of the list
		loadingQueue = std::vector<std::string>(modelNames.begin(), modelNames.end());
		// add missing model name to the set to prevent it from being added to the loading queue in case some model(s) inherit from it for some reason
		modelNames.insert(BlockModel::missing_model_name);
		loadedModels.insert(std::make_pair(BlockModel::missing_model_name, BlockModel::missing_model));
		// phase 1 of model loading - parse all model jonk files and load parent models
		for (size_t i = 0; i < loadingQueue.size(); i++) {
			const std::string modelName = loadingQueue[i];
			if (!containsModel(modelName, loadedModels)) { // skip models that have already been loaded
				BlockModel model = loadModelFile(modelName); // parse the model file
				if (model.getParentName()) {
					const std::string parentName = *model.getParentName();
					// add parent model name to list of models to load if it is not already in the loading queue
					if (modelNames.find(parentName) == modelNames.end()) {
						modelNames.insert(parentName);
						loadingQueue.push_back(parentName);
					}
				}
				loadedModels.try_emplace(modelName, std::move(model));
			}
		}
	}
	void BlockModelLoader::doLoadingPhase2() {
		// handle circular dependencies
		for (const auto& modelName : loadingQueue) {
			auto it = loadedModels.find(modelName);
			if (it != loadedModels.end())
				handleCircularDependencies(it->first, it->second, loadedModels);
		}
		// sort model loading queue by inheritance heirarchy depth
		// (guarantees that all models will be sorted after all of their dependencies)
		{
			// create a map of model names to inheritance heirarchy depths so that the depth of each model only needs to be calculated once
			std::unordered_map<std::string, size_t> inheritanceDepths {};
			for (const auto& [modelName, model] : loadedModels)
				inheritanceDepths.try_emplace(modelName, getInheritanceDepth(model, loadedModels));
			// sort the loading queue
			std::sort(loadingQueue.begin(), loadingQueue.end(), [&](const std::string& a, const std::string& b) -> bool {
				return inheritanceDepths[a] < inheritanceDepths[b];
			});
		}

		// resolve inherited properties
		for (const auto& modelName : loadingQueue) {
			auto it = loadedModels.find(modelName);
			if (it != loadedModels.end())
				it->second.resolveInheritedProperties(loadedModels);
		}
		// resolve texture variables and create each model's list of used textures
		for (const auto& modelName : loadingQueue) {
			auto it = loadedModels.find(modelName);
			if (it != loadedModels.end())
				it->second.resolveTextureLocations();
		}

		//std::cout << "awdawdawdadawdawdawwad\n" << jonk::Jonk(loadedModels["block/grass"].textureVariables).toJonkString() << "\n";
	}

}