#include <iostream>

#include "game/GameState.h"
#include "ai/MCTS.h"

int main() {
	NeuralNetwork nn;
	MCTS mcts(&nn, 100);

	GameState* root = GameState::newGame('X', "#...#.....#.O...........#");
	GameState* gameState = root;
	gameState->printGameState();

	while (gameState->getEndState() < -1) {
		const unsigned int bestMove = mcts.getBestMove(gameState);

		for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
			std::cout << i << " " << gameState->getValidMoves()->at(i) << " " << mcts.getMoveValue(gameState->getChild(i)) << '\n';
		}
		
		std::cout << bestMove << " " << gameState->getValidMoves()->at(bestMove) << '\n';

		int moveToPlay;
		std::cin >> moveToPlay;

		gameState = gameState->getChild(moveToPlay, false);

		gameState->printGameState();
	}

	std::cout << gameState->getEndState() << '\n';

	delete root;

	return 0;
}