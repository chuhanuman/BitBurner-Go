#ifndef MCTS_H
#define MCTS_H

#include "../game/GameState.h"

class MCTS {
public:
	/**
	 * @brief Constructs a new MCTS object with the given number of simulations with a minimum of 1
	 * @param simulations number of simulations to run each time
	 */
	explicit MCTS(unsigned int simulations);

	/**
	 * @brief Returns the average value of a game state
	 * @param gameState game state to evaluate
	 * @return the average value of a game state
	 */
	virtual float getMoveValue(const GameState* gameState) = 0;

	/**
	 * @brief Runs simulations on given game state and returns move probabilities corresponding to the number of times each move was visited
	 * @param gameState game state to start simulations on
	 * @return list of probabilities for each move
	 */
	virtual std::vector<float> getMoveProbabilities(GameState* gameState) = 0;

	/**
	 * @brief Runs simulations on given game state and returns the best move
	 * @param gameState game state to start simulations on
	 * @return index of best move
	 */
	virtual unsigned int getBestMove(GameState* gameState) = 0;

	/**
	 * @brief Sets the number of simulations with a minimum of 1
	 * @param simulations number of simulations to run each time
	 */
	void setSimulations(unsigned int simulations);

	/**
	 * @brief Resets MCTS tree
	 */
	virtual void reset() = 0;
protected:
	/**
	 * @brief The number of simulations to perform each time MCTS is run
	 */
	unsigned int simulations;
};

#endif