#ifndef MCTS_H
#define MCTS_H

#include <random>
#include <unordered_map>
#include <vector>

#include "GameState.h"

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
	float getMoveValue(const GameState* gameState);

	/**
	 * @brief Runs simulations on given game state and returns the best move which is the move with the highest average value
	 * @param gameState game state to start simulations on
	 * @return index of best move
	 */
	unsigned int getBestMove(GameState* gameState);

	/**
	 * @brief Sets the number of simulations with a minimum of 1
	 * @param simulations number of simulations to run each time
	 */
	void setSimulations(unsigned int simulations);

	/**
	 * @brief Resets MCTS tree
	 */
	void reset();
private:
	struct StateInfo {
		/**
		 * @brief The number of playouts run by the corresponding game state
		 */
		float playouts = 0;
		/**
		 * @brief The total value from all of the simulations that went past the corresponding game state
		 */
		float totalValue = 0;
	};
	
	/**
	 * @brief Recursively looks for unexplored game state using game state's selection score, plays it out, then updates values based on the simulation result
	 * @param potentialLeaf game state to simulate
	 * @return final value of simulation
	 */
	float simulate(GameState* potentialLeaf);
	
	/**
	 * @brief Plays out given game state to its end
	 * @param gameState game state to play out
	 * @return end state value
	 */
	static float playout(GameState* gameState);

	/**
	 * @brief Runs simulations on given game state
	 * @param gameState game state to start simulations on
	 */
	void runSimulations(GameState* gameState);
	
	/**
	 * @brief The number of simulations to perform each time MCTS is run
	 */
	unsigned int simulations;
	/**
	 * @brief Maps game state keys to their state information
	 */
	std::unordered_map<const GameState*, StateInfo> stateInfos;
	/**
     * @brief Used to seed rng
     */
	inline static std::random_device seeder;
	/** 
	 * @brief Used to generate random unsigned 64 bit integers
	 */
	inline static std::mt19937_64 rng = std::mt19937_64(seeder());
};

#endif