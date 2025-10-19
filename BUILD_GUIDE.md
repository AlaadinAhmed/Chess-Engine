# How to Build a C++ Chess Engine from Scratch (Ultimate Edition)

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

// Enum for piece types
enum Piece {
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING
};

// Enum for castling rights
enum Castling { WK = 1, WQ = 2, BK = 4, BQ = 8 };

struct Position {
    // Piece Bitboards
    uint64_t piece_bitboards[12]; // e.g., white_pawns=0, white_knights=1...

    // Combined Occupancy Bitboards
    uint64_t white_occupied;
    uint64_t black_occupied;
    uint64_t all_occupied;

    // Game State
    bool is_white_to_move;
    uint8_t castling_rights; // Bitmask using Castling enum
    int en_passant_square;   // Square index (0-63) or -1 if none
    int fifty_move_counter;
    uint64_t zobrist_key; // For transposition table
};
```

### 1.2 Zobrist Hashing
Zobrist hashing provides a way to compute a unique(ish) 64-bit key for each position. This is essential for the Transposition Table. The key is updated incrementally.

**Setup**:
1. At startup, create a table of pseudo-random 64-bit numbers:
   - One for each piece on each square (12 * 64)
   - One for the side to move (black)
   - One for each possible castling right (4)
   - One for each possible en-passant file (8)

**Updating the Key**:
When a move is made, the Zobrist key is updated with XOR operations. For example, moving a white pawn from e2 to e4:

```cpp
key ^= random_table[WHITE_PAWN][E2]; // remove pawn from e2
key ^= random_table[WHITE_PAWN][E4]; // add pawn to e4
key ^= random_table[BLACK_TO_MOVE];  // switch side to move
// ... and so on for castling/en-passant changes
```
This is much faster than re-computing the hash from scratch.

---

## Chapter 2: Move Generation

Move generation finds all legal moves. The process is to generate **pseudo-legal moves** and then validate their legality.

### 2.1 Pawn Move Generation
Pawns are the most complex piece to generate moves for.
- **Single Push**: Shift the pawn bitboard forward one rank.
- **Double Push**: For pawns on the 2nd/7th rank, shift them forward two ranks.
- **Captures**: Shift the pawn bitboard diagonally forward.
- **En Passant**: A special capture that can only happen immediately after a pawn makes a two-step advance.
- **Promotions**: If a pawn reaches the 8th/1st rank, it can be promoted to a Queen, Rook, Bishop, or Knight.

### 2.2 Legality Checking
After generating a pseudo-legal move, you must check if it's legal. The simplest way is:
1. Make the move on a temporary copy of the board.
2. Check if the king of the side that just moved is now in check. An `is_attacked()` function is useful here.
3. If the king is in check, the move was illegal.
4. Unmake the move.

---

## Chapter 3: The Search Function

The search function is the engine's core intelligence. A simple `alpha_beta` call is not enough for a strong engine. A proper search requires a framework of several components.

### 3.1 Principal Variation Search (PVS)
PVS is an optimization to alpha-beta that assumes the first move is the best. It performs a full search on the first move, but subsequent moves are searched with a "null window" to prove they are worse. If a move turns out to be better, it is re-searched with a full window.

### 3.2 Null Move Pruning
This is an aggressive pruning technique. The idea is to give the opponent an extra move (a "null move") and see if our position is still so good that it beats beta. If it does, we can prune the search, assuming the position is strong enough to withstand any reply.

### 3.3 Late Move Reductions (LMR)
This technique reduces the search depth for moves that are ordered later in the move list. The idea is that moves ordered later are less likely to be good, so we can spend less time on them. If a move that was reduced proves to be good, it can be re-searched at full depth.

---

## Chapter 4: Advanced Evaluation

A simple material and PST evaluation is a good start. A stronger engine needs more nuance.

### 4.1 Piece-Square Tables (PSTs)
PSTs are tables that assign a value to each piece on each square. For example, a knight on the rim is bad, but a knight in the center is good. Here is an example of a PST for white pawns:

```cpp
const int pawn_pst_mg[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
   50, 50, 50, 50, 50, 50, 50, 50,
   10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};
```

### 4.2 Other Evaluation Terms
- **Mobility**: Add a bonus for the number of legal moves a piece has.
- **Bishop Pair**: A bonus of around 50 points for having two bishops.
- **Connected Rooks**: A bonus for having rooks on the same rank or file that can see each other.
- **King Safety**: Penalize for open files around the king, and reward for pawn shields.

---

## Chapter 5: The UCI Protocol

To be used by graphical interfaces (GUIs) like Arena or CuteChess, an engine must implement a communication protocol. The **Universal Chess Interface (UCI)** is the standard.

Here is a more robust `main` loop:

```cpp
#include <iostream>
#include <string>
#include <thread>

void search_thread(Position pos) { ... }

int main() {
    Position pos;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name MyEngine" << std::endl;
            std::cout << "id author YourName" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (line == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (line.substr(0, 8) == "position") {
            // parse FEN and moves
        } else if (line.substr(0, 2) == "go") {
            // parse time controls
            std::thread t(search_thread, pos);
            t.detach();
        } else if (line == "quit") {
            break;
        }
    }
    return 0;
}
```

---

## Chapter 6: Testing and Debugging

Building a chess engine is notoriously bug-prone. A solid testing strategy is essential.

### 6.1 Perft Testing
**Perft** (Performance Test) is a standard way to verify the correctness of a move generator. It calculates the total number of legal moves to a certain depth.

### 6.2 FEN Parsing
A robust FEN (Forsyth-Edwards Notation) parser is critical for testing. It allows you to set up any position easily. Make sure your FEN parser can handle all aspects of the FEN specification, including en-passant, castling rights, and the fifty-move rule.

### 6.3 Using a Testing Framework
Consider using a testing framework like Google Test to create a suite of tests for your engine. You can create tests for:
- FEN parsing
- Move generation (perft)
- Evaluation (e.g., check that the evaluation of the starting position is 0)
- Search (e.g., check that the engine finds a simple mate-in-1)