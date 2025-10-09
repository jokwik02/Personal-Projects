from OthelloEvaluator import OthelloEvaluator

"""
  An evaluator that uses a weight matrix and mobility heuristics to evaluate the board position.
  Author: Joakim Wiksten
"""


class MatrixEvaluator(OthelloEvaluator):

    def evaluate(self, othello_position):
        # Weight matrix evaluation
        score = 0
        weight_matrix = [
            [100, -30, 6, 2, 2, 6, -30, 100],
            [-30, -50, 0, 0, 0, 0, -50, -30],
            [6, 0, 0, 0, 0, 0, 0, 6],
            [2, 0, 0, 3, 3, 0, 0, 2],
            [2, 0, 0, 3, 3, 0, 0, 2],
            [6, 0, 0, 0, 0, 0, 0, 6],
            [-30, -50, 0, 0, 0, 0, -50, -30],
            [100, -30, 6, 2, 2, 6, -30, 100]
        ]

        for i in range(1, 9):          
            for j in range(1, 9):      
                square = othello_position.board[i][j]
                if square == 'W':
                    score += weight_matrix[i-1][j-1] 
                elif square == 'B':
                    score -= weight_matrix[i-1][j-1] 


        # Mobility evaluation
        white_pos = othello_position.clone()
        white_pos.maxPlayer = True
        white_moves = len(white_pos.get_moves())

        black_pos = othello_position.clone()
        black_pos.maxPlayer = False
        black_moves = len(black_pos.get_moves())

        score += (white_moves - black_moves) * 5

        return score