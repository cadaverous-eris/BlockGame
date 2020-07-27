#pragma once

#include <memory>
#include <variant>
#include <type_traits>

#include "GameState.h"
#include "PlayState.h"

namespace eng {

	using GameStateVariant = std::variant<std::monostate, PlayState>;


	template<typename GameStateType>
	struct GameStateInitializer {
		static_assert(std::is_base_of_v<GameState, GameStateType>, "GameStateInitializers can only create types that derive from GameState");
		using game_state_type = GameStateType;

		virtual ~GameStateInitializer() {}
		virtual void emplaceGameState(GameStateVariant& gameState) = 0;
	};

	template<typename GameStateType, typename... Args>
	class GameStateInitializerImpl : public GameStateInitializer<GameStateType> {
	private:
		using arg_tuple = std::tuple<Args...>;

		bool initialized = false;
		const arg_tuple constructorArgs;
	public:
		template <std::enable_if_t<std::is_constructible_v<GameStateType, Args...>>* = nullptr>
		GameStateInitializerImpl(Args&&... args) : constructorArgs(std::forward_as_tuple(args...)) {}

		void emplaceGameState(GameStateVariant& gameState) override {
			if (initialized) throw std::runtime_error("GameStateInitializer::emplaceGameState() can only be called once per initializer object");
			initialized = true;
			emplaceGameStateFromTuple(gameState, constructorArgs, std::index_sequence_for<Args...>());
		}

	private:
		template<size_t... Is>
		static void emplaceGameStateFromTuple(GameStateVariant& gameState, const arg_tuple& tuple, std::index_sequence<Is...>) {
			gameState.emplace<GameStateType>(std::get<Is>(tuple)...);
		}
	};


	template<typename GameStateType>
	using game_state_initializer_ptr = std::unique_ptr<GameStateInitializer<GameStateType>>;
	using GameStateInitializerVariant = std::variant<
		std::monostate,
		game_state_initializer_ptr<PlayState>
	>;

	void doGameStateUpdate(GameStateVariant&);
	void doGameStateInput(GameStateVariant&);
	void doGameStateRender(GameStateVariant&, const float partialTicks);
	void doGameStateRendererResize(GameStateVariant&, size_t width, size_t height);


	void setGameStateFromInitializer(GameStateVariant&, GameStateInitializerVariant&);

	bool hasGameStateInitializer(const GameStateInitializerVariant&);

}