#include "Settings.h"

#include <stdexcept>

#include "Game.h"
#include "util/IOUtils.h"
#include "util/math/math.h"
#include "util/jonk/Jonk.h"
#include "SettingsJonkCompat.h"
#include "world/World.h"
#include "input/KeyBind.h"

namespace eng {

	void Settings::applyChanges(Game& game) {
		World::setChunkLoadRadius(chunkLoadRadius);
		game.renderer.setUIScale(uiScale);

		saveSettingsFile(*this);
	}

	Settings Settings::loadSettingsFile() {
		using namespace jonk;
		std::string fileText;
		Settings settings {};
		try {
			fileText = readTextFile(Settings::file_path);
		} catch (const std::runtime_error&) {
			//std::cerr << "Could not open settings.jonk\n";
			saveSettingsFile(settings);
			return settings;
		}
		Jonk settingsJonk;
		try {
			settingsJonk = parseJonk(fileText);
		} catch (const ParseError& e) {
			std::cerr << "Error parsing settings.jonk:" << '\n' << e.what() << '\n';
			saveSettingsFile(settings);
			return settings;
		}
		if (settingsJonk.isObject()) {
			JonkObject& settingsObj = settingsJonk.asObject();
			if (const auto chunkLoadRad = settingsObj.getOptional<int16_t>("chunk_load_radius"); chunkLoadRad)
				settings.chunkLoadRadius = std::clamp(*chunkLoadRad, min_chunk_load_radius, max_chunk_load_radius);
			if (const auto uiScale = settingsObj.getOptional<int32_t>("ui_scale"); uiScale)
				settings.uiScale = std::clamp(*uiScale, min_ui_scale, max_ui_scale);
			if (const auto windowSize = settingsObj.getOptional<glm::i16vec2>("window_size"); windowSize)
				settings.windowSize = glm::max(*windowSize, min_window_size);
			if (settingsObj.hasKey<Object>("keybinds")) {
				using namespace eng::input;
				JonkObject& keyBindsObj = settingsObj.at<Object>("keybinds");
				for (const auto& [keyBindName, keyInputConf] : keyBindsObj) {
					if (auto keyBind = findKeyBind(keyBindName); keyBind && keyBind->rebindable) {
						if (const auto keyInput = keyInputConf.getOptional<KeyInput>(); keyInput)
							bind(*keyBind, *keyInput);
						else if (keyInputConf.isNull())
							unbind(*keyBind);
					}
				}
			}
		} else {
			std::cerr << "Invalid settings file\n";
		}
		saveSettingsFile(settings);
		return settings;
	}

	void Settings::saveSettingsFile(const Settings& settings) {
		try {
			writeTextFile(Settings::file_path, jonk::Jonk(settings).toJonkString());
		} catch (const std::runtime_error& e) {
			std::cerr << "Error saving settings file:\n" << e.what() << '\n';
		}
	}

}