#include "AdvancedMCTS.h"

AdvancedMCTS::AdvancedMCTS(NeuralNetwork* neuralNetwork, const unsigned int simulations) : MCTS(simulations), neuralNetwork(neuralNetwork) {
}

float AdvancedMCTS::getMoveValue(const GameState* gameState) {
	auto const gameStateIter = stateInfos.find(gameState);
	if (gameStateIter != stateInfos.end()) {
		return gameStateIter->second.totalValue / gameStateIter->second.visits;
	}
	
	return -3;
}

std::vector<float> AdvancedMCTS::getMoveProbabilities(GameState* gameState) {
	addDirichletNoise(gameState);
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

unsigned int AdvancedMCTS::getBestMove(GameState* gameState) {
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

void AdvancedMCTS::reset() {
	stateInfos.clear();
}

float AdvancedMCTS::simulate(GameState* potentialLeaf) {
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
				//Child value ranges from 0 to 1
				float childValue;
				if (potentialLeaf->getColor() == 'O') {
					childValue = (1 + (childStateInfoIter->second.totalValue / childStateInfoIter->second.visits)) / 2;
				} else {
					childValue = (1 - (childStateInfoIter->second.totalValue / childStateInfoIter->second.visits)) / 2;
				}


				
				//Uses PUCT
				selectionScore = childValue + EXPLORATION_PARAMETER * leafStateInfoIter->second.validMoveProbabilities.at(i) * sqrtf(leafStateInfoIter->second.visits + 1) / (childStateInfoIter->second.visits + 1);
			} else {
				//Substitutes 0.5 for childValue
				selectionScore = 0.5f + EXPLORATION_PARAMETER * leafStateInfoIter->second.validMoveProbabilities.at(i) * sqrtf(leafStateInfoIter->second.visits + 1);
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

void AdvancedMCTS::runSimulations(GameState* gameState) {
	for (unsigned int i = 0; i < simulations; i++) {
		simulate(gameState);
	}
}

void AdvancedMCTS::addDirichletNoise(GameState* gameState) {
	auto stateInfoIter = stateInfos.find(gameState);
	if (stateInfoIter == stateInfos.end()) {
		//Evaluates leaf
		const std::pair<std::vector<float>, float> result = neuralNetwork->predict(gameState);

		StateInfo stateInfo;
		stateInfo.visits = 1;
		stateInfo.validMoveProbabilities = result.first;
		stateInfo.totalValue = result.second;
		stateInfoIter = stateInfos.emplace(gameState, stateInfo).first;
	}

	std::vector<float> dirichlet;
	dirichlet.resize(gameState->getValidMoves()->size());
	float sum = 0.0f;
	for (float& probability : dirichlet) {
		probability = static_cast<float>(gamma(rng));
		sum += probability;
	}

	for (int i = 0; i < dirichlet.size(); i++) {
		const float prior = stateInfoIter->second.validMoveProbabilities.at(i);
		stateInfoIter->second.validMoveProbabilities.at(i) = (1 - DIRICHLET_SCALAR) * prior + DIRICHLET_SCALAR * (dirichlet.at(i) / sum);
	}
}