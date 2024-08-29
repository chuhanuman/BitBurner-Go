#include "GameState.h"
#include "MCTS.h"
#include "Listener.h"

#include <iostream>

int main() {
	auto const address = net::ip::make_address("0.0.0.0");
	constexpr unsigned short port = 8080;

	net::io_context ioc{1};
	std::make_shared<Listener>(ioc, tcp::endpoint{address, port}, [](const std::string& request) {
		try {
			MCTS mcts(5000);
			GameState* gameState = GameState::newGame(request.at(0), request.substr(1));

			gameState->printGameState();

			std::string response = std::to_string(gameState->getValidMoves()->at(mcts.getBestMove(gameState)));

			delete gameState;

			std::cout << response << '\n';

			return response;
		} catch (...) {
			return std::string("Request body formatted incorrectly.");
		}
	})->run();

	ioc.run();

	return 0;
}
