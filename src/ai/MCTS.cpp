#include "MCTS.h"

MCTS::MCTS(const unsigned int simulations) {
	if (simulations < 1) {
		this->simulations = 1;
	} else {
		this->simulations = simulations;
	}
}

float MCTS::getMoveValue(const GameState* gameState) {
	auto const gameStateIter = stateInfos.find(gameState);
	if (gameStateIter != stateInfos.end()) {
		return gameStateIter->second.totalValue / gameStateIter->second.playouts;
	}
	
	return 0;
}

unsigned int MCTS::getBestMove(GameState* gameState) {
	runSimulations(gameState);
	
	unsigned int bestMove = 0;
	float bestMoveValue = -1;
	for (unsigned int i = 0; i < gameState->getValidMoves()->size(); i++) {
		float moveValue = getMoveValue(gameState->getChild(i));
		if (gameState->getColor() == 'X') {
			moveValue = 1 - moveValue;
		}

		if (moveValue > bestMoveValue) {
			bestMoveValue = moveValue;
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
	float value;
	
	auto leafStateInfoIter = stateInfos.find(potentialLeaf);
	const float endState = potentialLeaf->getEndState();
	if (leafStateInfoIter == stateInfos.end()) {
		//Evaluates leaf
		value = playout(potentialLeaf);
		
		StateInfo stateInfo;
		leafStateInfoIter = stateInfos.emplace(potentialLeaf, stateInfo).first;
		
		//Shuffles moves because the first unexplored move is always selected first
		std::shuffle(potentialLeaf->getValidMoves()->begin(), potentialLeaf->getValidMoves()->end(), rng);
	} else if (endState >= -1) {
		value = endState;
	} else {
		//Selects child to explore
		float bestSelectionScore = -1;
		unsigned int bestSelection = 0;
		for (unsigned int i = 0; i < potentialLeaf->getValidMoves()->size(); i++) {
			auto childStateInfoIter = stateInfos.find(potentialLeaf->getChild(i));
			if (childStateInfoIter == stateInfos.end()) {
				bestSelection = i;
				break;
			}
			
			const StateInfo* childStateInfo = &childStateInfoIter->second;
			//Uses UCT for score
			float selectionScore = 1.4f * sqrtf(log(leafStateInfoIter->second.playouts) / (childStateInfo->playouts + 1));
			if (potentialLeaf->getColor() == 'O') {
				selectionScore += (1 + (childStateInfo->totalValue / childStateInfo->playouts)) / 2;
			} else {
				selectionScore += (1 - (childStateInfo->totalValue / childStateInfo->playouts)) / 2;
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
	leafStateInfoIter->second.playouts++;
	leafStateInfoIter->second.totalValue += value;

	return value;
}

float MCTS::playout(GameState* gameState) {
	const float endState = gameState->getEndState();
	if (endState >= -1) {
		return endState;
	}

	std::uniform_int_distribution<int> distribution(0, static_cast<int>(gameState->getValidMoves()->size() - 1));
	GameState* child = gameState->getChild(distribution(rng), false);
	const float value = playout(child);
	
	delete child;
	return value;
}

void MCTS::runSimulations(GameState* gameState) {
	for (unsigned int i = 0; i < simulations; i++) {
		simulate(gameState);
	}
}
