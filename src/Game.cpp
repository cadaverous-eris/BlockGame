#include "Game.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "game_states/PlayState.h"
#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"
#include "world/World.h"
#include "block/BlockState.h"
#include "block/BlockRegistry.h"
#include "render/world/chunk/ChunkBakery.h"
#include "util/resources/ResourceManager.h"
#include "util/math/RNG.h"

namespace eng {

	static const std::vector<std::string> names {
		"Chonky Blocks",
		"Chonky Blocks",
		"Chonky Blocks",
		"Chonky Blocks",
		"Chonky Blocks 2: The Thiccening",
		std::string(reinterpret_cast<const char*>(u8"Rustic: The Game™©®")),
		std::string(reinterpret_cast<const char*>(u8"Rustic: The Game™©®")),
		std::string(reinterpret_cast<const char*>(u8"Rustic: The Game™©®")),
		"BlockSmith",
		"Blockintosh Plus",
		"Cube Land",
		"Blockarina of Mine",
		"MineClone",
		"MimeCraft",
		"Block Souls",
		"Blockborne",
	};

	Game::Game() :
			settings(loadSettingsFile()),
			window(this, settings.windowSize.x, settings.windowSize.y, ""),
			renderer(&window),
			inputManager(&window),
			name(names[RNG().nextUint32(names.size())]) {
		if (Game::gameInstance) {
			throw std::runtime_error("Game already created");
		}
		Game::gameInstance = this;

		window.setTitle(name);

		std::cout << "init" << std::endl;

		// initialize the renderer
		renderer.init();

		ResourceManager::initInstance(*this);
		ResourceManager::instance().loadResources();

		// set the chunk loading/unloading distance from the settings file
		World::setChunkLoadRadius(settings.chunkLoadRadius);

	}

	Game::~Game() {
		ResourceManager::destroyInstance();

		try {
			std::cout << "shutdown" << std::endl;
		} catch (...) {}
	}


	void Game::runGameLoop() {
		using namespace std::chrono_literals;
		using nanoseconds = std::chrono::nanoseconds;
		using milliseconds = std::chrono::milliseconds;

		prevLoopTime = clock::now();

		nanoseconds lag(0ns);
		
		nanoseconds timer(0ns);
		int fpsAccumulator = 0;
		int tpsAccumulator = 0;
		int fptAccumulator = 0; // frames per tick accumulator

		while (!shouldClose) {
			// handle timing
			const auto now = clock::now();
			auto elapsedTime = std::chrono::duration_cast<nanoseconds>(now - prevLoopTime);
			prevLoopTime = now;
			lag += elapsedTime;
			timer += elapsedTime;

			// average fps tracking
			if (std::chrono::duration_cast<milliseconds>(timer).count() > 1000) {
				avgFPS = fpsAccumulator;
				std::ostringstream windowTitleStream;
				//windowTitleStream << PROJECT_NAME << " [" << fpsAccumulator << "fps, " << tpsAccumulator << "tps]";
				windowTitleStream << name << " [" << fpsAccumulator << "fps]";
				window.setTitle(windowTitleStream.str());
				timer %= 1s;
				fpsAccumulator = 0;
				tpsAccumulator = 0;
			}

			// handle GameState changes
			if (hasGameStateInitializer(nextGameStateInitializer)) {
				setGameStateFromInitializer(currentGameState, nextGameStateInitializer);
			}

			// Handle input
			input();

			// Tick
			bool didTick = false;
			while (lag >= tickIntervalNanos) {
				update();
				tpsAccumulator++;
				lag -= tickIntervalNanos;
				
				if (!didTick) {
					const auto currentTickTime = clock::now();
					if (fptAccumulator > 0) {
						// seconds between last recorded tick time and current tick time
						const auto dsec = static_cast<double>((currentTickTime - prevFPTTickTime).count()) / clock::duration(1s).count();
						fps = static_cast<int>(fptAccumulator / dsec);
					}
					fptAccumulator = 0;
					prevFPTTickTime = currentTickTime;
				}
				didTick = true;
			}

			// Calculate partial ticks for render
			const float partialTicks = static_cast<float>(lag.count()) / tickIntervalNanos.count();
			if (partialTicks < 0 || partialTicks > 1) std::cerr << partialTicks << std::endl;

			// Render
			render(partialTicks);
			fpsAccumulator++;
			fptAccumulator++;
		}
	}

	void Game::input() {
		inputManager.input(); // Poll for and process input events

		doGameStateInput(currentGameState);
	}

	void Game::update() {
		doGameStateUpdate(currentGameState);


		inputManager.update();
	}

	void Game::render(float partialTicks) {
		// TODO: implement

		doGameStateRender(currentGameState, partialTicks);

		renderer.getUIRenderer()->render(partialTicks);
		renderer.getFontRenderer()->render(partialTicks);

		// Swap front and back buffers (update the display)
		window.swapBuffers();
	}

	// TODO: remove? make optional?
	void Game::syncFrameRate() const {
		using namespace std::chrono_literals;
		// Don't even try rendering more than 60 frames per second
		if (!window.isVSync()) {
			auto endTime = prevLoopTime + renderIntervalNanos;
			while (clock::now() < endTime) {
				std::this_thread::sleep_for(1ms);
			}
		}
	}

	const GameStateVariant& Game::getGameState() const {
		return currentGameState;
	}
	GameStateVariant& Game::getGameState() {
		return currentGameState;
	}

	void Game::quit() {
		shouldClose = true;
	}

	void Game::rendererResize(size_t width, size_t height) {
		doGameStateRendererResize(currentGameState, width, height);
	}
	void Game::rendererRescale(float scaleX, float scaleY) {
		doGameStateRendererRescale(currentGameState, scaleX, scaleY);
	}

	void Game::windowMaximizeCallback(bool maximized) {
		doGameStateWindowMaximizeCallback(currentGameState, maximized);
	}
	void Game::windowMinimizeCallback(bool minimized) {
		doGameStateWindowMinimizeCallback(currentGameState, minimized);
	}
	void Game::windowFocusCallback(bool focused) {
		doGameStateWindowFocusCallback(currentGameState, focused);
	}

}