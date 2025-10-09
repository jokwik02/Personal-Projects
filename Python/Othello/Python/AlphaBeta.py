from math import inf
from OthelloAlgorithm import OthelloAlgorithm
from OthelloAction import OthelloAction
import time
from TimeoutException import TimeoutException


class AlphaBeta(OthelloAlgorithm):
    """
    Alpha-Beta pruning search for Othello.

    This class implements the alpha-beta minimax algorithm using a provided evaluator.
    It has a hard deadline and throws a TimeoutException if exceeded.

	
    Author: Arvid Nordsvan
    """
    DefaultDepth = 1


    def __init__(self, othello_evaluator, depth=DefaultDepth):
        """
        Initialize the AlphaBeta searcher.

        Args:
            othello_evaluator: An evaluator with an `evaluate(pos) -> int/float` method.
            depth (int): The fixed maximum search depth (plies).
        """
        self.evaluator = othello_evaluator
        self.search_depth = depth


    def set_evaluator(self, othello_evaluator):
        """
        Set/replace the evaluation function.

        Args:
            othello_evaluator: New evaluator with `evaluate(pos)` method.
        """
        self.evaluator = othello_evaluator


    def set_search_depth(self, depth):
        """
        Set the fixed search depth.

        Args:
            depth (int): New maximum search depth (plies).
        """
        self.search_depth = depth


    def evaluate(self, othello_position, deadline):
        """
        Choose the best move for the side to move in `othello_position`.

        Uses alpha-beta search up to `self.search_depth` or until `deadline`
        is reached.

        Args:
            othello_position: The current OthelloPosition.
            deadline (float): Absolute time in seconds

        Returns:
            OthelloAction: The selected move. If no legal moves exist,
                           returns a pass move `OthelloAction(0, 0, is_pass_move=True)`.

        Raises:
            TimeoutException: If the deadline is reached during search.
        """
        if othello_position.to_move():
            _, move = self.max_value(othello_position, self.search_depth, -inf, inf, deadline)
        else:
            _, move = self.min_value(othello_position, self.search_depth, -inf, inf, deadline)
        
        return move or OthelloAction(0, 0, is_pass_move = True)
    
    def max_value(self, pos, depth, alpha, beta, deadline):
        """
        Max node of alpha-beta.

        Args:
            pos: Current OthelloPosition.
            depth (int): Remaining search depth (plies).
            alpha (float): Best value found so far for max along the path.
            beta (float): Best value found so far for min along the path.
            deadline (float): Absolute time limit.

        Returns:
            tuple[float, OthelloAction|None]: (best score, best move)

        Raises:
            TimeoutException: If the deadline is reached during search.
        """
        if time.time() >= deadline:
            raise TimeoutException()
        moves = pos.get_moves()
        if depth == 0:
            return self.evaluator.evaluate(pos), None
        if not moves:
            passed = pos.make_move(OthelloAction(0, 0, is_pass_move=True))
            if not passed.get_moves():
                return self.evaluator.evaluate(pos), OthelloAction(0, 0, is_pass_move=True)
            score, _ = self.min_value(passed, depth - 1, alpha, beta, deadline)
            return score, OthelloAction(0, 0, is_pass_move=True)
        
        value = -inf
        best_move = None
        for m in moves:
            child = pos.make_move(m)
            score, _ = self.min_value(child, depth - 1, alpha, beta, deadline)
            if score > value:
                value, best_move = score, m
            if value >= beta:
                return value, best_move
            alpha = max(alpha, value)
        return value, best_move
    
    def min_value(self, pos, depth, alpha, beta, deadline):
        """
        Min node of alpha-beta.

        Args:
            pos: Current OthelloPosition.
            depth (int): Remaining search depth (plies).
            alpha (float): Best value found so far for max along the path.
            beta (float): Best value found so far for min along the path.
            deadline (float): Absolute time limit.

        Returns:
            tuple[float, OthelloAction|None]: (best score, best move)

        Raises:
            TimeoutException: If the deadline is reached during search.
        """
        if time.time() >= deadline:
           raise TimeoutException()
        moves = pos.get_moves()
        if depth == 0:
            return self.evaluator.evaluate(pos), None
        if not moves:
            passed = pos.make_move(OthelloAction(0, 0, is_pass_move=True))
            if not passed.get_moves():
                return self.evaluator.evaluate(pos), OthelloAction(0, 0, is_pass_move=True)
            score, _ = self.max_value(passed, depth - 1, alpha, beta, deadline)
            return score, OthelloAction(0, 0, is_pass_move=True)
        
        value = inf
        best_move = None
        for m in moves:
            child = pos.make_move(m)
            score, _ = self.max_value(child, depth - 1, alpha, beta, deadline)
            if score < value:
                value, best_move = score, m
            if value <= alpha:
                return value, best_move
            beta = min(beta, value)
        return value, best_move