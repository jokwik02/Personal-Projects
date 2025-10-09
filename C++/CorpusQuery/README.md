# Corpus Query

**Description:**  
This C++ project implements a high-performance text query engine for structured corpora. It allows users to load large corpora, build efficient indices for token attributes (word, lemma, POS, C5), and perform complex queries with support for literal clauses, complements, and multi-token matching. Matches are highlighted in the console for easy visualization.

**Features:**  
- Load and index corpora from CSV files.  
- Query sentences using attribute-based clauses with equality/inequality support.  
- Efficient handling of large corpora using indexed searches.  
- Supports intersection, union, and difference operations on token sets.
- Built for maximum performance
- Console-based query interface with colored output for matched tokens.  


**Build & Run Instructions:**  

1. **Navigate to the project folder**:  
   cd path/to/FuzzyMatcher

2. **Build the project**:
   make
   
3. **Run the executable**:
   ./main

**Dependencies:**
- g++ compiler
- Standard C++ library

**Example usage:**
![example](images/query_example.png)
**Author:** 
Joakim Wiksten
