#pragma once

#include <glm/mat4x4.hpp>

#include "GameState.h"
#include "render/Renderer.h"
#include "render/Camera.h"
#include "input/KeyBind.h"
#include "world/World.h"
#include "render/world/WorldRenderer.h"
#include "render/ui/HudRenderer.h"
#include "util/RayCastResult.h"
#include "block/BlockRegistry.h"
#include "fluid/FluidRegistry.h"

namespace eng {

	class Game;

	class PlayState : public GameState {
	private:

		World world;

		Camera camera;

		WorldRenderer worldRenderer;

		HudRenderer hudRenderer;

		RayCastResult<float> playerRayCastResult;
		int blockPlaceDelay = 0;
		int blockBreakDelay = 0;
		const Block* selectedBlock = &blocks::cobblestone;

		int fluidPlaceDelay = 0; // TODO: remove
		int fluidBreakDelay = 0; // TODO: remove

		const float cameraSpeed = 0.2f;
		const float cameraSensitivity = 0.5f;

		input::KeyBind::PressHandler pauseKeyHandler;
		input::KeyBind::PressHandler inventoryKeyHandler;
		input::KeyBind::PressHandler printCameraKeyHandler;
		input::KeyBind::PressHandler takeScreenshotKeyHandler;

	public:
		PlayState(Game* const game); // TODO: take argument (WorldHandle?) to determine a world to load/create
		/*~PlayState();

		PlayState(const PlayState&) = delete;
		PlayState(PlayState&&) = default;
		PlayState& operator =(const PlayState&) = delete;
		PlayState& operator =(PlayState&&) = default;*/

		void input();

		void update();

		void render(float partialTicks);

		void rendererResize(size_t width, size_t height);

		void onGuiOpened();
		void onGuiClosed();

		void openInventory() const;
		void openPauseMenu() const;

		const RayCastResultF& getPlayerRayCastResult() const noexcept { return playerRayCastResult; }


		const Block* getSelectedBlock() const noexcept { return selectedBlock; }

		const Camera& getCamera() const noexcept { return camera; }

	private:

		void takeScreenshot() const;

	};

}