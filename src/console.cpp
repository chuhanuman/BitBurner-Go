#include <iostream>

#include "game/GameState.h"
#include "ai/BasicMCTS.h"
#include "ai/AdvancedMCTS.h"

int main(int argc, char* argv[]) {
	if (argc < 5) {
		std::cout << "Please give 4 arguments: model1 path or the word basic, mcts1 simulations, model2 path or the word basic, mcts2 simulations" << '\n';
	}

	MCTS* mcts1;
	MCTS* mcts2;
	NeuralNetwork neuralNetwork1, neuralNetwork2;

	if (strlen(argv[1]) == 5 && strncmp(argv[1], "basic", 5) == 0) {
		mcts1 = new BasicMCTS(std::stoi(argv[2]));
	} else {
		neuralNetwork1.load(argv[1]);
		mcts1 = new AdvancedMCTS(&neuralNetwork1, std::stoi(argv[2]));
	}

	if (strlen(argv[3]) == 5 && strncmp(argv[3], "basic", 5) == 0) {
		mcts2 = new BasicMCTS(std::stoi(argv[4]));
	} else {
		neuralNetwork2.load(argv[3]);
		mcts2 = new AdvancedMCTS(&neuralNetwork1, std::stoi(argv[4]));
	}

	GameState* gameState = GameState::newGame('X', "#...#.....#.O...........#");
	gameState->printGameState();

	while (gameState->getEndState() < -1) {
		std::vector<float> moveProbabilities1 = mcts1->getMoveProbabilities(gameState);
		std::vector<float> moveProbabilities2 = mcts2->getMoveProbabilities(gameState);

		std::cout << std::left;
		std::cout << std::setw(3) << "i "
				  << std::setw(5) << "Move "
				  << std::setw(12) << "Prob 1"
				  << std::setw(12) << "Prob 2"
				  << std::setw(12) << "Value 1"
				  << std::setw(12) << "Value 2" << '\n';
		
		for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
			std::cout << std::setw(3) << i
					  << std::setw(5) << gameState->getValidMoves()->at(i)
					  << std::setw(12) << moveProbabilities1[gameState->getValidMoves()->at(i) + 1]
					  << std::setw(12) << moveProbabilities2[gameState->getValidMoves()->at(i) + 1]
					  << std::setw(12) << mcts1->getMoveValue(gameState->getChild(i))
					  << std::setw(12) << mcts2->getMoveValue(gameState->getChild(i)) << '\n';
		}

		int moveToPlay;
		std::cin >> moveToPlay;

		GameState* childGameState = gameState->getChild(moveToPlay, false);
		delete gameState;
		gameState = childGameState;

		gameState->printGameState();
		mcts1->reset();
		mcts2->reset();
	}

	std::cout << gameState->getEndState() << '\n';

	delete gameState;
	delete mcts1;
	delete mcts2;

	return 0;
}