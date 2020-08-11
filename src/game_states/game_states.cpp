#include "game_states.h"

namespace eng {

	void doGameStateUpdate(GameStateVariant& gameStateVariant) {
		std::visit([](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T>) gameState.update();
		}, gameStateVariant);
	}
	void doGameStateInput(GameStateVariant& gameStateVariant) {
		std::visit([](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T>) gameState.input();
		}, gameStateVariant);
	}
	void doGameStateRender(GameStateVariant& gameStateVariant, const float partialTicks) {
		std::visit([partialTicks](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T>) gameState.render(partialTicks);
		}, gameStateVariant);
	}
	void doGameStateRendererResize(GameStateVariant& gameStateVariant, size_t width, size_t height) {
		std::visit([width, height](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T> && requires (T gs, size_t w, size_t h) { gs.rendererResize(w, h); })
				gameState.rendererResize(width, height);
		}, gameStateVariant);
	}
	void doGameStateRendererRescale(GameStateVariant& gameStateVariant, float scaleX, float scaleY) {
		std::visit([scaleX, scaleY](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T> && requires (T gs, float sx, float sy) { gs.rendererRescale(sx, sy); })
				gameState.rendererRescale(scaleX, scaleY);
		}, gameStateVariant);
	}
	void doGameStateWindowMaximizeCallback(GameStateVariant& gameStateVariant, bool maximized) {
		std::visit([maximized](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T> && requires (T gs, bool m) { gs.windowMaximizeCallback(m); })
				gameState.windowMaximizeCallback(maximized);
		}, gameStateVariant);
	}
	void doGameStateWindowMinimizeCallback(GameStateVariant& gameStateVariant, bool minimized) {
		std::visit([minimized](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T> && requires (T gs, bool m) { gs.windowMinimizeCallback(m); })
				gameState.windowMinimizeCallback(minimized);
		}, gameStateVariant);
	}
	void doGameStateWindowFocusCallback(GameStateVariant& gameStateVariant, bool focused) {
		std::visit([focused](auto& gameState) {
			using T = std::decay_t<decltype(gameState)>;
			if constexpr (std::is_base_of_v<GameState, T> && requires (T gs, bool m) { gs.windowFocusCallback(m); })
				gameState.windowFocusCallback(focused);
		}, gameStateVariant);
	}


	void setGameStateFromInitializer(GameStateVariant& gameState, GameStateInitializerVariant& initializerVariant) {
		std::visit([&](auto& initializerPtr) {
			using T = std::decay_t<decltype(initializerPtr)>;
			if constexpr (std::is_same_v<std::monostate, T>) {
				gameState = std::monostate{};
			} else {
				if (initializerPtr) {
					initializerPtr->emplaceGameState(gameState);
				} else {
					gameState = std::monostate{};
				}
			}
		}, initializerVariant);
		initializerVariant = std::monostate{};
	}

	bool hasGameStateInitializer(const GameStateInitializerVariant& gameStateInitialierVariant) {
		return !std::holds_alternative<std::monostate>(gameStateInitialierVariant);
	}

}