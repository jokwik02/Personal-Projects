import time
import sys
from OthelloPosition import OthelloPosition
from OthelloAction import OthelloAction
from AlphaBeta import AlphaBeta
from MatrixEvaluator import MatrixEvaluator
from TimeoutException import TimeoutException
"""
Main program for Othello that starts the game. It uses 
Iterative Deepening Search with the time limit argument.
    
Author: Arvid Nordsvan/Joakim Wiksten

Args:
    arg1: Position to evaluate (a string of length 65 representing the board).
    arg2: Time limit in seconds
"""
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Error: expected 2 arguments: <position_string(65)> <time_limit_seconds>", file=sys.stderr)
        sys.exit(1)
    
    posString = sys.argv[1]
    try:
        time_limit = float(sys.argv[2])
    except ValueError:
        print("Error: time_limit must be a number (seconds).", file=sys.stderr)
        sys.exit(1)
        
    if len(posString) != 65:
        print(f"Error: position string must be length 65 (got {len(posString)}).", file=sys.stderr)
        sys.exit(1)
    if posString[0] not in ("W", "B"):
        print("Error: position[0] must be 'W' or 'B' (player to move).", file=sys.stderr)
        sys.exit(1)
    if any(ch not in ("E", "O", "X") for ch in posString[1:]):
        print("Error: board cells must be only 'E', 'O', or 'X'.", file=sys.stderr)
        sys.exit(1)

    start = time.time()
    deadline = start + time_limit
    pos = OthelloPosition(posString)
    algorithm = AlphaBeta(MatrixEvaluator())

    # Iterative deepening search
    best_move = OthelloAction(0, 0, is_pass_move = True)
    depth = 1
    while depth <= 64:
        try:
            algorithm.set_search_depth(depth)
            move = algorithm.evaluate(pos, deadline)
            if move is not None:
                best_move = move
            depth += 1
        except TimeoutException:
            break

    # Send the chosen move to stdout (print it)
    best_move.print_move()