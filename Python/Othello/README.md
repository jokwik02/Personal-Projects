# Othello

**Description:**  
This project implements an Othello game engine that selects the best move for a given board position within a specified time limit. The engine uses **Alpha-Beta pruning** combined with **Iterative Deepening Search (IDS)**, allowing it to return the best move found before time expires.  
The engine was implemented together with a classmate, based on starter code provided by our lecturer.

**Features:**  
- Uses **Iterative Deepening Search** and **Alpha-Beta pruning** to find optimal moves.  
- Employs **mobility** and **weight-based heuristics** for move evaluation.  
- Includes **test code** to compare the engine’s performance against a naive opponent (provided by lecturer).

**Run Instructions:**  
Instructions for letting our engine play (called by othello.sh) against a naive engine (called by othello_native.sh)

1. **Navigate to the project folder**:
   ```bash
   cd path/to/Othello/test_code

2. **Create and activate a virtual environment (recommended)**
   ```bash
   python -m venv venv
   source venv/bin/activate        # On macOS/Linux  
   venv\Scripts\activate           # On Windows

3. **Install dependencies**
   ```bash
   pip install -r requirements.txt

4. **Run the program**:
   ```bash
   # For our engine as white
   bash othellostart.sh ../Python/othello.sh ./othello_naive.sh [Time Limit]

   # For our engine as black
   bash othellostart.sh ./othello_naive.sh ../Python/othello.sh [Time Limit]


**Author:** 
Joakim Wiksten (and classmate)
