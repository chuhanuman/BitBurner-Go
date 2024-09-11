#ifndef GAME_STATE_DATA_H
#define GAME_STATE_DATA_H

#include "../game/GameState.h"
#include "../game/GameStateConstants.h"

/**
 * @brief Size of vector representing game state
 */
constexpr int GAME_STATE_DATA_SIZE[3] = {1 + 1 + 8 * 2, SIDE_LENGTH, SIDE_LENGTH};
/**
 * @brief Length of vector representing game state
 */
constexpr int GAME_STATE_DATA_LENGTH = GAME_STATE_DATA_SIZE[0] * GAME_STATE_DATA_SIZE[1] * GAME_STATE_DATA_SIZE[2];
	
/**
 * @brief Represents a game state as a vector
 * @param gameState game state to convert
 * @return vector representation of game state
 */
std::vector<uint8_t> toVector(const GameState* gameState);

#endif
