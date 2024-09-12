#include "GameStateData.h"

std::vector<uint8_t> toVector(const GameState* gameState) {
	std::vector<uint8_t> data;
	data.reserve(GAME_STATE_DATA_LENGTH);
	const bool white = gameState->getColor() == 'O';
	const std::string* board = gameState->getBoard();
	const std::vector<std::string>* previousBoards = gameState->getPreviousBoards();
	for (unsigned int i = 0; i < GAME_STATE_DATA_SIZE[0]; i++) {
		for (unsigned int j = 0; j < GAME_STATE_DATA_SIZE[1]; j++) {
			for (unsigned int k = 0; k < GAME_STATE_DATA_SIZE[2]; k++) {
				if (i == 0) {
					//Current player's color
					data.push_back(white);
				} else if (i == 1) {
					//Blocked spaces
					data.push_back((*board)[j * SIDE_LENGTH + k] == '#');
				} else if (i % 2 == 0) {
					if (i / 2 == 1) {
						//Current board's white spaces
						data.push_back((*board)[j * SIDE_LENGTH + k] == 'O');
					} else if (i / 2 - 2 < previousBoards->size()) {
						//Previous board's white spaces
						data.push_back((*previousBoards)[previousBoards->size() - i / 2 + 1][j * SIDE_LENGTH + k] == 'O');
					} else {
						//Non existent
						data.push_back(false);
					}
				} else {
					if (i / 2 == 1) {
						//Current board's white spaces
						data.push_back((*board)[j * SIDE_LENGTH + k] == 'X');
					} else if (i / 2 - 2 < previousBoards->size()) {
						//Previous board's black spaces
						data.push_back((*previousBoards)[previousBoards->size() - i / 2 + 1][j * SIDE_LENGTH + k] == 'X');
					} else {
						//Non existent
						data.push_back(false);
					}
				}
			}
		}
	}
	
	return data;
}
