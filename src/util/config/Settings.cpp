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
			if (settingsObj.hasKey<int16_t>("chunk_load_radius")) {
				settings.chunkLoadRadius = settingsObj.get<int16_t>("chunk_load_radius");
				settings.chunkLoadRadius = std::clamp(settings.chunkLoadRadius, Settings::min_chunk_load_radius, Settings::max_chunk_load_radius);
			}
			if (settingsObj.hasKey<int32_t>("ui_scale")) {
				settings.uiScale = std::clamp(settingsObj.get<int32_t>("ui_scale"), Settings::min_ui_scale, Settings::max_ui_scale);
			}
			if (settingsObj.hasKey<glm::i16vec2>("window_size")) {
				settings.windowSize = settingsObj.get<glm::i16vec2>("window_size");
				if (settings.windowSize.x < min_window_size.x) settings.windowSize.x = min_window_size.x;
				if (settings.windowSize.y < min_window_size.y) settings.windowSize.y = min_window_size.y;
			}
			if (settingsObj.hasKey<Object>("keybinds")) {
				using namespace eng::input;
				JonkObject& keyBindsObj = settingsObj.at<Object>("keybinds");
				for (const auto& [keyBindName, keyInput] : keyBindsObj) {
					if (auto keyBind = findKeyBind(keyBindName); keyBind) {
						if (keyInput.is<KeyInput>()) {
							bind(*keyBind, keyInput.get<KeyInput>());
						} else if (keyInput.isNull()) {
							unbind(*keyBind);
						}
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