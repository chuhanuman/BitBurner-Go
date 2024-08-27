#include <iostream>

#include "GameState.h"
#include "MCTS.h"

int main() {
	MCTS mcts(10000);

	GameState* root = GameState::newGame('X', ".........................");
	GameState* gameState = root;
	gameState->printGameState();

	while (gameState->getEndState() < -1) {
		const unsigned int bestMove = mcts.getBestMove(gameState);

		for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
			std::cout << gameState->getValidMoves()->at(i) << " " << mcts.getMoveValue(gameState->getChild(i)) << '\n';
		}
		
		std::cout << gameState->getValidMoves()->at(bestMove) << '\n';

		gameState = gameState->getChild(bestMove, false);

		gameState->printGameState();
	}

	std::cout << gameState->getEndState() << '\n';

	delete root;

	return 0;
}