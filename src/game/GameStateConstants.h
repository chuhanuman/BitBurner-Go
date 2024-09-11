#ifndef GAME_STATE_CONSTANTS_H
#define GAME_STATE_CONSTANTS_H

#include <vector>

/**
 * @brief Board side length
 */
constexpr unsigned int SIDE_LENGTH = 5;
/**
 * @brief Board area
 */
constexpr unsigned int AREA = SIDE_LENGTH * SIDE_LENGTH;
/**
 * @brief Number of possible moves
 */
constexpr unsigned int NUM_MOVES = AREA + 1;

/**
 * @brief Returns an array with a list of neighbors for each cell
 * @return an array with a list of neighbors for each cell
 */
inline std::vector<unsigned int>* getNeighbors() {
	const auto allNeighbors = new std::vector<unsigned int>[AREA];

	for (unsigned int index = 0; index < AREA; index++) {
		if (index >= SIDE_LENGTH) {
			allNeighbors[index].push_back(index - SIDE_LENGTH);
		}
		if (index < static_cast<int>(AREA) - SIDE_LENGTH) {
			allNeighbors[index].push_back(index + SIDE_LENGTH);
		}
		if (index % SIDE_LENGTH > 0) {
			allNeighbors[index].push_back(index - 1);
		}
		if (index % SIDE_LENGTH < SIDE_LENGTH - 1) {
			allNeighbors[index].push_back(index + 1);
		}
	}

	return allNeighbors;
}

/**
 * @brief Neighbors array
 */
const std::vector<unsigned int>* NEIGHBORS = getNeighbors();

#endif