#include "GameState.h"
#include "MCTS.h"
#include "Listener.h"
#include "utils.h"

#include <iostream>

int main() {
	auto const address = net::ip::make_address("0.0.0.0");
	constexpr unsigned short port = 8080;

	net::io_context ioc{1};
	std::make_shared<Listener>(ioc, tcp::endpoint{address, port}, [](const std::string& request) {
		try {
			MCTS mcts(5000);

			const char color = requestParts.at(0).at(0);
			const std::string& board = requestParts.at(1);
			std::vector<std::string> previousBoards;
			if (requestParts.size() > 2) {
				previousBoards = {requestParts.begin() + 2, requestParts.end()};
			}
			
			GameState* gameState = GameState::newGame(color, board, previousBoards);

			std::string response = std::to_string(gameState->getValidMoves()->at(mcts.getBestMove(gameState)));

			delete gameState;

			return response;
		} catch (std::exception& e) {
			std::cout << "Request body formatted incorrectly." << '\n';
			std::cout << "Request: " << request << '\n';
			std::cout << "Exception: " << e.what() << '\n';

			return std::string("Request body formatted incorrectly.");
		}
	})->run();

	ioc.run();

	return 0;
}
