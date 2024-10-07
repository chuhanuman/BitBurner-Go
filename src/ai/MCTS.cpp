#include "MCTS.h"

MCTS::MCTS(const unsigned int simulations) {
	if (simulations < 1) {
		this->simulations = 1;
	} else {
		this->simulations = simulations;
	}
}

void MCTS::setSimulations(const unsigned int simulations) {
	this->simulations = simulations;
}