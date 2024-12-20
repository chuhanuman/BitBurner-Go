#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <random>
#include <string>
#include <vector>

class GameState {
public:
	//Prevents copying/moving game states
	GameState(const GameState& other) = delete;
	GameState& operator=(const GameState& other) = delete;
	GameState(const GameState&& other) = delete;
	GameState& operator=(const GameState&& other) = delete;

	/**
	 * @brief Clears up children generated
	 */
	~GameState();
	
	/**
	 * @brief Returns a game state created from the given parameters
	 * @param color color of current player
	 * @param board current board state
	 * @param previousBoards list of previous board states
	 * @return a new game state
	 */
	static GameState* newGame(char color, const std::string& board,  const std::vector<std::string>& previousBoards = {});

	/**
	 * @brief Returns a game board with random elements
	 * @param rng random number generator to use
	 * @return a game board with random elements
	 */
	static std::string getRandomBoard(std::mt19937_64& rng);

	/**
	 * @brief Returns the opposite of the color given
	 * @param color color to flip
	 * @return the color opposite the given one
	 */
	inline static char flipColor(char color);

	/**
	 * @brief Returns child game state based on playing a given move
	 * @param moveNum index of move to play in valid moves
	 * @param keepChild whether the child should be kept (and deleted) by the GameState
	 * @return child game state
	 */
	GameState* getChild(unsigned int moveNum, bool keepChild = true);
	
	/**
	 * @brief Returns a vector with all the valid moves
	 * @return list of valid moves
	 */
	std::vector<int>* getValidMoves();
	
	/**
	 * @brief Checks whether a move is valid
	 * @param move possible move
	 * @return whether the move is valid
	 */
	bool isValid(int move);
	
	/**
	 * @brief Returns the end state of this object 
	 * @return end state of this object
	 */
	[[nodiscard]] float getEndState() const;
	
	/**
	 * @brief Returns the color of next player to make a move
	 * @return color of player making next move
	 */
	[[nodiscard]] char getColor() const;

	/**
	 * @brief Returns the current board
	 * @return current board
	 */
	[[nodiscard]] const std::string* getBoard() const;

	/**
	 * @brief Returns a vector with previous boards
	 * @return a vector with previous boards
	 */
	[[nodiscard]] const std::vector<std::string>* getPreviousBoards() const;
	
	/**
	 * @brief Prints the game state to console
	 */
	void printGameState() const;

	/**
	 * @brief Overrides << operator for output streams and prints game state
	 * @param out output stream to use
	 * @param gameState game state to print
	 * @return output stream
	 */
	friend std::ostream& operator<<(std::ostream& out, const GameState& gameState);
private:
	//Prevents constructor from being used outside GameState and subclasses
	GameState() = default;

	/**
	 * @brief Returns whether the given chain is surrounded
	 * @param startIndex index of member of chain
	 * @param ignoreIndex index of cell to ignore (very useful, trust me)
	 * @return whether the given chain is surrounded
	 */
	[[nodiscard]] bool isSurrounded(unsigned int startIndex, unsigned int ignoreIndex) const;

	/**
	 * @brief Checks if the chain at the given index has no liberties and if it does destroys it
	 * @param board board to check
	 * @param index index of member of chain
	 */
	static void attemptDestroyChain(std::string& board, unsigned int index);

	/**
	 * @brief Returns child game state based on playing a given move
	 * @param move valid move to play
	 * @return child game state
	 */
	[[nodiscard]] GameState* makeMove(int move) const;
	
	/**
	 * @brief Places piece of current color at index and returns resulting board
	 * @param index position to place piece
	 * @return Board resulting from placing a piece
	 */
	[[nodiscard]] std::string placePiece(const unsigned int index) const;

	/**
	 * @brief Populates validMoves and children
	 */
	void setup();

	/**
	 * @brief Calculates the final score
	 */
	void endGame();

	/**
	 * @brief Game board with X marking black, O marking white, . marking empty, and # marking a wall
	 */
	std::string board;
	/**
	 * @brief Previous game boards
	 */
	std::vector<std::string> previousBoards;
	/**
	 * @brief Whether the last move was a pass
	 */
	bool passed;
	/**
	 * @brief Current player's (player who makes the next move) color 
	 */
	char color;
	/**
	 * @brief End state of game with -1 to 1 referring to the score of a game with 1 being a white wipe and -1 being a black wipe, any number less than -1 means the game isn't over
	 */
	float endState = -2;
	/**
	 * @brief Whether valid moves has been populated
	 */
	bool setupMoves = false;
	/**
	 * @brief List of valid moves
	 */
	std::vector<int> validMoves;
	/**
	 * @brief Children game states
	 */
	std::vector<GameState*> children;
};

#endif