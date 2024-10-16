#ifndef ADVANCED_MCTS_H
#define ADVANCED_MCTS_H

#include <random>
#include <unordered_map>

#include "MCTSConstants.h"
#include "NeuralNetwork.h"

#include "MCTS.h"

class AdvancedMCTS : public MCTS {
public:
	/**
	 * @brief Constructs a new AdvancedMCTS object with the given number of simulations with a minimum of 1
	 * @param neuralNetwork neural network used to predict probabilities and value of game states
	 * @param simulations number of simulations to run each time
	 */
	explicit AdvancedMCTS(NeuralNetwork* neuralNetwork, unsigned int simulations);
	
	/**
	 * @brief Returns the average value of a game state
	 * @param gameState game state to evaluate
	 * @return the average value of a game state
	 */
	float getMoveValue(const GameState* gameState) override;

	/**
	 * @brief Runs simulations on given game state and returns move probabilities corresponding to the number of times each move was visited
	 * @param gameState game state to start simulations on
	 * @return list of probabilities for each move
	 */
	std::vector<float> getMoveProbabilities(GameState* gameState) override;

	/**
	 * @brief Runs simulations on given game state and returns the best move which is the move visited the most
	 * @param gameState game state to start simulations on
	 * @return index of best move
	 */
	unsigned int getBestMove(GameState* gameState) override;

	/**
	 * @brief Resets MCTS tree
	 */
	void reset() override;
private:
	struct StateInfo {
		/**
		 * @brief Holds the probabilities of making each valid move given by a neural network normalized so that the probabilities add up to one
		 */
		std::vector<float> validMoveProbabilities;
		/**
		 * @brief The number of times the corresponding game state was visited by a parent
		 */
		float visits = 0.0f;
		/**
		 * @brief The total value from all of the simulations that went past the corresponding game state
		 */
		float totalValue = 0.0f;
	};
	
	/**
	 * @brief Recursively looks for unexplored game state using game state's selection score, plays it out, then updates values based on the simulation result
	 * @param potentialLeaf game state to simulate
	 * @return final value of simulation
	 */
	float simulate(GameState* potentialLeaf);

	/**
	 * @brief Runs simulations on given game state
	 * @param gameState game state to start simulations on
	 */
	void runSimulations(GameState* gameState);

	/**
	 * @brief Adds dirichlet noise to game state's move probabilities
	 * @param gameState game state to add noise to
	 */
	void addDirichletNoise(GameState* gameState);

	/**
	 * @brief The neural network used to predict the value and move probabilities of game boards
	 */
	NeuralNetwork* neuralNetwork;
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
	/**
	 * @brief Used to generate elements in dirichlet distribution
	 */
	inline static std::gamma_distribution<> gamma = std::gamma_distribution<>(ALPHA, 1);
};

#endif