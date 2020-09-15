#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <chrono>
#include <string>

#include <glm/vec2.hpp>

#include "render/Renderer.h"
#include "render/Window.h"
#include "input/InputManager.h"
#include "game_states/game_states.h"
#include "util/config/Settings.h"
#include "util/resources/TextureAtlas.h"

int main();

namespace eng {

	class Game;

	class Gui;

	class Game {
		friend int ::main();
	private:
		static inline Game* gameInstance = nullptr;
	public:
		using clock = std::chrono::steady_clock;

		// Game ticks per second
		static inline constexpr int TICKRATE = 20;
		// Target FPS
		static inline constexpr int FRAMERATE = 60;
		// nanoseconds between game ticks
		static inline constexpr std::chrono::nanoseconds tickIntervalNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<long long, std::ratio<1, Game::TICKRATE>>(1));
		// target render interval in nanoseconds
		static inline constexpr std::chrono::nanoseconds renderIntervalNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<long long, std::ratio<1, Game::FRAMERATE>>(1));

		Settings settings; // TODO: make private and add getters (and setters?)
		Window window; // the game's main window
		Renderer renderer;
		input::InputManager inputManager;

	private:
		bool shouldClose = false; // exit condition for game loop

		std::unique_ptr<Gui> nextGui;
		std::unique_ptr<Gui> activeGui;
		bool shouldCloseGui = false;

		int fps = 0; // approximate framerate updated once per tick batch
		int avgFPS = 0; // average framerate over the past second

		eng::GameStateVariant currentGameState = std::monostate{}; // the current GameState
		eng::GameStateInitializerVariant nextGameStateInitializer = std::monostate{}; // initializer for the next GameState

		// time point of the previous iteration of the game loop
		clock::time_point prevLoopTime = clock::now();
		// time point of the end of the first tick in the previous batch of ticks
		clock::time_point prevFPTTickTime = clock::now();

		input::KeyBind::PressHandler handleMaximizePressed { *input::keybinds::MAXIMIZE_WINDOW, [this](const input::KeyInput&) {
			if (this->window.isMaximized()) this->window.restore();
			else this->window.maximize();
		} };

		std::string name = "Chonky Blocks";

	public:
		inline static Game& instance() noexcept { return *gameInstance; }

		// Get a reference to the current GameState
		const GameStateVariant& getGameState() const;
		// Get a reference to the current GameState
		GameStateVariant& getGameState();

		// TODO: template methods to get current gamestate if it is of a certain type
		// TODO: template methods to check if specific gamestate type is active

		// Set the next GameState
		template<class GameStateType, typename... Args>
		void changeGameState(Args&&... args) {
			using initializer_ptr = game_state_initializer_ptr<GameStateType>;
			nextGameStateInitializer = initializer_ptr(new GameStateInitializerImpl<GameStateType, Game* const, Args...>(this, std::forward<Args>(args)...));
		}

		// Run the game loop
		void runGameLoop();

		// End the game loop, causing the program to shutdown
		void quit();

		bool isPaused() const noexcept;

		Gui* getActiveGui() const noexcept;
		void openGui(std::unique_ptr<Gui>&&) noexcept;
		void closeGui() noexcept;

		// called when the window size changes so that the current GameState can react to the change
		void rendererResize(size_t width, size_t height);
		// called when the window scale changes so that the current GameState can react to the change
		void rendererRescale(float scaleX, float scaleY);

		void windowMaximizeCallback(bool maximized);
		void windowMinimizeCallback(bool minimized);
		void windowFocusCallback(bool focused);

		inline int getFPS() const noexcept { return fps; }
		inline int getAvgFPS() const noexcept { return avgFPS; }
		inline const std::string& getName() noexcept { return name; }

		void applySettingsChanges();
		void saveSettingsFile();

		~Game();

	private:
		Game();

		Game(const Game&) = delete;
		Game(Game&&) = delete;

		Game& operator =(const Game&) = delete;
		Game& operator =(Game&&) = delete;


		void input();

		void update();

		void render(float partialTicks);

		// make thread sleep until it is time to render the next frame
		void syncFrameRate() const;

	};

}