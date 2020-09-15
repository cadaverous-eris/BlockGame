#pragma once

#include <tuple>
#include <type_traits>
#include <stdexcept>

namespace eng {

	class Game;

	class GameState {
	protected:
		Game* const game;

	public:
		GameState(Game* const game) : game(game) {}
		~GameState() = default;

		GameState(const GameState&) = delete;
		GameState(GameState&&) = default;
		GameState& operator =(const GameState&) = delete;
		GameState& operator =(GameState&&) = default;

		//void input() {}
		//void update() {}
		//void render(float partialTicks) {}

		//void rendererResize(size_t width, size_t height) {}
		//void rendererRescale(float scaleX, float scaleY) {}
		//void windowMaximizeCallback(bool maximized) {}
		//void windowMinimizeCallback(bool minimized) {}
		//void windowFocusCallback(bool focused) {}

		//void onGuiOpened();
		//void onGuiClosed();

		Game* getGame() { return game; }
		const Game* getGame() const { return game; }
	};


	template<typename T, typename = void>
	struct is_game_state : std::false_type {};
	template<typename T>
	struct is_game_state<T, std::void_t<
		std::enable_if_t<std::is_base_of_v<GameState, T>> // TODO: detect methods?
	>> : std::true_type {};
	template<typename T>
	inline constexpr bool is_game_state_v = is_game_state<T>::value;

}
