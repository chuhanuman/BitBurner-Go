#include <iostream>

#include "game/GameState.h"
#include "ai/MCTS.h"

int main(int argc, char* argv[]) {
	NeuralNetwork neuralNetwork;
	if (argc >= 2) {
		if (!neuralNetwork.load(argv[1])) {
			std::cout << "ERROR: Starting current model did not load correctly from " << argv[1] << '\n';
		}
	}

	int simulations = 100;
	if (argc >= 3) {
		simulations = std::stoi(argv[2]);
	}

	MCTS mcts(&neuralNetwork, simulations);

	GameState* gameState = GameState::newGame('X', "#...#.....#.O...........#");
	gameState->printGameState();

	while (gameState->getEndState() < -1) {
		std::pair<std::vector<float>, float> prediction = neuralNetwork.predict(gameState);
		std::vector<float> moveProbabilities = mcts.getMoveProbabilities(gameState);
		for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
			std::cout << i << " ";
			std::cout << gameState->getValidMoves()->at(i) << " ";
			std::cout << prediction.first[gameState->getValidMoves()->at(i) + 1] << " ";
			std::cout << moveProbabilities[gameState->getValidMoves()->at(i) + 1] << " ";
			std::cout << mcts.getMoveValue(gameState->getChild(i)) << '\n';
		}
		std::cout << prediction.second << '\n';

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