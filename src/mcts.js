let nsg;

const getNeighbors = (size, x, y) => {
    let neighbors = [];

    if (x > 0) {
        neighbors.push([x - 1, y]);
    }
    if (x < size - 1) {
        neighbors.push([x + 1, y]);
    }
    if (y > 0) {
        neighbors.push([x, y - 1]);
    }
    if (y < size - 1) {
        neighbors.push([x, y + 1]);
    }

    return neighbors;
}

const shuffleArray = (array) => {
    for (let i = array.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [array[i], array[j]] = [array[j], array[i]];
    }
}

class GameState {
    constructor(color, board, genMoves) {
        this.color = color;
        this.board = [];
        for (const column of board) {
            this.board.push([...column]);
        }
        this.previousBoards = [];
        this.size = this.board[0].length;
        this.score = -2; //Positive scores refer to white while negative scores refer to black, -2 means no score has been determined

        if (this.genMoves == undefined) {
            this.moves = this.getMoves();
        }
    }

    reverse(color) {
        return (color == "O") ? "X" : "O";
    }

    findLiberties(x, y, color) {
        let visitedBoard = [];
        for (let x = 0; x < this.size; x++) {
            visitedBoard.push([]);
            for (let y = 0; y < this.size; y++) {
                visitedBoard[x].push(false);
            }
        }
        visitedBoard[x][y] = true;

        let stack = getNeighbors(this.size, x, y);
        let liberties = 0;

        while (stack.length > 0) {
            let pos = stack.pop();

            if (visitedBoard[pos[0]][pos[1]]) {
                continue;
            }
            visitedBoard[pos[0]][pos[1]] = true;

            let cell = this.board[pos[0]][pos[1]];
            if (cell == ".") {
                liberties++;
            } else if (cell == color) {
                for (const neighbor of getNeighbors(this.size, pos[0], pos[1])) {
                    stack.push(neighbor);
                }
            }
        }

        return liberties;
    }

    findChain(board, x, y, color) {
        let visitedBoard = [];
        for (let x = 0; x < this.size; x++) {
            visitedBoard.push([]);
            for (let y = 0; y < this.size; y++) {
                visitedBoard[x].push(false);
            }
        }

        let stack = [[x, y]];
        let chain = [];
        let liberties = 0;

        while (stack.length > 0) {
            let pos = stack.pop();

            if (visitedBoard[pos[0]][pos[1]]) {
                continue;
            }
            visitedBoard[pos[0]][pos[1]] = true;

            let cell = board[pos[0]][pos[1]];
            if (cell == ".") {
                liberties++;
            } else if (cell == color) {
                chain.push(pos);
                for (const neighbor of getNeighbors(this.size, pos[0], pos[1])) {
                    stack.push(neighbor);
                }
            }
        }

        return [liberties, chain];
    }

    evaluateMove(x, y) {
        let newBoard = [];
        for (const column of this.board) {
            newBoard.push([...column]);
        }
        
        newBoard[x][y] = this.color;
        
        const enemyColor =  this.reverse(this.color);
        for (const neighbor of getNeighbors(this.size, x, y)) {
            if (newBoard[neighbor[0]][neighbor[1]] == enemyColor) {
                let [enemyLiberties, enemyChain] = this.findChain(newBoard, neighbor[0], neighbor[1], enemyColor);
                if (enemyLiberties == 0) {
                    for (const pos of enemyChain) {
                        newBoard[pos[0]][pos[1]] = ".";
                    }
                }
            }
        }

        return newBoard;
    }

    getMoves() {
        let validMoves = [[-1, -1]];

        for (let x = 0; x < this.size; x++) {
            for (let y = 0; y < this.size; y++) {
                if (this.board[x][y] == ".") {
                    const possibleRepeat = this.previousBoards.find((board) => (JSON.parse(board)[x][y] == this.color));
                    const neighbors = getNeighbors(this.size, x, y);

                    //Move is not suicide from empty spaces
                    const hasLiberty = neighbors.some((pos) => (this.board[pos[0]][pos[1]] == "."));
                    if (!possibleRepeat && hasLiberty) {
                        validMoves.push([x, y]);
                        continue;
                    }

                    //Move is not suicide from adjacent chain
                    const liberties = this.findLiberties(x, y, this.color);
                    if (!possibleRepeat && liberties != 0) {
                        validMoves.push([x, y]);
                        continue;
                    }

                    //Move is not suicide from capturing enemy chain
                    const enemyLiberties = this.findLiberties(x, y, this.reverse(this.color));
                    const adjacentEnemy = neighbors.some((pos) => (this.board[pos[0]][pos[1]] == this.reverse(this.color)));
                    if (!possibleRepeat && enemyLiberties == 0 && adjacentEnemy) {
                        validMoves.push([x, y]);
                        continue;
                    }
                    
                    //Move is suicide
                    if (!hasLiberty && liberties == 0 && (enemyLiberties != 0 || !adjacentEnemy)) {
                        continue;
                    }

                    //Move is not repeat
                    const resultBoard = this.evaluateMove(x, y);
                    if (!this.previousBoards.includes(JSON.stringify(resultBoard))) {
                        validMoves.push([x, y]);
                    }
                }
            }
        }

        return validMoves;
    }

    endGame() {
        let whitePoints = 0;
        let blackPoints = 0;

        let visitedBoard = [];
        for (let x = 0; x < this.size; x++) {
            visitedBoard.push([]);
            for (let y = 0; y < this.size; y++) {
                let cell = this.board[x][y];

                if (cell == ".") {
                    visitedBoard[x].push(false);
                } else {
                    visitedBoard[x].push(true);

                    if (cell == "O") {
                        whitePoints++;
                    } else if (cell == "X") {
                        blackPoints++;
                    }
                }
            }
        }

        //@ignore-infinite
        while (true) {
            let emptyX = -1;
            let emptyY = -1;
            for (let x = 0; x < this.size; x++) {
                for (let y = 0; y < this.size; y++) {
                    if (!visitedBoard[x][y] && this.board[x][y] == ".") {
                        emptyX = x;
                        emptyY = y;
                        break;
                    }
                }

                if (emptyX != -1) {
                    break;
                }
            }

            if (emptyX == -1) {
                break;
            }

            let stack = [[emptyX, emptyY]];
            let chainLength = 0;
            let hasWhite = false;
            let hasBlack = false;

            while (stack.length > 0) {
                let pos = stack.pop();

                let cell = this.board[pos[0]][pos[1]];
                if (cell == "O") {
                    hasWhite = true;
                } else if (cell == "X") {
                    hasBlack = true;
                }

                if (visitedBoard[pos[0]][pos[1]]) {
                    continue;
                }
                visitedBoard[pos[0]][pos[1]] = true;

                if (cell == ".") {
                    chainLength++;
                    for (const neighbor of getNeighbors(this.size, pos[0], pos[1])) {
                        stack.push(neighbor);
                    }
                }
            }

            if (hasWhite && !hasBlack) {
                whitePoints += chainLength;
            } else if (hasBlack && !hasWhite) {
                blackPoints += chainLength;
            }
        }

        this.score = (whitePoints - blackPoints) / (whitePoints + blackPoints);
    }

    makeMove(x, y) {
        if (x == -1 && this.previousBoards.includes(JSON.stringify(this.board))) {
            this.endGame();
        }

        this.previousBoards.push(JSON.stringify(this.board));
        if (x != -1) {
            this.board = this.evaluateMove(x, y);
        }

        this.color = this.reverse(this.color);

        this.moves = this.getMoves();
    }

    getChild(move) {
        let newGameState = new GameState(this.color, this.board, true);
        newGameState.previousBoards = [...this.previousBoards];
        newGameState.score = this.score;
        newGameState.makeMove(move[0], move[1]);

        return newGameState;
    }
}

const playout = (gameState) => {
    let score = gameState.score;
    if (score != -2) {
        return score;
    }

    let index = Math.floor(Math.random() * gameState.moves.length);

    return playout(gameState.getChild(gameState.moves[index]));
}

const simulate = (gameState, stateInfos, path = "") => {
    let value;
	
	let score = gameState.score;
	if (!stateInfos.has(path)) {
		//Evaluates leaf
		value = playout(gameState);
		
		stateInfos.set(path, [0, 0]); //Stores playouts and total value

        shuffleArray(gameState.moves);
	} else if (score != -2) {
		value = score;
	} else {
		//Selects child to explore
        let bestSelection = -1;
        let bestSelectionScore = -1;
        for (let i = 0; i < gameState.moves.length; i++) {
            if (!stateInfos.has(path.concat(",", i))) {
                bestSelection = i;
                break;
            }
            
            let stateInfo = stateInfos.get(path.concat(",", i));
            //Uses UCT for score
            let selectionScore = 1.5 * Math.sqrt(Math.log(stateInfos.get(path)[0]) / (stateInfo[0] + 1));
            if (gameState.color == "O") {
                selectionScore += (1 + stateInfo[1] / stateInfo[0]);
            } else {
                selectionScore += (1 - stateInfo[1] / stateInfo[0]);
            }
            if (selectionScore > bestSelectionScore) {
                bestSelectionScore = selectionScore;
                bestSelection = i;
            }
        }

		let child = gameState.getChild(gameState.moves[bestSelection]);

		value = simulate(child, stateInfos, path.concat(",",bestSelection));
	}

	//Updates values
    stateInfos.get(path)[0]++;
    stateInfos.get(path)[1] += value;

	return value;
}

const findMove = (gameState) => {
    if (gameState.moves.length == 0) {
        return [-1, -1];
    }

    let stateInfos = new Map();
    for (let i = 0; i < 1000; i++) {
        simulate(gameState, stateInfos);
    }

    let bestMove = 0;
    let bestValue = (gameState.color == "O") ? -1 : 1;
    for (let i = 0; i < gameState.moves.length; i++) {
        let stateInfo = stateInfos.get([i].join(","));
        if (stateInfo != undefined) {
            const value = stateInfo[1] / stateInfo[0];
            if ((gameState.color == "O" && value > bestValue) || (gameState.color == "X" && value < bestValue)) {
                bestValue = value;
                bestMove = i;
            }
        }
    }

    return gameState.moves[bestMove];
}

/** @param {NS} ns */
export async function main(ns) {
    nsg = ns;

    while (true) {
        ns.go.resetBoardState(ns.args[0], ns.args[1]);

        let gameState = new GameState((ns.go.getCurrentPlayer() == "White") ? "O" : "X", ns.go.getBoardState());

        let endState = "temp";
        while (endState != "gameOver") {
            const [x, y] = findMove(gameState);

            nsg.print(gameState.board);
            nsg.print(gameState.moves);
            
            gameState.makeMove(x, y);
            if (x == -1) {
                endState = (await ns.go.passTurn())?.type;
            } else {
                endState = (await ns.go.makeMove(x, y))?.type;
            }

            nsg.print(gameState.board);
            nsg.print(gameState.moves);

            let result = await ns.go.opponentNextTurn();
            endState = result?.type;

            if (endState == "move") {
                gameState.makeMove(result?.x, result?.y);
            } else {
                gameState.makeMove(-1, -1);
            }
            
        }

        ns.print(gameState.score);

        await ns.sleep(20);
    }
}