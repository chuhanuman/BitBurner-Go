#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <iostream>
#include <vector>

class Example {
public:
	/**
	 * @brief Loads examples from list of turns in a game each with a unique end value
	 * @param turnInformation vector of game states, move probabilities, and values for turns
	 * @return vector of examples
	 */
	static std::vector<Example> load(const std::vector<std::tuple<std::vector<uint8_t>, std::vector<float>, float>>& turnInformation);
	
	/**
	 * @brief Loads examples from list of turns in a game and an end value
	 * @param turnInformation vector of game states and move probabilities during a turn
	 * @param value value of the list of turns
	 * @return vector of examples
	 */
	static std::vector<Example> load(const std::vector<std::pair<std::vector<uint8_t>, std::vector<float>>>& turnInformation, float value);
	
	/**
	 * @brief Loads examples from stream
	 * @param in stream to load examples from
	 * @return vector of examples
	 */
	static std::vector<Example> load(std::istream& in);
	
	/**
	 * @brief Saves examples to stream
	 * @param out stream to write examples to
	 * @param examples vector of examples to save
	 */
	static void save(std::ostream& out, const std::vector<Example>& examples);

	/**
	 * @brief Returns game state data
	 * @return game state data
	 */
	std::vector<uint8_t>& getGameStateData();

	/**
	 * @brief Returns move probabilities as a vector with the probability of each move
	 * @return move probabilities
	 */
	std::vector<float> getMoveProbabilities();
	
	/**
	 * @brief Returns value of example
	 * @return value of example
	 */
	[[nodiscard]] float getValue() const;
private:
	/**
	 * @brief Game state in example in vector form
	 */
	std::vector<uint8_t> gameStateData;
	/**
	 * @brief Move probabilities for game state in the form {moveIndex, probability}
	 */
	std::vector<float> moveProbabilities;
	/**
	 * @brief Value of game state
	 */
	float value;
};

#endif
