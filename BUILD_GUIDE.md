# How to Build a C++ Chess Engine from Scratch (Expanded Edition)

This guide provides a detailed, step-by-step explanation of the algorithms and data structures needed to build a functional chess engine in C++. This expanded version includes more advanced topics crucial for engine strength.

---

## Chapter 1: Project Setup & Board Representation

We will use **Bitboards** for a high-performance board representation.

### 1.1 The `Position` Struct

The `Position` struct holds the entire game state. It contains 12 `uint64_t` bitboards for each piece type and color (e.g., `white_pawns`, `black_knights`).

**`include/position.hpp`:**
```cpp
#pragma once
#include <cstdint>

struct Position {
    // Piece Bitboards
    uint64_t piece_bitboards[12]; // e.g., white_pawns=0, white_knights=1...

    // Combined Occupancy Bitboards
    uint64_t white_occupied;
    uint64_t black_occupied;
    uint64_t all_occupied;

    // Game State
    bool is_white_to_move;
    uint8_t castling_rights; // Bitmask: 1=WK, 2=WQ, 4=BK, 8=BQ
    int en_passant_square;   // Square index (0-63) or -1 if none
    int fifty_move_counter;
    uint64_t zobrist_key; // For transposition table
};
```

### 1.2 Bitboard Manipulation
A piece is added to a bitboard using a bitwise OR (`|=`) and removed using a bitwise AND with a NOT (`&= ~`).

```cpp
// To add a white pawn to square e4 (index 28)
position.piece_bitboards[WHITE_PAWN] |= (1ULL << 28);

// To remove it
position.piece_bitboards[WHITE_PAWN] &= ~(1ULL << 28);
```

---

## Chapter 2: Move Generation

Move generation finds all legal moves. The process is to generate **pseudo-legal moves** and then validate their legality.

### 2.1 The Move Generation Algorithm

1.  Create an empty list of moves.
2.  Get a bitboard of all pieces for the current player.
3.  Loop through each piece on that bitboard. A fast method is to find and clear the least significant bit (LSB) in a loop:
    ```cpp
    uint64_t pieces = ...;
    while (pieces) {
        int square = __builtin_ctzll(pieces); // Get index of LSB
        // ... generate moves for the piece on this square ...
        pieces &= pieces - 1; // Clear the LSB to move to the next piece
    }
    ```
4.  Inside the loop, a `switch` on the piece type calls a specific move generation function.
5.  **Legality Check**: For each generated move, make the move on a temporary board and verify the king is not left in check. If it is safe, the move is legal.

### 2.2 Generating Sliding Piece Moves (Bishops, Rooks, Queens)

Generating moves for sliding pieces is complex because they are blocked by other pieces. A highly effective method is using **Magic Bitboards**.

**The Magic Bitboard Algorithm:**
1.  **Pre-calculation**: At startup, you generate a set of "magic" numbers and attack tables.
2.  **In Move Generation**: To find the moves for a rook on square `s`:
    a. Get a bitboard of all occupied squares: `blockers = pos.all_occupied`.
    b. Isolate only the blockers on the rook's rank and file.
    c. Use the magic number to transform this blocker set into an index for your pre-calculated attack table.
    d. Look up the attack set from the table using this index.

This seems complex, but it reduces move generation for sliding pieces to a few bitwise operations and a table lookup, which is extremely fast.

---

## Chapter 3: The Search Function

The search function is the engine's core intelligence. A simple `alpha_beta` call is not enough for a strong engine. A proper search requires a framework of several components.

### 3.1 Iterative Deepening

Instead of searching for a fixed depth (e.g., 5 moves), we search in a loop. This is called Iterative Deepening.

```cpp
void search(Position& pos, int max_time) {
    for (int depth = 1; depth <= MAX_SEARCH_DEPTH; ++depth) {
        // Call alpha_beta with the current depth
        best_move = alpha_beta(pos, depth, ...);
        // if time is up, break the loop
    }
}
```
**Benefits**: 
- **Time Management**: We can stop the search at any time (e.g., after 2 seconds) and still have a best move from the previously completed depth.
- **Move Ordering**: The best move found at `depth=3` is an excellent candidate to search first at `depth=4`.

### 3.2 Move Ordering

The performance of Alpha-Beta search depends *exponentially* on the order in which moves are searched. If you always search the best move first, pruning is maximized. A good move ordering scheme is critical.

**The Algorithm**: Before searching the moves at a node, sort them in the following order of priority:

1.  **PV-Move**: The Principal Variation move (the best move found from the previous Iterative Deepening search).
2.  **Transposition Table Move**: If the position is in our Transposition Table, the move stored there is often very good.
3.  **Promotions and Good Captures**: Queen promotions and captures where a low-value piece takes a high-value piece (e.g., Pawn x Queen). You can use a simple function called **Static Exchange Evaluation (SEE)** to estimate if a capture is good.
4.  **Killer Moves**: These are quiet (non-capture) moves that have caused a beta-cutoff at the same ply in other branches of the search. We store two "killer moves" per ply.
5.  **History Heuristic**: All other quiet moves are sorted based on a "history score", which is incremented whenever a move is found to be good elsewhere in the search.

### 3.3 Transposition Table

A Transposition Table (TT) is a large hash table that stores the results of previously searched positions. This avoids re-searching the same position over and over.

**The Algorithm**:
1.  Before searching a position, calculate its **Zobrist Key**.
2.  Probe the TT with this key. An entry in the table typically contains:
    - The Zobrist key (to verify it's not a hash collision).
    - The search depth the position was evaluated at.
    - The score.
    - The best move found.
    - A flag (Exact, Lower Bound, or Upper Bound).
3.  **Using the Entry**: If the stored depth is greater than or equal to our current search depth, we can often use the stored score directly and prune the entire search of this node.
4.  **Storing an Entry**: After a search of a node is complete, store the result (score, depth, best move) in the TT.

### 3.4 Quiescence Search

When the main search reaches its depth limit (a leaf node), it calls `quiescence()`. This is a special search that only considers captures and promotions until a "quiet" position is reached. This avoids the **horizon effect**, where an engine might miss a simple capture that occurs just beyond its search depth.

---

## Chapter 4: Advanced Evaluation

A simple material and PST evaluation is a good start. A stronger engine needs more nuance.

### 4.1 Tapered Evaluation

The importance of evaluation terms changes as the game progresses. King safety is vital in the middlegame, but an active king is an asset in the endgame. We can blend scores using a **Game Phase** value.

1.  Calculate a `phase` value (e.g., from 0 for endgame to 24 for opening), based on the non-pawn material on the board.
2.  Calculate both a middlegame score (`mg_score`) and an endgame score (`eg_score`) for the position.
3.  Combine them: `final_score = (mg_score * phase + eg_score * (24 - phase)) / 24;`

### 4.2 Evaluation Components

- **Pawn Structure**: Use dedicated functions to find and apply penalties for doubled pawns, isolated pawns, and bonuses for passed pawns.
- **King Safety**: A simple approach is to create a "danger score" for the king. For each square around the king, add points for each enemy piece that attacks it. This score can be subtracted from the evaluation.
- **Piece Mobility**: For each piece, count the number of legal moves it has. Add a small bonus for each move, as more mobile pieces are generally better.

---

## Chapter 5: The UCI Protocol

To be used by graphical interfaces (GUIs) like Arena or CuteChess, an engine must implement a communication protocol. The **Universal Chess Interface (UCI)** is the standard.

Your `main()` loop should not just be for playing, but for listening to UCI commands:

**The Main Loop Algorithm**:
1.  Wait for input from the GUI.
2.  If the input is `"uci"`, respond with `"id name MyEngine"` and `"uciok"`.
3.  If the input is `"isready"`, respond with `"readyok"`.
4.  If the input starts with `"position"`, parse the FEN string or move list and set up the internal board.
5.  If the input starts with `"go"`, parse the time controls (e.g., `wtime`, `btime`) and start your search function in a separate thread.
6.  When the search function completes, print the result to the console in the format `"bestmove e2e4"`.