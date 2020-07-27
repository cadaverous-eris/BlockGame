#pragma once

#include <string>
#include <optional>

#include <glm/vec2.hpp>
#include <glm/fwd.hpp>

namespace eng {

	class Game;

	struct Settings {
		static inline constexpr const char* file_path = "settings.jonk";

		static inline constexpr int16_t min_chunk_load_radius = 24, max_chunk_load_radius = 512;
		static inline constexpr glm::i16vec2 min_window_size { 212, 120 };

		int16_t chunkLoadRadius = 192;
		int16_t chunkRenderRadius = 192;
		int16_t lightingRadius = 192;
		//int16_t chunkUpdateRadius = 192;
		glm::i16vec2 windowSize { 960, 540 };
		
		

		void applyChanges(Game& game);
	};

	Settings loadSettingsFile();
	void saveSettingsFile(const Settings& settings);

}