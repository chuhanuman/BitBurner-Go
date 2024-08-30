#include "GameState.h"

#include <algorithm>
#include <iostream>

GameState* GameState::newGame(const char color, const std::string& board, const std::vector<std::string>& previousBoards) {
	const auto gameState = new GameState;
	gameState->color = color;
	gameState->board = board;
	gameState->passed = false;
	area = static_cast<unsigned int>(board.size());
	sideLength = static_cast<int>(sqrt(area));

	neighbors.clear();
	neighbors.reserve(area);
	for (unsigned int i = 0; i < area; i++) {
		neighbors.push_back(getNeighbors(i));
	}

	gameState->previousBoards.reserve(previousBoards.size());
	for (const std::string& previousBoard : previousBoards) {
		gameState->previousBoards.emplace(previousBoard);
	}

	return gameState;
}

char GameState::flipColor(const char color) {
	return (color == 'O') ? 'X' : 'O';
}

GameState* GameState::getChild(const unsigned int moveNum, const bool keepChild) {
	if (keepChild) {
		if (children[moveNum] == nullptr) {
			children[moveNum] = makeMove(validMoves[moveNum]);
		}

		return children[moveNum];
	}
	
	if (children[moveNum] != nullptr) {
		GameState* child = children[moveNum];
		children[moveNum] = nullptr;
		
		return child;
	}
	
	return makeMove(validMoves[moveNum]);
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

		const bool possibleRepeat = std::any_of(previousBoards.begin(), previousBoards.end(),
			[this, move](auto& previousBoard){return previousBoard[move] == color;});

		//Move is not suicide from adjacent spaces
		const bool hasAdjacentLiberty = std::any_of(neighbors[move].begin(), neighbors[move].end(), 
			[this](const int neighbor){return board[neighbor] == '.';});
        if (!possibleRepeat && hasAdjacentLiberty) {
            return true;
        }

        //Move is not suicide from adjacent chain
		const bool hasChainLiberty = std::any_of(neighbors[move].begin(), neighbors[move].end(), 
			[this, move](const int neighbor){return board[neighbor] == color && !isSurrounded(neighbor, move);});
        if (!possibleRepeat && hasChainLiberty) {
            return true;
        }

		//Move is not suicide from killing enemy chain
		const bool hasKillLiberty = std::any_of(neighbors[move].begin(), neighbors[move].end(), 
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
		if (previousBoards.find(resultBoard) == previousBoards.end()) {
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
	for (int y = static_cast<int>(sideLength) - 1; y >= 0; y--) {
		for (int x = 0; x < static_cast<int>(sideLength); x++) {
			std::cout << board[x * sideLength + y];
		}
		std::cout << '\n';
	}
}

bool GameState::isSurrounded(const unsigned int startIndex, const unsigned int ignoreIndex) const {
	auto const visitedBoard = new bool[area];
	for (unsigned int i = 0; i < area; i++) {
		visitedBoard[i] = false;
	}
	visitedBoard[ignoreIndex] = true;

	const char color = board[startIndex];
    std::vector<unsigned int> stack = {startIndex};

    while (!stack.empty()) {
        const unsigned int curIndex = stack.back();
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
            for (const unsigned int neighbor : neighbors[curIndex]) {
                stack.push_back(neighbor);
            }
        }
    }

	delete[] visitedBoard;
	return true;
}

void GameState::attemptDestroyChain(std::string& board, const unsigned int index) const {
	auto const visitedBoard = new bool[area];
	for (unsigned int i = 0; i < area; i++) {
		visitedBoard[i] = false;
	}

	const char color = board[index];
    std::vector<unsigned int> stack = {index};
    std::vector<unsigned int> chain = {};

    while (!stack.empty()) {
        const unsigned int curIndex = stack.back();
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
            for (const unsigned int neighbor : neighbors[curIndex]) {
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
	child->previousBoards = previousBoards;
	child->previousBoards.emplace(board);
	if (move != -1) {
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

std::vector<unsigned int> GameState::getNeighbors(const unsigned int index) {
	std::vector<unsigned int> neighbors;

	if (index >= sideLength) {
		neighbors.push_back(index - sideLength);
	}
	if (index < static_cast<int>(area) - sideLength) {
		neighbors.push_back(index + sideLength);
	}
	if (index % sideLength > 0) {
		neighbors.push_back(index - 1);
	}
	if (index % sideLength < sideLength - 1) {
		neighbors.push_back(index + 1);
	}

	return neighbors;
}

std::string GameState::placePiece(const unsigned int index) const {
	std::string newBoard = board;
	newBoard[index] = color;

	const char enemyColor = flipColor(color);
	for (const unsigned int neighbor : neighbors[index]) {
		if (newBoard[neighbor] == enemyColor) {
			attemptDestroyChain(newBoard, neighbor);
		}
	}

	return newBoard;
}

void GameState::setup() {
	for (int i = -1; i < static_cast<int>(area); i++) {
		if (isValid(i)) {
			validMoves.push_back(i);
			children.push_back(nullptr);
		}
	}

	setupMoves = true;
}

void GameState::endGame() {
	float whitePoints = 0, blackPoints = 0;

    auto const visitedBoard = new bool[area];
	for (unsigned int i = 0; i < area; i++) {
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
        for (unsigned int i = 0; i < area; i++) {
            if (!visitedBoard[i]) {
                emptyIndex = static_cast<int>(i);
                break;
            }
        }

        if (emptyIndex == -1) {
            break;
        }

        std::vector<unsigned int> stack = {static_cast<unsigned int>(emptyIndex)};
        float chainLength = 0;
        bool whiteControl = false;
        bool blackControl = false;

        while (!stack.empty()) {
            const unsigned int curIndex = stack.back();
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
                for (const unsigned int neighbor : neighbors[curIndex]) {
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
