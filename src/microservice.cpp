#include "GameState.h"
#include "MCTS.h"
#include "Listener.h"

#include <iostream>

int main() {
	auto const address = net::ip::make_address("0.0.0.0");
	constexpr unsigned short port = 8080;

	net::io_context ioc{1};
	std::make_shared<Listener>(ioc, tcp::endpoint{address, port}, [](std::string request) {
		MCTS mcts(10000);
		GameState* gameState = GameState::newGame(request.at(0), request.substr(1));

		std::string response = std::to_string(gameState->getValidMoves()->at(mcts.getBestMove(gameState)));

		delete gameState;

		return response;
	})->run();

	ioc.run();

	return 0;
}
