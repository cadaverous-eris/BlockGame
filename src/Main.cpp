#include "Game.h"
#include "game_states/PlayState.h"

#include <iostream>
//#include <glm/gtx/io.hpp>
//#include "util/direction.h"

#ifndef PROJECT_NAME
#define PROJECT_NAME=""
#endif

int main() {
	try {
		// initialize the game instance
		eng::Game game {};

		// initialize the game state
		game.changeGameState<eng::PlayState>();
		// start the game loop
		game.runGameLoop();

	} catch (const std::runtime_error& e) {
		std::cerr << "Runtime Exception thrown!\n\n" << e.what() << '\n';
		return 1;
	}

	return 0;
}
