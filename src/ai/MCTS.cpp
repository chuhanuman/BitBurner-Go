#include "MCTS.h"

MCTS::MCTS(NeuralNetwork* neuralNetwork, const unsigned int simulations) : neuralNetwork(neuralNetwork) {
	if (simulations < 1) {
		this->simulations = 1;
	} else {
		this->simulations = simulations;
	}
}

float MCTS::getMoveValue(const GameState* gameState) {
	auto const gameStateIter = stateInfos.find(gameState);
	if (gameStateIter != stateInfos.end()) {
		return gameStateIter->second.totalValue / gameStateIter->second.visits;
	}
	
	return -3;
}

std::vector<float> MCTS::getMoveProbabilities(GameState* gameState) {
	runSimulations(gameState);

	float totalSimulations = 1;
	if (stateInfos.find(gameState) != stateInfos.end()) {
		totalSimulations = stateInfos.at(gameState).visits;
	}

	std::vector<float> newMoveProbabilities;
	newMoveProbabilities.resize(NUM_MOVES);
	for (unsigned int i = 0; i < NUM_MOVES; i++) {
		newMoveProbabilities[i] = 0;
	}

	for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
		auto const childStateIter = stateInfos.find(gameState->getChild(i));
		if (childStateIter != stateInfos.end()) {
			newMoveProbabilities[gameState->getValidMoves()->at(i) + 1] = childStateIter->second.visits / totalSimulations;
		}
	}

	return newMoveProbabilities;
}

unsigned int MCTS::getBestMove(GameState* gameState) {
	runSimulations(gameState);

	float totalSimulations = 1;
	if (stateInfos.find(gameState) != stateInfos.end()) {
		totalSimulations = stateInfos.at(gameState).visits;
	}
	
	unsigned int bestMove = 0;
	float mostVisits = -1;
	for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
		float visits;
		auto const childStateIter = stateInfos.find(gameState->getChild(i));
		if (childStateIter != stateInfos.end()) {
			visits = childStateIter->second.visits / totalSimulations;
		} else {
			visits = 0;
		}

		if (visits > mostVisits) {
			mostVisits = visits;
			bestMove = i;
		}
	}

	return bestMove;
}

void MCTS::setSimulations(const unsigned int simulations) {
	this->simulations = simulations;
}

void MCTS::reset() {
	stateInfos.clear();
}

float MCTS::simulate(GameState* potentialLeaf) {
	float value = -1;
	
	auto leafStateInfoIter = stateInfos.find(potentialLeaf);
	const float endState = potentialLeaf->getEndState();
	if (leafStateInfoIter == stateInfos.end()) {
		//Evaluates leaf
		const std::pair<std::vector<float>, float> result = neuralNetwork->predict(potentialLeaf);
		value = result.second;

		StateInfo stateInfo;
		stateInfo.validMoveProbabilities = result.first;
		leafStateInfoIter = stateInfos.emplace(potentialLeaf, stateInfo).first;
	} else if (endState >= -1) {
		value = endState;
	} else {
		//Selects child to explore
		float bestSelectionScore = -1;
		unsigned int bestSelection = 0;
		for (unsigned int i = 0; i < potentialLeaf->getValidMoves()->size(); i++) {
			float selectionScore;

			auto childStateInfoIter = stateInfos.find(potentialLeaf->getChild(i));
			if (childStateInfoIter != stateInfos.end()) {
				float childValue;
				if (potentialLeaf->getColor() == 'O') {
					childValue = (1 + (childStateInfoIter->second.totalValue / childStateInfoIter->second.visits)) / 2;
				} else {
					childValue = (1 - (childStateInfoIter->second.totalValue / childStateInfoIter->second.visits)) / 2;
				}
				
				//Uses PUCT
				selectionScore = childValue + 1.4 * leafStateInfoIter->second.validMoveProbabilities.at(i) * sqrtf(leafStateInfoIter->second.visits + 1) / (childStateInfoIter->second.visits + 1);
			} else {
				//Substitutes 0.5 for childValue
				selectionScore = 0.5f + 1.4 * leafStateInfoIter->second.validMoveProbabilities.at(i) * sqrtf(leafStateInfoIter->second.visits + 1);
			}

			if (selectionScore > bestSelectionScore) {
				bestSelectionScore = selectionScore;
				bestSelection = i;
			}
		}

		GameState *child = potentialLeaf->getChild(bestSelection);
		value = simulate(child);
	}

	//Updates values
	leafStateInfoIter->second.visits++;
	leafStateInfoIter->second.totalValue += value;

	return value;
}

void MCTS::runSimulations(GameState* gameState) {
	for (unsigned int i = 0; i < simulations; i++) {
		simulate(gameState);
	}
}
