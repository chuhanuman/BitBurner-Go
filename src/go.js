/** @param {NS} ns */
export async function main(ns) {
    while (true) {
        ns.go.resetBoardState(ns.args[0], ns.args[1]);

        let endState = "temp";
        while (endState != "gameOver") {
            let boardStateString = "";
            for (const column of ns.go.getBoardState()) {
                boardStateString += column;
            }
            
            let gameStateString = ((ns.go.getCurrentPlayer() == "White") ? "O" : "X") + boardStateString;
            let response = await fetch("http://localhost:8080/", {
                method: "POST",
                headers: {
                    "Content-Type": "text/plain"
                },
                body: gameStateString
            });

            

            const index = parseInt(await response.json(), 10);
            const x = Math.floor(index / 5);
            const y = index % 5;
            
            if (index == -1) {
                endState = (await ns.go.passTurn())?.type;
            } else {
                endState = (await ns.go.makeMove(x, y))?.type;
            }

            let result = await ns.go.opponentNextTurn();
            endState = result?.type;
        }

        await ns.sleep(20);
    }
}