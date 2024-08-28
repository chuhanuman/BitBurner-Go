#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <unordered_set>
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
	 * @return a new game state
	 */
	static GameState* newGame(char color, const std::string& board);

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
     * @brief Returns string version of game state made to be compact and unique not necessarily readable
     * @return string version of game state
     */
	[[nodiscard]] std::string getKey() const;
	
	/**
	 * @brief Prints the game state to console
	 */
	void printGameState() const;
private:
	//Prevents constructor from being used outside GameState and subclasses
	GameState() = default;

	/**
	 * @brief Returns whether the given chain is surrounded
	 * @param startIndex index of member of chain
	 * @param ignoreIndex index of cell to ignore (very useful, trust me)
	 * @return whether the given chain is surrounded
	 */
	[[nodiscard]] bool isSurrounded(int startIndex, int ignoreIndex) const;

	/**
	 * @brief Checks if the chain at the given index has no liberties and if it does destroys it
	 * @param board board to check
	 * @param index index of member of chain
	 */
	void attemptDestroyChain(std::string& board, int index) const;

	/**
	 * @brief Returns child game state based on playing a given move
	 * @param move valid move to play
	 * @return child game state
	 */
	[[nodiscard]] GameState* makeMove(int move) const;

	/**
	 * @brief Returns a list of a given cell's neighbors' indices
	 * @param index index of cell
	 * @return list of neighboring cells' indices
	 */
	[[nodiscard]] std::vector<int> getNeighbors(int index) const;
	
	/**
	 * @brief Places piece of current color at index and returns resulting board
	 * @param index position to place piece
	 * @return Board resulting from placing a piece
	 */
	[[nodiscard]] std::string placePiece(int index) const;

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
	 * @brief Previous game board
	 */
	std::unordered_set<std::string> previousBoards;
	/**
	 * @brief Whether the last move was a pass
	 */
	bool passed;
	/**
	 * @brief Current player's (player who makes the next move) color 
	 */
	char color;
	/**
	 * @brief Board side length
	 */
	int size;
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