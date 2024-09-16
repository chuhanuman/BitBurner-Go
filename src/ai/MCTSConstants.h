#ifndef MCTS_CONSTANTS_H
#define MCTS_CONSTANTS_H

/**
 * @brief Multiplied by the exploration term in the calculation for selection score, higher values encourage exploration
 */
constexpr float EXPLORATION_PARAMETER = 1.4f;
/**
 * @brief Used in creating a Dirichlet distribution, higher values encourage exploration
 */
constexpr float ALPHA = 1.0f;
/**
 * @brief Used to scale how much the Dirichlet distribution affects move probabilities, higher values encourage exploration
 */
constexpr float DIRICHLET_SCALAR = 0.25f;

#endif