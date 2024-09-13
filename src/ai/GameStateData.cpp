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
						data.push_back(2);
					}
				} else {
					if (i / 2 == 1) {
						//Current board's black spaces
						data.push_back((*board)[j * SIDE_LENGTH + k] == 'X');
					} else if (i / 2 - 2 < previousBoards->size()) {
						//Previous board's black spaces
						data.push_back((*previousBoards)[previousBoards->size() - i / 2 + 1][j * SIDE_LENGTH + k] == 'X');
					} else {
						//Non existent
						data.push_back(2);
					}
				}
			}
		}
	}
	
	return data;
}

GameState* getGameState(const std::vector<uint8_t>& data) {
	const bool color = data.at(0);
	std::string board;
	for (unsigned int i = 0; i < AREA; i++) {
		board.push_back('.');
	}

	for (unsigned int i = 1; i <= 3; i++) {
		for (unsigned int j = 0; j < GAME_STATE_DATA_SIZE[1]; j++) {
			for (unsigned int k = 0; k < GAME_STATE_DATA_SIZE[2]; k++) {
				if (i == 1) {
					if (data.at(i * GAME_STATE_DATA_SIZE[0] * GAME_STATE_DATA_SIZE[1] + j * GAME_STATE_DATA_SIZE[1] + k)) {
						board.at(j * SIDE_LENGTH + k) = '#';
					}
				} else if (i == 2) {
					if (data.at(i * GAME_STATE_DATA_SIZE[0] * GAME_STATE_DATA_SIZE[1] + j * GAME_STATE_DATA_SIZE[1] + k)) {
						board.at(j * SIDE_LENGTH + k) = 'O';
					}
				} else if (i == 3) {
					if (data.at(i * GAME_STATE_DATA_SIZE[0] * GAME_STATE_DATA_SIZE[1] + j * GAME_STATE_DATA_SIZE[1] + k)) {
						board.at(j * SIDE_LENGTH + k) = 'X';
					}
				}
			}
		}
	}

	return GameState::newGame(color ? 'O' : 'X', board);
}
