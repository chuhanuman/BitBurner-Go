#include <iostream>

#include "game/GameState.h"
#include "ai/MCTS.h"

int main(int argc, char* argv[]) {
	NeuralNetwork neuralNetwork;
	MCTS mcts(&neuralNetwork, 100);
	if (argc >= 2) {
		if (!neuralNetwork.load(argv[1])) {
			std::cout << "ERROR: Starting current model did not load correctly from " << argv[1] << '\n';
		}
	}

	GameState* gameState = GameState::newGame('X', "#...#.....#.O...........#");
	gameState->printGameState();

	while (gameState->getEndState() < -1) {
		std::vector<float> moveProbabilities = mcts.getMoveProbabilities(gameState);
		for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
			std::cout << i << " " << gameState->getValidMoves()->at(i) << " " << moveProbabilities[gameState->getValidMoves()->at(i) + 1] << " " << mcts.getMoveValue(gameState->getChild(i)) << '\n';
		}

		int moveToPlay;
		std::cin >> moveToPlay;

		GameState* childGameState = gameState->getChild(moveToPlay, false);
		delete gameState;
		gameState = childGameState;

		gameState->printGameState();
		mcts.reset();
	}

	std::cout << gameState->getEndState() << '\n';

	delete gameState;

	return 0;
}