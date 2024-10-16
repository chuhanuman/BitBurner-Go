#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <torch/torch.h>

#include <random>

#include "Net.h"
#include "Example.h"

#include "../game/GameState.h"

class NeuralNetwork {
public:
	/**
	 * @brief Runs given state through neural net and returns the results
	 * @param gameState game state to run through neural net  
	 * @return pairs with the move probabilities of the given board and the value of the given board
	 */
	std::pair<std::vector<float>, float> predict(const GameState* gameState);

	/**
	 * @brief Trains neural net on given examples using the given batch size
	 * @param examples vector of examples
	 * @param batchSize number of examples to include in each batch
	 */
	void train(std::vector<Example>& examples, int batchSize);

	/**
	 * @brief Loads neural net from file path and returns whether it was successful
	 * @param inputFilename file path to load neural net from 
	 * @return whether the neural net loaded successfully 
	 */
	bool load(const std::string& inputFilename);

	/**
	 * @brief Saves neural net to file path and returns whether it was successful
	 * @param outputFilename file path to save neural net to 
	 * @return whether the neural net saved successfully 
	 */
	bool save(const std::string& outputFilename) const;
private:
	/**
	 * @brief Neural net to run boards through
	 */
	NetImpl net;
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
