#include "GameState.h"

#include <algorithm>
#include <iostream>

GameState* GameState::newGame(const char color, const std::string& board) {
	const auto gameState = new GameState;
	gameState->color = color;
	gameState->board = board;
	gameState->passed = false;
	gameState->size = static_cast<int>(sqrt(board.size()));

	return gameState;
}

char GameState::flipColor(const char color) {
	return (color == 'O') ? 'X' : 'O';
}

GameState* GameState::getChild(const unsigned int moveNum, const bool keepChild) {
	if (keepChild) {
		if (children.at(moveNum) == nullptr) {
			children.at(moveNum) = makeMove(validMoves.at(moveNum));
		}

		return children.at(moveNum);
	}
	
	if (children.at(moveNum) != nullptr) {
		GameState* child = children.at(moveNum);
		children.at(moveNum) = nullptr;
		
		return child;
	}
	
	return makeMove(validMoves.at(moveNum));
}

std::vector<int>* GameState::getValidMoves() {
	if (!setupMoves) {
		setup();
	}

	return &validMoves;
}

bool GameState::isValid(const int move) {
	if (!setupMoves) {
		//Pass is always valid
		if (move == -1) {
			return true;
		}

		//Cannot move onto occupied cell
		if (board[move] != '.') {
			return false;
		}

        const bool possibleRepeat = previousBoard[move] == color;
		const std::vector<int> neighbors = getNeighbors(move);

		//Move is not suicide from adjacent spaces
		const bool hasAdjacentLiberty = std::any_of(neighbors.begin(), neighbors.end(), 
			[this](const int neighbor){return board[neighbor] == '.';});
        if (!possibleRepeat && hasAdjacentLiberty) {
            return true;
        }

        //Move is not suicide from adjacent chain
		const bool hasChainLiberty = std::any_of(neighbors.begin(), neighbors.end(), 
			[this, move](const int neighbor){return board[neighbor] == color && !isSurrounded(neighbor, move);});
        if (!possibleRepeat && hasChainLiberty) {
            return true;
        }

		//Move is not suicide from killing enemy chain
		const bool hasKillLiberty = std::any_of(neighbors.begin(), neighbors.end(), 
			[this, move](const int neighbor){return board[neighbor] == flipColor(color) && isSurrounded(neighbor, move);});
        if (!possibleRepeat && hasKillLiberty) {
            return true;
        }

		//Move is suicide
		if (!hasAdjacentLiberty && !hasChainLiberty && !hasKillLiberty) {
			return false;
		}

		//Move is not repeat
		const std::string resultBoard = placePiece(move);
		if (resultBoard != previousBoard) {
			return true;
		}
        
        return false;
	} else {
		return std::find(validMoves.begin(), validMoves.end(), move) != validMoves.end();
	}
}

float GameState::getEndState() const {
	return endState;
}

char GameState::getColor() const {
	return color;
}

std::string GameState::getKey() const {
	return board + (passed ? 'T' : 'F');
}

void GameState::printGameState() const {
	for (int y = size - 1; y >= 0; y--) {
		for (int x = 0; x < size; x++) {
			std::cout << board[x * size + y];
		}
		std::cout << '\n';
	}
}

bool GameState::isSurrounded(int startIndex, int ignoreIndex) const {
	auto const visitedBoard = new bool[board.size()];
	for (unsigned int i = 0; i < board.size(); i++) {
		visitedBoard[i] = false;
	}
	visitedBoard[ignoreIndex] = true;

	const char color = board.at(startIndex);
    std::vector<int> stack = {startIndex};

    while (!stack.empty()) {
        const int curIndex = stack.back();
		stack.pop_back();

        if (visitedBoard[curIndex]) {
            continue;
        }
        visitedBoard[curIndex] = true;

        if (board[curIndex] == '.') {
			//Chain is not surrounded
			delete[] visitedBoard;
            return false;
        } else if (board[curIndex] == color) {
            for (const int neighbor : getNeighbors(curIndex)) {
                stack.push_back(neighbor);
            }
        }
    }

	delete[] visitedBoard;
	return true;
}

void GameState::attemptDestroyChain(std::string& board, const int index) const {
	auto const visitedBoard = new bool[board.size()];
	for (unsigned int i = 0; i < board.size(); i++) {
		visitedBoard[i] = false;
	}

	const char color = board.at(index);
    std::vector<int> stack = {index};
    std::vector<int> chain = {};

    while (!stack.empty()) {
        const int curIndex = stack.back();
		stack.pop_back();

        if (visitedBoard[curIndex]) {
            continue;
        }
        visitedBoard[curIndex] = true;

        if (board[curIndex] == '.') {
			//Chain cannot be destroyed
			delete[] visitedBoard;
            return;
        } else if (board[curIndex] == color) {
            chain.push_back(curIndex);
            for (const int neighbor : getNeighbors(curIndex)) {
                stack.push_back(neighbor);
            }
        }
    }

    for (const int enemyIndex : chain) {
        board[enemyIndex] = '.';
    }

	delete[] visitedBoard;
}

GameState::~GameState() {
	for (const GameState* child : children) {
		delete child;
	}
}

GameState* GameState::makeMove(const int move) const {
	auto const child = new GameState;

	child->color = flipColor(color);
	child->size = size;
	if (move != -1) {
		child->previousBoard = board;
		child->board = placePiece(move);
		child->passed = false;
    } else {
		child->board = board;
	    child->passed = true;

		if (passed) {
			child->endGame();
		}
    }

	return child;
}

std::vector<int> GameState::getNeighbors(const int index) const {
	std::vector<int> neighbors;

	if (index >= size) {
		neighbors.push_back(index - size);
	}
	if (index < static_cast<int>(board.size()) - size) {
		neighbors.push_back(index + size);
	}
	if (index % size > 0) {
		neighbors.push_back(index - 1);
	}
	if (index % size < size - 1) {
		neighbors.push_back(index + 1);
	}

	return neighbors;
}

std::string GameState::placePiece(const int index) const {
	std::string newBoard = board;
	newBoard.at(index) = color;

	const char enemyColor = flipColor(color);
	for (const int neighbor : getNeighbors(index)) {
		if (newBoard.at(neighbor) == enemyColor) {
			attemptDestroyChain(newBoard, neighbor);
		}
	}

	return newBoard;
}

void GameState::setup() {
	for (int i = -1; i < static_cast<int>(board.size()); i++) {
		if (isValid(i)) {
			validMoves.push_back(i);
			children.push_back(nullptr);
		}
	}

	setupMoves = true;
}

void GameState::endGame() {
	float whitePoints = 0, blackPoints = 0;

    auto const visitedBoard = new bool[board.size()];
	for (unsigned int i = 0; i < board.size(); i++) {
		if (board[i] == '.') {
			visitedBoard[i] = false;
		} else {
			visitedBoard[i] = true;

			if (board[i] == 'O') {
	            whitePoints++;
	        } else if (board[i] == 'X') {
	            blackPoints++;
	        }
		}
	}

	if (whitePoints <= 0 && blackPoints <= 0) {
		endState = 0;

		delete[] visitedBoard;
		return;
	}
    
    while (true) {
		int emptyIndex = -1;
        for (unsigned int i = 0; i < board.size(); i++) {
            if (!visitedBoard[i]) {
                emptyIndex = static_cast<int>(i);
                break;
            }
        }

        if (emptyIndex == -1) {
            break;
        }

        std::vector<int> stack = {emptyIndex};
        float chainLength = 0;
        bool whiteControl = false;
        bool blackControl = false;

        while (!stack.empty()) {
            const int curIndex = stack.back();
			stack.pop_back();

            if (board[curIndex] == 'O') {
                whiteControl = true;
				continue;
            } else if (board[curIndex] == 'X') {
                blackControl = true;
				continue;
            }

            if (visitedBoard[curIndex]) {
                continue;
            }
            visitedBoard[curIndex] = true;

            if (board[curIndex] == '.') {
                chainLength++;
                for (const int neighbor : getNeighbors(curIndex)) {
                    stack.push_back(neighbor);
                }
            }
        }

        if (whiteControl && !blackControl) {
            whitePoints += chainLength;
        } else if (blackControl && !whiteControl) {
            blackPoints += chainLength;
        }
    }

    endState = (whitePoints - blackPoints) / (whitePoints + blackPoints);

	delete[] visitedBoard;
}
