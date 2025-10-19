# Building a Chess Engine from Scratch: A Tutorial

This guide walks you through the fundamental concepts and steps required to build a basic chess engine in C++.

## 1. Introduction

A chess engine is a program that can play chess. It does this by:
1.  Representing the board state.
2.  Generating all legal moves from a given position.
3.  Evaluating the "goodness" of a position.
4.  Searching through future possible moves to find the best one.

This tutorial will cover the creation of these core components.

## 2. Prerequisites

*   A modern C++ compiler (like GCC, Clang, or MSVC).
*   CMake for building the project.
*   A text editor or IDE (like VSCode, CLion, etc.).

## 3. Project Structure

A clean project structure is crucial. Here is a recommended layout, similar to this project:

```
/
├── CMakeLists.txt
├── include/      # Header files (.hpp)
│   ├── bitboard.hpp
│   ├── movegen.hpp
│   ├── eval.hpp
│   └── search.hpp
└── src/          # Source files (.cpp)
    ├── bitboard.cpp
    ├── movegen.cpp
    ├── eval.cpp
    ├── search.cpp
    └── main.cpp
```

## 4. Core Components

### Step 4.1: Board Representation

The first step is to decide how to represent the chessboard in memory. A common and efficient method is using **Bitboards**.

A bitboard is a 64-bit integer where each bit corresponds to a square on the board. We use a separate bitboard for each piece type and color.

*   **`include/bitboard.hpp`**: Define the bitboard structures and functions to set, clear, and query squares.
*   **`src/bitboard.cpp`**: Implement the bitboard logic.

**Example:** A bitboard for all white pawns might look like this, where `1`s represent pawn locations.

```
00000000
00000000
00000000
00000000
00000000
00000000
11111111  // Pawns on the 2nd rank
00000000
```

### Step 4.2: Move Generation

Once you have a board representation, you need to generate all legal moves for the current player.

This is a complex task, often broken down by piece type. For example, to find all pawn moves, you would look at the pawn bitboard and calculate all possible single pushes, double pushes, and captures.

*   **`include/movegen.hpp`**: Declare functions that generate moves for a given position.
*   **`src/movegen.cpp`**: Implement the move generation logic for each piece. This is often one of the most challenging parts to get right.

### Step 4.3: Evaluation Function

The evaluation function (`eval`) assigns a numerical score to a board position, indicating how favorable it is for a particular side (usually White). A positive score means White is better, a negative score means Black is better.

A simple evaluation can be based on material advantage:
*   Pawn: 100
*   Knight: 300
*   Bishop: 320
*   Rook: 500
*   Queen: 900

More advanced evaluations also consider piece-square tables (the value of a piece changes based on its position) and other positional factors.

*   **`include/eval.hpp`**: Declare the evaluation function.
*   **`src/eval.cpp`**: Implement the logic that calculates the score.

### Step 4.4: Search Algorithm

The search function is the brain of the engine. It explores the tree of possible future moves to find the best one.

The most common algorithm is **Minimax** with **Alpha-Beta Pruning**.

*   **Minimax**: Assumes both players play optimally. It tries to maximize its own score (as the "maximizer") while assuming the opponent will try to minimize it (as the "minimizer").
*   **Alpha-Beta Pruning**: An optimization to Minimax that significantly reduces the number of nodes (positions) that need to be evaluated in the search tree.

*   **`include/search.hpp`**: Declare the main search function.
*   **`src/search.cpp`**: Implement the recursive alpha-beta search logic.

## 5. Putting It All Together

The `main.cpp` file will contain the main loop of your program. It will:
1.  Initialize the board to the starting position.
2.  Enter a loop:
    a. Print the board.
    b. If it's the user's turn, wait for them to input a move.
    c. If it's the engine's turn, call the `search` function to find the best move.
    d. Make the chosen move on the board.
    e. Check for game-over conditions (checkmate, stalemate).

## 6. Building and Running

With `CMakeLists.txt` configured correctly, you can build your engine with:

```bash
mkdir build
cd build
cmake ..
make
```

This will create an executable in the `build` directory that you can run to play against your engine.

This tutorial provides a high-level overview. Each step involves significant detail and is a rewarding challenge to implement. Good luck!
