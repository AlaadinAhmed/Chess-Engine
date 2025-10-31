/**
@page chess_engine_guide The Complete Guide to Writing a Chess Engine

@tableofcontents

@section guide_intro Introduction

A chess engine is a sophisticated computer program that plays chess at superhuman levels. This comprehensive guide will teach you how to build one from scratch, explaining every concept in detail with diagrams, examples, and complete code.

@subsection what_is_engine What is a Chess Engine?

A chess engine consists of several interconnected components:

@dot "Chess Engine Architecture"
digraph engine {
    rankdir=TB;
    node [shape=box, style=filled, fillcolor=lightblue];
    
    UCI [label="UCI Protocol\n(Communication)", fillcolor=lightgreen];
    Board [label="Board Representation\n(Bitboards)", fillcolor=lightyellow];
    MoveGen [label="Move Generator\n(Legal Moves)", fillcolor=lightcyan];
    Eval [label="Evaluation Function\n(Position Score)", fillcolor=pink];
    Search [label="Search Algorithm\n(Find Best Move)", fillcolor=orange];
    TT [label="Transposition Table\n(Cache)", fillcolor=violet];
    
    UCI -> Board [label="Set Position"];
    UCI -> Search [label="Start Search"];
    Board -> MoveGen [label="Generate"];
    MoveGen -> Search [label="Provide Moves"];
    Board -> Eval [label="Evaluate"];
    Eval -> Search [label="Return Score"];
    Search -> TT [label="Store/Lookup"];
    Search -> UCI [label="Best Move"];
}
@enddot

**Core Components:**

1. **Board Representation** - How to store the position efficiently (pieces, castling rights, etc.)
2. **Move Generation** - Finding all legal moves in any position
3. **Position Evaluation** - Determining who is winning and by how much
4. **Search Algorithm** - Looking ahead to find the best move
5. **UCI Protocol** - Communicating with chess GUIs
6. **Transposition Table** - Caching previously searched positions

@subsection what_you_learn What You'll Learn

By the end of this guide, you'll understand:
- ✅ Every line of code in a chess engine
- ✅ Why each technique is necessary
- ✅ How Stockfish (the world's strongest engine) works
- ✅ How to write your own competitive chess engine
- ✅ Advanced techniques like NNUE neural networks

@subsection prerequisites Prerequisites

| Category | Requirement | Details |
|----------|-------------|---------|
| **Programming** | Intermediate C++ | Classes, templates, STL |
| **Chess** | Basic rules | How pieces move, check, checkmate |
| **Math** | Bitwise operations | AND, OR, XOR, shifts |
| **Data Structures** | Arrays, hash tables | Basic CS knowledge |

@subsection tools_needed Tools You'll Need

@code{.bash}
# Compiler with C++17 support
g++ --version  # GCC 7+ or Clang 5+

# Chess GUI for testing
# Download Arena or Cute Chess GUI

# Version control
git --version

# Optional: Graphical debugger
gdb --version
@endcode

---

@section chapter1 Chapter 1: Understanding Chess Programming Fundamentals

@subsection chess_challenge 1.1 The Chess Programming Challenge

@subsubsection search_space_problem The Search Space Explosion

Chess has an **enormous** search space. Let's visualize this:

@dot "Search Space Growth"
digraph search_growth {
    rankdir=TB;
    node [shape=ellipse, style=filled];
    
    start [label="Starting Position", fillcolor=lightgreen];
    ply1 [label="Depth 1\n~20 positions", fillcolor=lightyellow];
    ply2 [label="Depth 2\n~400 positions", fillcolor=orange];
    ply3 [label="Depth 3\n~8,902 positions", fillcolor=red];
    ply4 [label="Depth 4\n~197,281 positions", fillcolor=darkred, fontcolor=white];
    ply5 [label="Depth 5\n~4,865,609 positions", fillcolor=purple, fontcolor=white];
    
    start -> ply1 [label="×20"];
    ply1 -> ply2 [label="×20"];
    ply2 -> ply3 [label="×22"];
    ply3 -> ply4 [label="×22"];
    ply4 -> ply5 [label="×25"];
}
@enddot

**Growth Statistics:**

| Depth | Positions | Branching Factor |
|-------|-----------|------------------|
| 0 | 1 | - |
| 1 | 20 | 20 |
| 2 | 400 | 20 |
| 3 | 8,902 | 22.25 |
| 4 | 197,281 | 22.16 |
| 5 | 4,865,609 | 24.67 |
| 10 | ~69,352,859,712 | ~30 average |

@note The total number of possible chess games is estimated at \f$10^{120}\f$, which is more than the number of atoms in the observable universe (\f$10^{80}\f$)!

@subsubsection complexity_factors Complexity Factors

**1. Branching Factor**

@dot "Average Branching Factor by Game Phase"
digraph branching {
    node [shape=box];
    
    opening [label="Opening\n~35 moves", fillcolor=lightgreen, style=filled];
    middlegame [label="Middlegame\n~40 moves", fillcolor=yellow, style=filled];
    endgame [label="Endgame\n~15 moves", fillcolor=lightblue, style=filled];
}
@enddot

**2. Time Constraints**

Typical time controls:
- **Blitz**: 3-5 minutes per game → ~2 seconds per move
- **Rapid**: 15 minutes per game → ~10 seconds per move  
- **Classical**: 2 hours per game → ~1 minute per move

**3. Evaluation Complexity**

@dot "Position Complexity"
digraph eval_complexity {
    rankdir=LR;
    node [shape=box, style=filled];
    
    simple [label="Simple Position\nMaterial only", fillcolor=lightgreen];
    moderate [label="Moderate Position\nMaterial + Position", fillcolor=yellow];
    complex [label="Complex Position\nFull evaluation", fillcolor=orange];
    tactical [label="Tactical Position\nDeep calculation", fillcolor=red];
    
    simple -> moderate -> complex -> tactical;
}
@enddot

@subsubsection solution_overview The Solution: Selective Search

Instead of searching everything, we use smart techniques:

@dot "Search Optimization Strategy"
digraph optimization {
    rankdir=TB;
    node [shape=box, style=filled, fillcolor=lightblue];
    
    full [label="Full Minimax\n~10^15 nodes at depth 10"];
    alphabeta [label="Alpha-Beta Pruning\n~10^9 nodes (1000x faster!)"];
    tt [label="+ Transposition Table\n~10^8 nodes"];
    moveorder [label="+ Move Ordering\n~10^7 nodes"];
    pruning [label="+ Advanced Pruning\n~10^6 nodes"];
    
    full -> alphabeta [label="Basic optimization"];
    alphabeta -> tt [label="Cache results"];
    tt -> moveorder [label="Search best first"];
    moveorder -> pruning [label="Skip bad moves"];
}
@enddot

**Key Techniques:**
1. **Alpha-Beta Pruning** - Skip provably bad branches (1000x speedup!)
2. **Transposition Tables** - Cache positions we've seen before  
3. **Move Ordering** - Search best moves first (better pruning)
4. **Selective Extensions** - Search deeper in interesting positions
5. **Pruning Heuristics** - Skip obviously bad moves

@subsection core_concepts 1.2 Core Concepts

@subsubsection ply_vs_move Concept 1: Ply vs Move

Understanding the difference is crucial for implementing search:

@startuml "Ply vs Move Illustration"
!define LIGHTBLUE #E8F4F8
!define LIGHTGREEN #E8F8E8

participant "White" as W <<LIGHTGREEN>>
participant "Board" as B
participant "Black" as Bl <<LIGHTBLUE>>

W -> B: e2-e4
note right: Ply 1 (White's turn)\nMove 1 start
B --> W: Position after e4

Bl -> B: e7-e5  
note right: Ply 2 (Black's turn)\nMove 1 complete
B --> Bl: Position after e5

W -> B: Ng1-f3
note right: Ply 3\nMove 2 start
B --> W: Position after Nf3
@enduml

**Key Differences:**

| Concept | Definition | Example |
|---------|------------|---------|
| **Ply** | One player's turn | White plays e4 = 1 ply |
| **Move** | Both players move | 1. e4 e5 = 1 move (2 plies) |
| **Depth** | How many plies to search | Depth 6 = 3 full moves |

@code{.cpp}
// In search code:
int search(Position& pos, int depth) {
    if (depth == 0) {
        return evaluate(pos);  // Reached depth limit
    }
    
    // depth counts PLIES not moves
    // depth=10 means looking ahead 5 full moves
}
@endcode

@warning Always use **ply** as the unit of depth in search. Depth 10 = 10 plies = 5 full moves!

---

## Introduction

### What is a Chess Engine?

A chess engine is a computer program that can:
1. **Understand chess positions** - Store and manipulate the board state
2. **Generate legal moves** - Find all possible moves in any position
3. **Evaluate positions** - Determine who is winning and by how much
4. **Search ahead** - Look at future positions to find the best move
5. **Communicate** - Interface with chess GUIs via UCI protocol

### What You'll Learn

By the end of this guide, you'll understand:
- Every line of code in a chess engine
- Why each technique is necessary
- How Stockfish (the world's strongest engine) works
- How to write your own competitive chess engine

### Prerequisites

- **Programming**: Intermediate C++ knowledge
- **Chess**: Know how pieces move and basic rules
- **Math**: Basic bitwise operations (AND, OR, XOR, shifts)
- **Data Structures**: Arrays, structs, hash tables

### Tools You'll Need

```bash
# Compiler with C++17 support
g++ --version  # GCC 7+ or Clang 5+

# Chess GUI for testing
# Download Arena or Cute Chess GUI

# Version control
git --version
```

---

## Chapter 1: Understanding Chess Programming Fundamentals

### 1.1 The Chess Programming Challenge

**Why is chess programming hard?**

1. **Huge search space**: From starting position, there are:
   - 20 legal first moves
   - 400 positions after 1 move by each side
   - 197,281 positions after 2 moves by each side
   - ~10^120 possible chess games (more atoms than in the universe!)

2. **Real-time constraints**: Must make good moves quickly (often 1-10 seconds)

3. **Complex evaluation**: Determining who's winning is not trivial

**The Solution: Selective Search**

Instead of searching all positions, we:
1. Search the most promising moves deeper
2. Use heuristics to evaluate positions quickly
3. Cache results to avoid recomputing

### 1.2 Core Concepts

#### Concept 1: Ply vs Move

```
Ply = One player's turn
Move = White moves + Black moves

Example:
1. e4 e5  <- This is ONE MOVE but TWO PLIES
   ^^^
   First ply (White)
      ^^^
      Second ply (Black)
```

**Why this matters**: Search depth is measured in ply, not moves.

#### Concept 2: Centipawns

Evaluation scores use "centipawns" (1/100th of a pawn):

```
Score:  Meaning:
+100   = White is ahead by 1 pawn
+900   = White is ahead by a queen
-300   = Black is ahead by a knight
+30000 = White has checkmate
```

**Why centipawns?** Allows for fine-grained positional adjustments (e.g., +10 for better pawn structure).

#### Concept 3: Minimax Principle

```
You want to maximize your score
Your opponent wants to minimize your score (maximize theirs)

Example tree:
         You to move
        /     |      \
       /      |       \
     Max    Max      Max
    scores  scores   scores
     /|\     /|\      /|\
    / | \   / | \    / | \
  Opp moves on each branch
  (trying to minimize your score)
```

**The algorithm**: At each level, alternate between maximizing and minimizing.

---

## Chapter 2: Board Representation - The Foundation

### 2.1 Why Bitboards?

**The Problem with Array Representation**

```cpp
// Old-school 8x8 array approach
char board[8][8] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    // ... etc
};

// Problems:
// 1. Slow to find pieces (must scan whole array)
// 2. Hard to do set operations (find all attacked squares)
// 3. Takes 64 bytes of memory
// 4. Copying is slow (64 individual elements)
```

**The Bitboard Solution**

A bitboard is a 64-bit number where each bit represents a square:

```
Binary:   Bit represents:
Bit 0  -> a1  (0x1ULL)
Bit 1  -> b1  (0x2ULL)
Bit 2  -> c1  (0x4ULL)
...
Bit 7  -> h1  (0x80ULL)
Bit 8  -> a2
...
Bit 63 -> h8  (0x8000000000000000ULL)
```

Visual representation:
```
  a  b  c  d  e  f  g  h
8 56 57 58 59 60 61 62 63  <- Rank 8
7 48 49 50 51 52 53 54 55  <- Rank 7
6 40 41 42 43 44 45 46 47
5 32 33 34 35 36 37 38 39
4 24 25 26 27 28 29 30 31
3 16 17 18 19 20 21 22 23
2  8  9 10 11 12 13 14 15
1  0  1  2  3  4  5  6  7   <- Rank 1
```

**Why bitboards are amazing:**

1. **Fast operations using CPU instructions**
```cpp
// Find all white pieces
uint64_t all_white = white_pawns | white_knights | white_bishops | ...;
// Takes 1 CPU cycle!

// Find attacks
uint64_t attacked = rook_attacks & enemy_pieces;
// Bitwise AND in 1 cycle
```

2. **Compact memory**
```cpp
// 12 bitboards for all pieces = 12 * 8 bytes = 96 bytes total
// vs 64 bytes for array that's harder to use
```

3. **Easy to copy positions**
```cpp
Position copy = original;  // Copies all bitboards instantly
```

### 2.2 Complete Position Structure

**Everything needed to represent a chess position:**

```cpp
struct Position {
    // === BITBOARDS FOR PIECES ===
    // Each bitboard has 1s where that piece exists
    
    uint64_t WhitePawns;      // Bit = 1 means white pawn on that square
    uint64_t WhiteKnights;
    uint64_t WhiteBishops;
    uint64_t WhiteRooks;
    uint64_t WhiteQueen;      // Only 1 bit should be set (unless promoted)
    uint64_t WhiteKing;       // Exactly 1 bit set
    
    uint64_t BlackPawns;
    uint64_t BlackKnights;
    uint64_t BlackBishops;
    uint64_t BlackRooks;
    uint64_t BlackQueen;
    uint64_t BlackKing;
    
    // === COMBINED BITBOARDS (for speed) ===
    // These are computed from the piece bitboards
    
    uint64_t WhiteoccupiedSquares;  // All white pieces OR'd together
    uint64_t BlackoccupiedSquares;  // All black pieces OR'd together
    uint64_t occupiedSquares;       // All pieces (white | black)
    uint64_t emptySquares;          // ~occupiedSquares
    
    // === GAME STATE ===
    
    bool whiteToMove;           // true = white's turn, false = black's turn
    
    uint8_t castelingRights;    // Encoded as 4 bits: KQkq
                                // Bit 0 (0x1): White kingside  (K)
                                // Bit 1 (0x2): White queenside (Q)
                                // Bit 2 (0x4): Black kingside  (k)
                                // Bit 3 (0x8): Black queenside (q)
    
    uint64_t enPassant;         // Bitboard with 1 bit set for e.p. square
                                // Example: if white plays e2-e4,
                                // enPassant = (1ULL << 16) = e3 square
                                // Set to 0 if no e.p. available
    
    int move50rule;             // Halfmove clock for 50-move draw rule
                                // Increments each move
                                // Resets to 0 on pawn move or capture
    
    int fullMoveNumber;         // Starts at 1, increments after black moves
    
    // === HASH KEY (for transposition table) ===
    
    uint64_t zobrist_key;       // Unique hash of the position
                                // Changes incrementally with each move
};
```

**WHY we need each field:**

- **Piece bitboards**: Fast "where are the pawns?" queries
- **Combined bitboards**: Quick "is this square occupied?" checks
- **whiteToMove**: Know whose turn it is
- **castelingRights**: Legal castling depends on whether king/rooks moved
- **enPassant**: En passant is legal only on the move after pawn double-push
- **move50rule**: Enforce 50-move draw rule
- **zobrist_key**: Quickly look up if we've seen this position before

### 2.3 Bitboard Operations - The Toolbox

**Operation 1: Setting a bit (placing a piece)**

```cpp
// Put a white pawn on e4 (square 28)
WhitePawns |= (1ULL << 28);

// Explanation:
// 1ULL << 28  creates: 0b...00010000000000000000000000000
//                              ^
//                              Bit 28 set
// |= means "OR with this and store"
// This turns bit 28 ON without affecting other bits
```

**Operation 2: Clearing a bit (removing a piece)**

```cpp
// Remove white pawn from e4
WhitePawns &= ~(1ULL << 28);

// Explanation:
// 1ULL << 28     = 0b...00010000000000000000000000000
// ~(1ULL << 28)  = 0b...11101111111111111111111111111
//                         ^
//                         All 1s except bit 28
// &= means "AND with this and store"
// This turns bit 28 OFF without affecting other bits
```

**Operation 3: Testing a bit (is piece present?)**

```cpp
// Check if there's a white pawn on e4
bool pawn_on_e4 = (WhitePawns & (1ULL << 28)) != 0;

// Explanation:
// & creates a bitboard with only bit 28 (if it was set)
// != 0 converts to boolean
```

**Operation 4: Counting bits (how many pieces?)**

```cpp
// Count white pawns
int pawn_count = __builtin_popcountll(WhitePawns);

// Explanation:
// __builtin_popcountll is a GCC/Clang intrinsic
// It uses the CPU's POPCNT instruction
// Counts number of 1-bits in the value
```

**Operation 5: Finding the first bit (find a piece)**

```cpp
// Find position of first white pawn
int square = __builtin_ctzll(WhitePawns);

// Explanation:
// __builtin_ctzll = "Count Trailing Zeros Long Long"
// Returns index of least significant 1-bit
// Example: 0b...0100100 returns 2
// Uses CPU's BSF (Bit Scan Forward) instruction
```

**Operation 6: Clearing the first bit (iterate through pieces)**

```cpp
// Process all white pawns
uint64_t pawns = WhitePawns;
while (pawns) {
    int square = __builtin_ctzll(pawns);
    
    // ... do something with this pawn ...
    
    pawns &= pawns - 1;  // Remove this bit
}

// Explanation of "pawns &= pawns - 1":
// Say pawns = 0b...0101100
//             0b...0101100  (original)
//           - 0b...0000001  (subtract 1)
//           = 0b...0101011
//           & 0b...0101100  (AND with original)
//           = 0b...0101000  <- Least significant bit removed!
```

### 2.4 FEN Parsing - Loading Positions

**What is FEN?**

FEN (Forsyth-Edwards Notation) is the standard way to describe a chess position.

Example:
```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

**FEN has 6 fields separated by spaces:**

1. **Piece placement** (rank 8 to rank 1, separated by '/')
   - Lowercase = black pieces (r,n,b,q,k,p)
   - Uppercase = white pieces (R,N,B,Q,K,P)
   - Numbers = empty squares (1-8)

2. **Active color** ('w' or 'b')

3. **Castling rights** ('KQkq', '-' if none)
   - K = white can castle kingside
   - Q = white can castle queenside
   - k = black can castle kingside
   - q = black can castle queenside

4. **En passant target square** (e.g., 'e3' or '-')

5. **Halfmove clock** (for 50-move rule)

6. **Fullmove number** (starts at 1)

**Complete FEN Parsing Algorithm:**

```cpp
void parseFEN(Position& pos, const std::string& fen) {
    // STEP 1: Clear the position
    pos.WhitePawns = 0;
    pos.WhiteKnights = 0;
    pos.WhiteBishops = 0;
    pos.WhiteRooks = 0;
    pos.WhiteQueen = 0;
    pos.WhiteKing = 0;
    pos.BlackPawns = 0;
    pos.BlackKnights = 0;
    pos.BlackBishops = 0;
    pos.BlackRooks = 0;
    pos.BlackQueen = 0;
    pos.BlackKing = 0;
    
    // WHY: Start with a blank board
    
    // STEP 2: Split FEN into fields
    std::istringstream iss(fen);
    std::string piece_placement, active_color, castling, ep_square;
    int halfmove, fullmove;
    
    iss >> piece_placement >> active_color >> castling >> ep_square >> halfmove >> fullmove;
    
    // WHY: Each field is space-separated
    
    // STEP 3: Parse piece placement
    int rank = 7;  // Start at rank 8 (index 7)
    int file = 0;  // Start at file a (index 0)
    
    for (char c : piece_placement) {
        if (c == '/') {
            // WHY: '/' separates ranks
            rank--;
            file = 0;
        }
        else if (c >= '1' && c <= '8') {
            // WHY: Numbers mean empty squares
            file += (c - '0');
        }
        else {
            // It's a piece letter
            int square = rank * 8 + file;
            uint64_t bit = (1ULL << square);
            
            // WHY: We convert rank,file to square index
            // rank * 8 + file maps to 0-63
            
            switch (c) {
                case 'P': pos.WhitePawns   |= bit; break;
                case 'N': pos.WhiteKnights |= bit; break;
                case 'B': pos.WhiteBishops |= bit; break;
                case 'R': pos.WhiteRooks   |= bit; break;
                case 'Q': pos.WhiteQueen   |= bit; break;
                case 'K': pos.WhiteKing    |= bit; break;
                case 'p': pos.BlackPawns   |= bit; break;
                case 'n': pos.BlackKnights |= bit; break;
                case 'b': pos.BlackBishops |= bit; break;
                case 'r': pos.BlackRooks   |= bit; break;
                case 'q': pos.BlackQueen   |= bit; break;
                case 'k': pos.BlackKing    |= bit; break;
            }
            
            file++;
        }
    }
    
    // STEP 4: Parse active color
    pos.whiteToMove = (active_color == "w");
    
    // WHY: Simple true/false based on 'w' or 'b'
    
    // STEP 5: Parse castling rights
    pos.castelingRights = 0;
    
    if (castling.find('K') != std::string::npos) pos.castelingRights |= 0x1;
    if (castling.find('Q') != std::string::npos) pos.castelingRights |= 0x2;
    if (castling.find('k') != std::string::npos) pos.castelingRights |= 0x4;
    if (castling.find('q') != std::string::npos) pos.castelingRights |= 0x8;
    
    // WHY: Each castling right is a separate bit
    // This allows fast checking: if (rights & 0x1) can_white_castle_kingside
    
    // STEP 6: Parse en passant square
    pos.enPassant = 0;
    
    if (ep_square != "-") {
        int ep_file = ep_square[0] - 'a';  // Convert 'e' to 4
        int ep_rank = ep_square[1] - '1';  // Convert '3' to 2
        int ep_sq = ep_rank * 8 + ep_file;
        pos.enPassant = (1ULL << ep_sq);
    }
    
    // WHY: Store as bitboard for consistency
    // Makes it easy to check: if (pawn_attacks & pos.enPassant) can_ep_capture
    
    // STEP 7: Parse move counters
    pos.move50rule = halfmove;
    pos.fullMoveNumber = fullmove;
    
    // STEP 8: Compute combined bitboards
    pos.WhiteoccupiedSquares = pos.WhitePawns | pos.WhiteKnights | 
                               pos.WhiteBishops | pos.WhiteRooks |
                               pos.WhiteQueen | pos.WhiteKing;
    
    pos.BlackoccupiedSquares = pos.BlackPawns | pos.BlackKnights |
                               pos.BlackBishops | pos.BlackRooks |
                               pos.BlackQueen | pos.BlackKing;
    
    pos.occupiedSquares = pos.WhiteoccupiedSquares | pos.BlackoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;
    
    // WHY: These are used constantly in move generation
    // Computing once here saves time later
    
    // STEP 9: Compute Zobrist hash
    pos.zobrist_key = compute_zobrist_key(pos);
    
    // WHY: We'll need this for transposition table
    // (Zobrist hashing explained in detail later)
}
```

**Example walkthrough:**

```
FEN: rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1

Step by step:
1. Clear all bitboards to 0
2. Split: "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"
         "b" "KQkq" "e3" "0" "1"
3. Parse pieces:
   - rank=7, file=0: 'r' -> BlackRooks |= (1ULL << 56)
   - rank=7, file=1: 'n' -> BlackKnights |= (1ULL << 57)
   - ... etc
   - rank=4, file=4: 'P' -> WhitePawns |= (1ULL << 28) [e4]
4. whiteToMove = false (it's "b")
5. castelingRights = 0x1 | 0x2 | 0x4 | 0x8 = 0xF (all castling allowed)
6. enPassant = (1ULL << 20) [e3 square]
7. move50rule = 0, fullMoveNumber = 1
8. Compute combined bitboards
9. Compute hash
```

### 2.5 Helper Functions

**Converting between square formats:**

```cpp
// Convert square index (0-63) to algebraic notation ("a1"-"h8")
std::string square_to_string(int square) {
    int file = square % 8;      // 0-7 -> a-h
    int rank = square / 8;      // 0-7 -> 1-8
    
    std::string result;
    result += ('a' + file);     // 'a' + 4 = 'e'
    result += ('1' + rank);     // '1' + 3 = '4'
    return result;              // "e4"
}

// Convert algebraic notation to square index
int string_to_square(const std::string& sq) {
    int file = sq[0] - 'a';     // 'e' - 'a' = 4
    int rank = sq[1] - '1';     // '4' - '1' = 3
    return rank * 8 + file;     // 3 * 8 + 4 = 28
}

// WHY: UCI protocol uses algebraic notation
// Internally we use square indices
```

**Finding what piece is on a square:**

```cpp
Pieces get_piece_at(const Position& pos, int square) {
    uint64_t bit = (1ULL << square);
    
    // WHY: Check each bitboard to see if this square is occupied
    
    // Check white pieces
    if (pos.WhitePawns   & bit) return W_PAWN;
    if (pos.WhiteKnights & bit) return W_KNIGHT;
    if (pos.WhiteBishops & bit) return W_BISHOP;
    if (pos.WhiteRooks   & bit) return W_ROOK;
    if (pos.WhiteQueen   & bit) return W_QUEEN;
    if (pos.WhiteKing    & bit) return W_KING;
    
    // Check black pieces
    if (pos.BlackPawns   & bit) return B_PAWN;
    if (pos.BlackKnights & bit) return B_KNIGHT;
    if (pos.BlackBishops & bit) return B_BISHOP;
    if (pos.BlackRooks   & bit) return B_ROOK;
    if (pos.BlackQueen   & bit) return B_QUEEN;
    if (pos.BlackKing    & bit) return B_KING;
    
    return NO_PIECE;
}

// WHY: We need this for:
// - Determining captures
// - Move validation
// - Debugging/display
```

---

## Chapter 3: Move Generation - Finding All Legal Moves

### 3.1 Move Representation

**The Move Structure:**

```cpp
struct Move {
    int from;                   // Source square (0-63)
    int to;                     // Destination square (0-63)
    Pieces promotion = NO_PIECE; // Promotion piece (or NO_PIECE)
    int score = 0;              // For move ordering (explained later)
};

// WHY each field:
// - from/to: Minimal info needed to make a move
// - promotion: Pawn promotions need to know Q/R/B/N
// - score: Later we'll use this for alpha-beta optimization
```

**Move List Container:**

```cpp
struct MoveList {
    Move moves[256];    // Maximum possible moves in any position is ~218
    int count = 0;      // How many moves are in the list
    
    // Helper to add moves
    void add(Move m) {
        if (count < 256) {
            moves[count++] = m;
        }
    }
};

// WHY 256: Safe upper bound
// Worst case positions have ~218 legal moves
// 256 is a nice power of 2
```

### 3.2 Attack Bitboards - Pre-computed Tables

**WHY we need attack tables:**

For pieces like kings and knights, their attacks don't depend on other pieces (non-sliding). We can pre-compute all possible attacks once at startup.

**King Attacks:**

```cpp
// Global array: kingAttacks[square] = bitboard of king attacks from that square
uint64_t kingAttacks[64];

void initKingAttacks() {
    // FOR each square on the board
    for (int square = 0; square < 64; square++) {
        uint64_t attacks = 0;
        int rank = square / 8;  // 0-7
        int file = square % 8;  // 0-7
        
        // WHY we need rank/file: To check board boundaries
        
        // A king can move to 8 adjacent squares (if on board)
        
        // North (rank + 1)
        if (rank < 7) {
            attacks |= (1ULL << (square + 8));
        }
        // WHY rank < 7: King on rank 8 can't move north
        
        // South (rank - 1)
        if (rank > 0) {
            attacks |= (1ULL << (square - 8));
        }
        
        // East (file + 1)
        if (file < 7) {
            attacks |= (1ULL << (square + 1));
        }
        // WHY file < 7: King on h-file can't move east
        
        // West (file - 1)
        if (file > 0) {
            attacks |= (1ULL << (square - 1));
        }
        
        // North-East
        if (rank < 7 && file < 7) {
            attacks |= (1ULL << (square + 9));
        }
        // WHY +9: Up one rank (+8) and right one file (+1)
        
        // North-West
        if (rank < 7 && file > 0) {
            attacks |= (1ULL << (square + 7));
        }
        // WHY +7: Up one rank (+8) and left one file (-1)
        
        // South-East
        if (rank > 0 && file < 7) {
            attacks |= (1ULL << (square - 7));
        }
        
        // South-West
        if (rank > 0 && file > 0) {
            attacks |= (1ULL << (square - 9));
        }
        
        kingAttacks[square] = attacks;
    }
}

// USAGE:
// uint64_t king_moves = kingAttacks[king_square];
// Takes 1 array lookup!
```

**Knight Attacks:**

```cpp
uint64_t knightAttacks[64];

void initKnightAttacks() {
    for (int square = 0; square < 64; square++) {
        uint64_t attacks = 0;
        int rank = square / 8;
        int file = square % 8;
        
        // A knight moves in an L-shape: 2 squares in one direction,
        // 1 square perpendicular
        
        // There are 8 possible L-shapes:
        
        // 2 up, 1 right
        if (rank < 6 && file < 7) {
            attacks |= (1ULL << (square + 17));
        }
        // WHY +17: Up 2 ranks (+16) and right 1 file (+1)
        // WHY rank < 6: Need room for 2 ranks up
        
        // 2 up, 1 left
        if (rank < 6 && file > 0) {
            attacks |= (1ULL << (square + 15));
        }
        
        // 2 right, 1 up
        if (file < 6 && rank < 7) {
            attacks |= (1ULL << (square + 10));
        }
        // WHY file < 6: Need room for 2 files right
        
        // 2 right, 1 down
        if (file < 6 && rank > 0) {
            attacks |= (1ULL << (square - 6));
        }
        
        // 2 down, 1 right
        if (rank > 1 && file < 7) {
            attacks |= (1ULL << (square - 15));
        }
        
        // 2 down, 1 left
        if (rank > 1 && file > 0) {
            attacks |= (1ULL << (square - 17));
        }
        
        // 2 left, 1 up
        if (file > 1 && rank < 7) {
            attacks |= (1ULL << (square + 6));
        }
        
        // 2 left, 1 down
        if (file > 1 && rank > 0) {
            attacks |= (1ULL << (square - 10));
        }
        
        knightAttacks[square] = attacks;
    }
}
```

**Pawn Attacks (direction-dependent):**

```cpp
// Pawn attacks are different for white and black
// AND they're different from pawn moves!

uint64_t GetPawnAttacks(const Position& pos, int square, bool white) {
    uint64_t attacks = 0;
    uint64_t bit = (1ULL << square);
    int file = square % 8;
    
    if (white) {
        // White pawns attack diagonally upward
        
        // Attack to the left (northwest)
        if (file > 0) {  // Not on a-file
            attacks |= (bit << 7);
        }
        // WHY << 7: Up one rank (+8) and left one file (-1) = +7
        
        // Attack to the right (northeast)
        if (file < 7) {  // Not on h-file
            attacks |= (bit << 9);
        }
        // WHY << 9: Up one rank (+8) and right one file (+1) = +9
    }
    else {
        // Black pawns attack diagonally downward
        
        // Attack to the left (southwest)
        if (file > 0) {
            attacks |= (bit >> 9);
        }
        
        // Attack to the right (southeast)
        if (file < 7) {
            attacks |= (bit >> 7);
        }
    }
    
    return attacks;
}

// WHY not pre-computed like king/knight:
// Could be, but there are only 2 cases (white/black)
// and it's fast enough to compute on-demand
```

### 3.3 Magic Bitboards - The Sliding Piece Trick

**THE PROBLEM:**

Bishops, rooks, and queens are "sliding pieces". Their attacks depend on blockers:

```
Example: Rook on e4 with various blockers

No blockers:           Blocker on e7:        Blockers on e7 and e2:
. . . . r . . .        . . . . r . . .       . . . . r . . .
. . . . x . . .        . . . . x . . .       . . . . x . . .
. . . . x . . .        . . . . x . . .       . . . . x . . .
. . . . x . . .        . . . . x . . .       . . . . x . . .
x x x x R x x x        x x x x R x x x       x x x x R x x x
. . . . x . . .        . . . . x . . .       . . . . x . . .
. . . . x . . .        . . . . x . . .       . . . . P . . .  <- blocker
. . . . x . . .        . . . . P . . .       . . . . . . . .
                                ^
Can attack all x's    Blocked by P          e2 and e7 blocked
```

Pre-computing all combinations would take ~2^64 entries PER SQUARE = impossible!

**THE SOLUTION: Magic Bitboards**

The key insight: We don't care about ALL squares, just the ones that matter.

For a rook on e4:
- We care about pieces on the e-file and 4th rank
- We DON'T care about pieces on a1, h8, etc.

**How Magic Bitboards Work:**

1. **For each square, identify relevant blockers**
   ```cpp
   For rook on e4:
   Relevant squares = e-file (except e4) + 4th rank (except e4)
                    = about 14 squares
   Possible blocker patterns = 2^14 = 16,384 (manageable!)
   ```

2. **Find a "magic number" that hashes blockers perfectly**
   ```cpp
   hash = (blockers * MAGIC) >> (64 - bits)
   
   This maps 2^14 blocker patterns to 2^14 unique indices
   ```

3. **Pre-compute attack bitboard for each pattern**
   ```cpp
   attacks[square][hash] = attack bitboard for this blocker pattern
   ```

4. **Lookup is instant**
   ```cpp
   uint64_t get_rook_attacks(int square, uint64_t occupied) {
       uint64_t blockers = occupied & rook_masks[square];
       uint64_t hash = (blockers * rook_magics[square]) >> (64 - rook_bits[square]);
       return rook_attacks[square][hash];
   }
   ```

**Complete Magic Bitboards Implementation:**

```cpp
// Magic numbers (found by trial and error, can use pre-computed ones)
const uint64_t rook_magics[64] = {
    0x0080001020400080ULL, 0x0040001000200040ULL, // ... 64 magic numbers
    // These are found by a brute-force search
    // Many implementations just use known good values
};

const int rook_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,  // Corners need 12 bits, edges need 11
    11, 10, 10, 10, 10, 10, 10, 11,  // Center squares need only 10 bits
    // ... pattern continues
};

// Attack tables
uint64_t rook_masks[64];            // Relevant squares for each square
uint64_t rook_attacks[64][4096];    // Pre-computed attacks

void init_magics() {
    for (int square = 0; square < 64; square++) {
        // STEP 1: Compute mask of relevant squares
        rook_masks[square] = compute_rook_mask(square);
        
        // STEP 2: For each possible blocker pattern...
        int num_patterns = 1 << rook_bits[square];  // 2^bits
        
        for (int pattern = 0; pattern < num_patterns; pattern++) {
            // Convert pattern index to actual blocker bitboard
            uint64_t blockers = index_to_bitboard(pattern, rook_masks[square]);
            
            // Compute what squares the rook can attack with these blockers
            uint64_t attacks = compute_rook_attacks_slow(square, blockers);
            
            // Hash the blockers
            uint64_t hash = (blockers * rook_magics[square]) >> (64 - rook_bits[square]);
            
            // Store in lookup table
            rook_attacks[square][hash] = attacks;
        }
    }
}

// Helper: Compute rook mask (relevant squares)
uint64_t compute_rook_mask(int square) {
    uint64_t mask = 0;
    int rank = square / 8;
    int file = square % 8;
    
    // Add all squares on the same rank (excluding edges and rook square)
    for (int f = 1; f < 7; f++) {  // Skip a-file and h-file
        if (f != file) {
            mask |= (1ULL << (rank * 8 + f));
        }
    }
    
    // Add all squares on the same file (excluding edges and rook square)
    for (int r = 1; r < 7; r++) {  // Skip rank 1 and rank 8
        if (r != rank) {
            mask |= (1ULL << (r * 8 + file));
        }
    }
    
    // WHY exclude edges:
    // Edge squares don't affect where the rook can move
    // (rook always attacks to the edge)
    
    return mask;
}

// Helper: Compute attacks for a blocker configuration (slow but accurate)
uint64_t compute_rook_attacks_slow(int square, uint64_t blockers) {
    uint64_t attacks = 0;
    int rank = square / 8;
    int file = square % 8;
    
    // Cast ray in each direction until hitting a blocker
    
    // North
    for (int r = rank + 1; r < 8; r++) {
        int sq = r * 8 + file;
        attacks |= (1ULL << sq);
        if (blockers & (1ULL << sq)) break;  // Hit a blocker
    }
    
    // South
    for (int r = rank - 1; r >= 0; r--) {
        int sq = r * 8 + file;
        attacks |= (1ULL << sq);
        if (blockers & (1ULL << sq)) break;
    }
    
    // East
    for (int f = file + 1; f < 8; f++) {
        int sq = rank * 8 + f;
        attacks |= (1ULL << sq);
        if (blockers & (1ULL << sq)) break;
    }
    
    // West
    for (int f = file - 1; f >= 0; f--) {
        int sq = rank * 8 + f;
        attacks |= (1ULL << sq);
        if (blockers & (1ULL << sq)) break;
    }
    
    return attacks;
}

// Same approach for bishops (diagonal rays) and queens (rooks + bishops)
```

**Why Magic Bitboards are Amazing:**

```cpp
// OLD WAY: Compute attacks every time (slow)
for each direction {
    scan until blocker or edge
}
// 10+ operations per call

// MAGIC WAY: One lookup
uint64_t attacks = rook_attacks[square][(occupied & rook_masks[square]) * MAGIC >> shift];
// 3 operations: AND, multiply, shift, array access
// Uses CPU's multiply and shift instructions = FAST
```

### 3.4 Complete Move Generation - All Pieces

Now we put it all together to generate moves for every piece type.

#### Pawn Moves - The Most Complex

**WHY pawns are special:**
1. Move forward but capture diagonally
2. Can move 2 squares on first move
3. Promote on the 8th/1st rank
4. Can capture en passant

**Complete pawn move generation:**

```cpp
void generate_pawn_moves(Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t pawns = white ? pos.WhitePawns : pos.BlackPawns;
    int forward = white ? 8 : -8;           // Direction: +8 for white, -8 for black
    int start_rank = white ? 1 : 6;         // Rank where pawns start
    int promo_rank = white ? 7 : 0;         // Rank where pawns promote
    
    // WHY we need these: Pawns behave differently based on color
    
    while (pawns) {
        int from = __builtin_ctzll(pawns);
        int from_rank = from / 8;
        int to_square = from + forward;
        int to_rank = to_square / 8;
        
        // === SINGLE PUSH ===
        if (!(pos.occupiedSquares & (1ULL << to_square))) {
            // Square in front is empty
            
            if (to_rank == promo_rank) {
                // PROMOTION - Generate 4 moves (Q, R, B, N)
                if (white) {
                    moves.add({from, to_square, W_QUEEN});
                    moves.add({from, to_square, W_ROOK});
                    moves.add({from, to_square, W_BISHOP});
                    moves.add({from, to_square, W_KNIGHT});
                } else {
                    moves.add({from, to_square, B_QUEEN});
                    moves.add({from, to_square, B_ROOK});
                    moves.add({from, to_square, B_BISHOP});
                    moves.add({from, to_square, B_KNIGHT});
                }
                // WHY 4 moves: Rules allow promoting to any piece
                // WHY not king: Can't promote to king
            } else {
                // Normal push
                moves.add({from, to_square});
            }
            
            // === DOUBLE PUSH ===
            if (from_rank == start_rank) {
                // Pawn is on starting rank
                int double_square = from + (2 * forward);
                
                if (!(pos.occupiedSquares & (1ULL << double_square))) {
                    // Both squares in front are empty
                    moves.add({from, double_square});
                    // WHY: First move can be 2 squares
                }
            }
        }
        
        // === CAPTURES ===
        uint64_t attacks = GetPawnAttacks(pos, from, white);
        uint64_t enemy = white ? pos.BlackoccupiedSquares : pos.WhiteoccupiedSquares;
        uint64_t capture_targets = attacks & enemy;
        
        while (capture_targets) {
            int capture_sq = __builtin_ctzll(capture_targets);
            int capture_rank = capture_sq / 8;
            
            if (capture_rank == promo_rank) {
                // Promotion capture
                if (white) {
                    moves.add({from, capture_sq, W_QUEEN});
                    moves.add({from, capture_sq, W_ROOK});
                    moves.add({from, capture_sq, W_BISHOP});
                    moves.add({from, capture_sq, W_KNIGHT});
                } else {
                    moves.add({from, capture_sq, B_QUEEN});
                    moves.add({from, capture_sq, B_ROOK});
                    moves.add({from, capture_sq, B_BISHOP});
                    moves.add({from, capture_sq, B_KNIGHT});
                }
            } else {
                // Normal capture
                moves.add({from, capture_sq});
            }
            
            capture_targets &= capture_targets - 1;
        }
        
        // === EN PASSANT ===
        if (pos.enPassant != 0) {
            // There's an en passant square available
            if (attacks & pos.enPassant) {
                // This pawn can capture it
                int ep_square = __builtin_ctzll(pos.enPassant);
                moves.add({from, ep_square});
                
                // WHY: En passant is a special capture where you capture
                // a pawn that just moved 2 squares
            }
        }
        
        pawns &= pawns - 1;  // Remove this pawn and continue
    }
}

// WHY this is complex:
// - 4 types of moves (push, double push, capture, en passant)
// - Promotions generate 4 moves each
// - Different behavior for white/black
```

#### Knight Moves - Simple Pattern

```cpp
void generate_knight_moves(Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t knights = white ? pos.WhiteKnights : pos.BlackKnights;
    uint64_t own_pieces = white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
    
    while (knights) {
        int from = __builtin_ctzll(knights);
        
        // Get all squares the knight can move to
        uint64_t targets = knightAttacks[from];
        
        // Remove squares occupied by our own pieces
        targets &= ~own_pieces;
        
        // WHY: Can't capture own pieces
        
        while (targets) {
            int to = __builtin_ctzll(targets);
            moves.add({from, to});
            targets &= targets - 1;
        }
        
        knights &= knights - 1;
    }
}

// WHY this is simple:
// - Knights jump over pieces (don't need blocker checks)
// - Pre-computed attack table makes it fast
// - Same pattern for white and black
```

#### Bishop Moves - Use Magic Bitboards

```cpp
void generate_bishop_moves(Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    uint64_t bishops = white ? pos.WhiteBishops : pos.BlackBishops;
    uint64_t own_pieces = white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
    
    while (bishops) {
        int from = __builtin_ctzll(bishops);
        
        // Get bishop attacks using magic bitboards
        uint64_t attacks = get_bishop_attacks(from, pos.occupiedSquares);
        
        // WHY occupiedSquares: Bishop attacks depend on all blockers
        
        // Remove our own pieces
        uint64_t targets = attacks & ~own_pieces;
        
        while (targets) {
            int to = __builtin_ctzll(targets);
            moves.add({from, to});
            targets &= targets - 1;
        }
        
        bishops &= bishops - 1;
    }
}

// Same pattern for rooks and queens
```

#### Castling - Special King Move

**Castling rules (complex!):**
1. King and rook haven't moved (check castling rights)
2. Squares between king and rook are empty
3. King is not in check
4. King doesn't pass through check
5. King doesn't land in check

```cpp
void generate_castling_moves(Position& pos, MoveList& moves) {
    if (pos.whiteToMove) {
        // === WHITE KINGSIDE (e1-g1) ===
        if ((pos.castelingRights & 0x1) &&                    // Has the right
            !(pos.occupiedSquares & 0x60ULL) &&               // f1 and g1 empty
            !is_square_attacked(pos, 4, false) &&             // e1 not in check
            !is_square_attacked(pos, 5, false) &&             // f1 not attacked
            !is_square_attacked(pos, 6, false)) {             // g1 not attacked
            
            moves.add({4, 6});  // King from e1 to g1
            
            // WHY check f1: King passes through f1
            // WHY not check h1: Rook can be attacked
        }
        
        // === WHITE QUEENSIDE (e1-c1) ===
        if ((pos.castelingRights & 0x2) &&                    // Has the right
            !(pos.occupiedSquares & 0xEULL) &&                // b1, c1, d1 empty
            !is_square_attacked(pos, 4, false) &&             // e1 not in check
            !is_square_attacked(pos, 3, false) &&             // d1 not attacked
            !is_square_attacked(pos, 2, false)) {             // c1 not attacked
            
            moves.add({4, 2});  // King from e1 to c1
            
            // WHY 0xE = 0b1110: Bits 1, 2, 3 = b1, c1, d1
            // WHY not check a1 or b1: Rook can be attacked, b1 can be attacked
        }
    } else {
        // === BLACK KINGSIDE (e8-g8) ===
        if ((pos.castelingRights & 0x4) &&
            !(pos.occupiedSquares & 0x6000000000000000ULL) &&
            !is_square_attacked(pos, 60, true) &&
            !is_square_attacked(pos, 61, true) &&
            !is_square_attacked(pos, 62, true)) {
            
            moves.add({60, 62});
        }
        
        // === BLACK QUEENSIDE (e8-c8) ===
        if ((pos.castelingRights & 0x8) &&
            !(pos.occupiedSquares & 0xE00000000000000ULL) &&
            !is_square_attacked(pos, 60, true) &&
            !is_square_attacked(pos, 59, true) &&
            !is_square_attacked(pos, 58, true)) {
            
            moves.add({60, 58});
        }
    }
}

// WHY castling is special:
// - Only move where two pieces move at once
// - Unique in chess rules
// - Important for king safety in opening
```

#### King Moves

```cpp
void generate_king_moves(Position& pos, MoveList& moves) {
    bool white = pos.whiteToMove;
    int from = __builtin_ctzll(white ? pos.WhiteKing : pos.BlackKing);
    uint64_t own_pieces = white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares;
    
    uint64_t attacks = kingAttacks[from];
    uint64_t targets = attacks & ~own_pieces;
    
    while (targets) {
        int to = __builtin_ctzll(targets);
        moves.add({from, to});
        targets &= targets - 1;
    }
}
```

### 3.5 Checking if a Square is Attacked

**WHY we need this:**
- Validate king moves (can't move into check)
- Detect checkmate/stalemate
- Castling legality

```cpp
bool is_square_attacked(const Position& pos, int square, bool by_white) {
    uint64_t square_bb = (1ULL << square);
    
    if (by_white) {
        // Check if white pieces attack this square
        
        // Pawns
        uint64_t pawn_attacks = GetPawnAttacks(pos, square, false);  // Black pawn perspective
        if (pawn_attacks & pos.WhitePawns) return true;
        
        // WHY false: We want to know where black pawns would be
        // to attack this square, then check if white pawns are there
        
        // Knights
        if (knightAttacks[square] & pos.WhiteKnights) return true;
        
        // Bishops and Queens (diagonal)
        uint64_t bishop_attacks = get_bishop_attacks(square, pos.occupiedSquares);
        if (bishop_attacks & (pos.WhiteBishops | pos.WhiteQueen)) return true;
        
        // Rooks and Queens (straight)
        uint64_t rook_attacks = get_rook_attacks(square, pos.occupiedSquares);
        if (rook_attacks & (pos.WhiteRooks | pos.WhiteQueen)) return true;
        
        // King
        if (kingAttacks[square] & pos.WhiteKing) return true;
    }
    else {
        // Check if black pieces attack this square (same logic)
        uint64_t pawn_attacks = GetPawnAttacks(pos, square, true);
        if (pawn_attacks & pos.BlackPawns) return true;
        
        if (knightAttacks[square] & pos.BlackKnights) return true;
        
        uint64_t bishop_attacks = get_bishop_attacks(square, pos.occupiedSquares);
        if (bishop_attacks & (pos.BlackBishops | pos.BlackQueen)) return true;
        
        uint64_t rook_attacks = get_rook_attacks(square, pos.occupiedSquares);
        if (rook_attacks & (pos.BlackRooks | pos.BlackQueen)) return true;
        
        if (kingAttacks[square] & pos.BlackKing) return true;
    }
    
    return false;
}

// OPTIMIZATION: This is called frequently!
// Many engines cache attack bitboards per position
```

### 3.6 Legality Check - Final Validation

**Pseudo-legal vs Legal:**
- **Pseudo-legal**: Looks legal but might leave king in check
- **Legal**: Actually legal (king not in check after move)

```cpp
void generate_legal_moves(Position& pos, MoveList& legal) {
    MoveList pseudo_legal;
    
    // Step 1: Generate all pseudo-legal moves
    generate_pawn_moves(pos, pseudo_legal);
    generate_knight_moves(pos, pseudo_legal);
    generate_bishop_moves(pos, pseudo_legal);
    generate_rook_moves(pos, pseudo_legal);
    generate_queen_moves(pos, pseudo_legal);
    generate_king_moves(pos, pseudo_legal);
    generate_castling_moves(pos, pseudo_legal);
    
    // Step 2: Filter out illegal moves
    for (int i = 0; i < pseudo_legal.count; i++) {
        Move m = pseudo_legal.moves[i];
        
        // Make the move
        makemove(pos, m);
        
        // Find our king (after the move, it's the opponent's turn)
        bool we_are_white = !pos.whiteToMove;  // Flipped after move
        int king_sq = __builtin_ctzll(we_are_white ? pos.WhiteKing : pos.BlackKing);
        
        // Is our king attacked?
        bool king_attacked = is_square_attacked(pos, king_sq, pos.whiteToMove);
        
        // Undo the move
        undomove(pos, m);
        
        // If king is NOT attacked, move is legal
        if (!king_attacked) {
            legal.add(m);
        }
    }
}

// WHY this works:
// 1. Generate all possible-looking moves
// 2. Try each one
// 3. See if our king is left in check
// 4. If not, move is legal
```

---

## Chapter 4: Making and Unmaking Moves

### 4.1 Why Make/Unmake is Critical

**The Challenge:**
- Search explores millions of positions
- Each position needs to be evaluated
- Must be able to try a move, then undo it perfectly

**WHY we need perfect undo:**
```
Position A
  Make move M -> Position B
    Make move N -> Position C
    Unmake move N -> Must get exactly Position B back!
  Unmake move M -> Must get exactly Position A back!
```

If undo is wrong, the search gets corrupted and the engine plays illegal moves!

### 4.2 What Needs to be Saved

```cpp
struct UndoInfo {
    // Everything that changes and can't be recomputed
    uint64_t oldHashKey;
    uint8_t oldCastelingRights;
    uint64_t oldEnPassant;
    int oldHalfMove;
    
    // Capture information
    Pieces oldCapturedPiece;
    
    // Special move flags
    bool isEnPassant;
    int enPassantCapturedPawnSquare;
    
    bool isCastling;
    int castlingRookFrom;
    int castlingRookTo;
    
    // Moved piece (for undo)
    Pieces movedPiece;
    bool side;  // Who moved
};

// WHY each field:
// - oldHashKey: Zobrist key can't be recomputed from scratch (slow)
// - oldCastelingRights: Changes when king/rook moves
// - oldEnPassant: Only valid for one move
// - oldHalfMove: For 50-move rule
// - oldCapturedPiece: Need to put it back
// - isEnPassant: Special capture (remove pawn from different square)
// - isCastling: Need to move rook back too
// - movedPiece: Need to know what moved
```

### 4.3 Making a Move - Step by Step

```cpp
// Global array to store undo information
UndoInfo history[1024];  // Max ply in search
int history_ply = 0;

void makemove(Position& pos, Move m) {
    // ========================================
    // STEP 1: SAVE UNDO INFORMATION
    // ========================================
    UndoInfo& undo = history[history_ply++];
    
    undo.oldHashKey = pos.zobrist_key;
    undo.oldCastelingRights = pos.castelingRights;
    undo.oldEnPassant = pos.enPassant;
    undo.oldHalfMove = pos.move50rule;
    undo.side = pos.whiteToMove;
    
    // WHY save these: They change and can't be easily recomputed
    
    // ========================================
    // STEP 2: UPDATE ZOBRIST HASH (remove old state)
    // ========================================
    pos.zobrist_key ^= zobrist_side;  // Toggle side to move
    
    if (pos.enPassant) {
        pos.zobrist_key ^= zobrist_ep[__builtin_ctzll(pos.enPassant)];
    }
    
    pos.zobrist_key ^= zobrist_castle[pos.castelingRights];
    
    // WHY XOR: Zobrist hashing uses XOR to add/remove pieces
    // XOR has property: A ^ B ^ B = A (undoes itself)
    
    // ========================================
    // STEP 3: CLEAR EN PASSANT
    // ========================================
    pos.enPassant = 0;  // Will be set later if pawn double push
    
    // ========================================
    // STEP 4: IDENTIFY MOVE TYPE
    // ========================================
    uint64_t from_bb = (1ULL << m.from);
    uint64_t to_bb = (1ULL << m.to);
    
    undo.movedPiece = get_piece_at(pos, m.from);
    undo.oldCapturedPiece = get_piece_at(pos, m.to);
    
    // ========================================
    // STEP 5: HANDLE CAPTURE
    // ========================================
    if (undo.oldCapturedPiece != NO_PIECE) {
        // It's a capture - remove captured piece
        remove_piece_from_bitboard(pos, undo.oldCapturedPiece, m.to);
        pos.zobrist_key ^= zobrist_piece[undo.oldCapturedPiece][m.to];
        pos.move50rule = 0;  // Capture resets 50-move rule
        
        // Update castling rights if we captured a rook on its starting square
        if (undo.oldCapturedPiece == W_ROOK) {
            if (m.to == 0) pos.castelingRights &= ~0x2;  // a1 rook
            if (m.to == 7) pos.castelingRights &= ~0x1;  // h1 rook
        }
        if (undo.oldCapturedPiece == B_ROOK) {
            if (m.to == 56) pos.castelingRights &= ~0x8;  // a8 rook
            if (m.to == 63) pos.castelingRights &= ~0x4;  // h8 rook
        }
        
        // WHY: If we capture the rook, opponent can't castle that side anymore
    } else {
        pos.move50rule++;  // No capture, increment halfmove clock
    }
    
    // ========================================
    // STEP 6: MOVE THE PIECE
    // ========================================
    remove_piece_from_bitboard(pos, undo.movedPiece, m.from);
    pos.zobrist_key ^= zobrist_piece[undo.movedPiece][m.from];
    
    if (m.promotion != NO_PIECE) {
        // It's a promotion - add promoted piece, not pawn
        add_piece_to_bitboard(pos, m.promotion, m.to);
        pos.zobrist_key ^= zobrist_piece[m.promotion][m.to];
        pos.move50rule = 0;  // Pawn move resets 50-move rule
    } else {
        // Normal move - add same piece to destination
        add_piece_to_bitboard(pos, undo.movedPiece, m.to);
        pos.zobrist_key ^= zobrist_piece[undo.movedPiece][m.to];
    }
    
    // ========================================
    // STEP 7: SPECIAL MOVES
    // ========================================
    
    undo.isEnPassant = false;
    undo.isCastling = false;
    
    // EN PASSANT CAPTURE
    if (get_piece_type(undo.movedPiece) == PAWN && pos.enPassant && (to_bb == undo.oldEnPassant)) {
        // Pawn moved to en passant square
        undo.isEnPassant = true;
        
        // Remove captured pawn (it's not on m.to, it's one rank different!)
        int captured_pawn_sq = m.to + (pos.whiteToMove ? -8 : 8);
        undo.enPassantCapturedPawnSquare = captured_pawn_sq;
        
        Pieces captured_pawn = pos.whiteToMove ? B_PAWN : W_PAWN;
        remove_piece_from_bitboard(pos, captured_pawn, captured_pawn_sq);
        pos.zobrist_key ^= zobrist_piece[captured_pawn][captured_pawn_sq];
        
        // WHY different square: En passant captures pawn beside you, not in front
    }
    
    // PAWN DOUBLE PUSH - SET EN PASSANT SQUARE
    if (get_piece_type(undo.movedPiece) == PAWN && abs(m.from - m.to) == 16) {
        // Pawn moved 2 squares
        pos.enPassant = (1ULL << (m.from + (m.to - m.from) / 2));
        pos.zobrist_key ^= zobrist_ep[__builtin_ctzll(pos.enPassant)];
        pos.move50rule = 0;  // Pawn move resets 50-move rule
        
        // WHY: Set en passant square to the square it jumped over
        // Example: e2-e4, en passant square is e3
    }
    
    // CASTLING
    if (get_piece_type(undo.movedPiece) == KING && abs(m.from - m.to) == 2) {
        // King moved 2 squares - must be castling
        undo.isCastling = true;
        
        int rook_from, rook_to;
        
        if (m.to == 6) {  // White kingside
            rook_from = 7; rook_to = 5;
        } else if (m.to == 2) {  // White queenside
            rook_from = 0; rook_to = 3;
        } else if (m.to == 62) {  // Black kingside
            rook_from = 63; rook_to = 61;
        } else {  // Black queenside
            rook_from = 56; rook_to = 59;
        }
        
        undo.castlingRookFrom = rook_from;
        undo.castlingRookTo = rook_to;
        
        // Move the rook
        Pieces rook = pos.whiteToMove ? W_ROOK : B_ROOK;
        remove_piece_from_bitboard(pos, rook, rook_from);
        add_piece_to_bitboard(pos, rook, rook_to);
        
        pos.zobrist_key ^= zobrist_piece[rook][rook_from];
        pos.zobrist_key ^= zobrist_piece[rook][rook_to];
    }
    
    // ========================================
    // STEP 8: UPDATE CASTLING RIGHTS
    // ========================================
    if (undo.movedPiece == W_KING) {
        pos.castelingRights &= ~0x3;  // White loses both castling rights
    } else if (undo.movedPiece == B_KING) {
        pos.castelingRights &= ~0xC;  // Black loses both castling rights
    } else if (undo.movedPiece == W_ROOK) {
        if (m.from == 0) pos.castelingRights &= ~0x2;  // a1 rook moved
        if (m.from == 7) pos.castelingRights &= ~0x1;  // h1 rook moved
    } else if (undo.movedPiece == B_ROOK) {
        if (m.from == 56) pos.castelingRights &= ~0x8;  // a8 rook moved
        if (m.from == 63) pos.castelingRights &= ~0x4;  // h8 rook moved
    }
    
    pos.zobrist_key ^= zobrist_castle[pos.castelingRights];
    
    // ========================================
    // STEP 9: SWITCH SIDES
    // ========================================
    pos.whiteToMove = !pos.whiteToMove;
    pos.zobrist_key ^= zobrist_side;
    
    // ========================================
    // STEP 10: UPDATE COMBINED BITBOARDS
    // ========================================
    update_occupancy_bitboards(pos);
}

// WHY this is complex:
// - Many special cases (castling, en passant, promotion)
// - Must update hash incrementally (for speed)
// - Must save everything needed for undo
```

### 4.4 Unmaking a Move - Perfect Reversal

```cpp
void undomove(Position& pos, Move m) {
    // Retrieve undo information
    UndoInfo& undo = history[--history_ply];
    
    // WHY --history_ply: Decrement first, then access
    // This gets the UndoInfo we saved in makemove
    
    // ========================================
    // STEP 1: SWITCH SIDES BACK
    // ========================================
    pos.whiteToMove = undo.side;
    
    // ========================================
    // STEP 2: UNDO CASTLING
    // ========================================
    if (undo.isCastling) {
        // Move rook back
        Pieces rook = pos.whiteToMove ? W_ROOK : B_ROOK;
        
        remove_piece_from_bitboard(pos, rook, undo.castlingRookTo);
        add_piece_to_bitboard(pos, rook, undo.castlingRookFrom);
        
        // WHY: Reverse the rook movement
    }
    
    // ========================================
    // STEP 3: MOVE PIECE BACK
    // ========================================
    if (m.promotion != NO_PIECE) {
        // It was a promotion - remove promoted piece, add pawn back
        remove_piece_from_bitboard(pos, m.promotion, m.to);
        add_piece_to_bitboard(pos, undo.movedPiece, m.from);
    } else {
        // Normal move - move piece back
        remove_piece_from_bitboard(pos, undo.movedPiece, m.to);
        add_piece_to_bitboard(pos, undo.movedPiece, m.from);
    }
    
    // ========================================
    // STEP 4: RESTORE CAPTURED PIECE
    // ========================================
    if (undo.oldCapturedPiece != NO_PIECE) {
        add_piece_to_bitboard(pos, undo.oldCapturedPiece, m.to);
    }
    
    // ========================================
    // STEP 5: UNDO EN PASSANT
    // ========================================
    if (undo.isEnPassant) {
        // Restore the captured pawn
        Pieces captured_pawn = pos.whiteToMove ? B_PAWN : W_PAWN;
        add_piece_to_bitboard(pos, captured_pawn, undo.enPassantCapturedPawnSquare);
        
        // WHY: En passant capture removed pawn from different square
    }
    
    // ========================================
    // STEP 6: RESTORE GAME STATE
    // ========================================
    pos.zobrist_key = undo.oldHashKey;
    pos.castelingRights = undo.oldCastelingRights;
    pos.enPassant = undo.oldEnPassant;
    pos.move50rule = undo.oldHalfMove;
    
    // WHY just restore: Easier than recomputing
    // Hash would be very slow to recompute
    
    // ========================================
    // STEP 7: UPDATE COMBINED BITBOARDS
    // ========================================
    update_occupancy_bitboards(pos);
}

// CRITICAL: After unmake, position MUST be identical to before make!
// Any difference = bug that will corrupt search
```

**Testing make/unmake:**

```cpp
void test_make_unmake() {
    Position pos;
    pos.setStartingPosition();
    
    uint64_t original_hash = pos.zobrist_key;
    
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves[i]);
        undomove(pos, moves[i]);
        
        // Position should be identical
        assert(pos.zobrist_key == original_hash);
        
        // WHY this test is critical:
        // If undo is broken, search will give wrong results
    }
}
```

---

## Chapter 5: Position Evaluation

### 5.1 The Evaluation Function - Heart of the Engine

**What is evaluation?**
A function that returns a score representing who's winning:
- Positive = White is better
- Negative = Black is better  
- 0 = Equal position

**Evaluation is used in:**
- Leaf nodes of search tree
- Quiescence search
- Move ordering

### 5.2 Material - The Foundation

**Piece values (in centipawns):**

```cpp
const int PIECE_VALUES[13] = {
    320,  // W_KNIGHT
    330,  // W_BISHOP
    500,  // W_ROOK
    100,  // W_PAWN
    900,  // W_QUEEN
    0,    // W_KING (infinite value)
    320,  // B_KNIGHT
    330,  // B_BISHOP
    500,  // B_ROOK
    100,  // B_PAWN
    900,  // B_QUEEN
    0,    // B_KING
    0     // NO_PIECE
};

// WHY these values:
// - Based on decades of chess knowledge
// - Bishop slightly > Knight (bishop pair is strong)
// - Rook = 5 pawns
// - Queen = 9 pawns
```

**Simple material evaluation:**

```cpp
int evaluate_material(const Position& pos) {
    int score = 0;
    
    // Count white pieces
    score += __builtin_popcountll(pos.WhitePawns) * 100;
    score += __builtin_popcountll(pos.WhiteKnights) * 320;
    score += __builtin_popcountll(pos.WhiteBishops) * 330;
    score += __builtin_popcountll(pos.WhiteRooks) * 500;
    score += __builtin_popcountll(pos.WhiteQueen) * 900;
    
    // Count black pieces
    score -= __builtin_popcountll(pos.BlackPawns) * 100;
    score -= __builtin_popcountll(pos.BlackKnights) * 320;
    score -= __builtin_popcountll(pos.BlackBishops) * 330;
    score -= __builtin_popcountll(pos.BlackRooks) * 500;
    score -= __builtin_popcountll(pos.BlackQueen) * 900;
    
    // Return from white's perspective
    return pos.whiteToMove ? score : -score;
}

// WHY __builtin_popcountll: Fast bit counting
// WHY negate if black: Evaluation always from side to move's perspective
```

### 5.3 Piece-Square Tables - Position Matters

**THE INSIGHT:** A knight on e4 is worth more than a knight on a1!

**Piece-Square Table (PST) concept:**
```
Assign bonus/penalty based on where piece is located

Example - Knight PST (from white's perspective):
  a   b   c   d   e   f   g   h
8 -50 -40 -30 -30 -30 -30 -40 -50
7 -40 -20   0   0   0   0 -20 -40
6 -30   0  10  15  15  10   0 -30
5 -30   5  15  20  20  15   5 -30
4 -30   0  15  20  20  15   0 -30
3 -30   5  10  15  15  10   5 -30
2 -40 -20   0   5   5   0 -20 -40
1 -50 -40 -30 -30 -30 -30 -40 -50

WHY this pattern:
- Knights on the rim are dim (edges are bad)
- Center squares are strong
- e4/d4/e5/d5 are ideal
```

**Complete PST implementation:**

```cpp
// Pawn PST (encourage advancement and center control)
const int pawn_pst[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,  // Rank 1 (pawns never here)
     50,  50,  50,  50,  50,  50,  50,  50,  // Rank 2 (starting rank)
     10,  10,  20,  30,  30,  20,  10,  10,  // Rank 3
      5,   5,  10,  25,  25,  10,   5,   5,  // Rank 4
      0,   0,   0,  20,  20,   0,   0,   0,  // Rank 5 (central pawns)
      5,  -5, -10,   0,   0, -10,  -5,   5,  // Rank 6
      5,  10,  10, -20, -20,  10,  10,   5,  // Rank 7 (discourage early h/g moves)
      0,   0,   0,   0,   0,   0,   0,   0   // Rank 8 (promotion)
};

// WHY each rank:
// - Rank 2: Bonus for pawns still home (pawn structure)
// - Ranks 3-5: Encourage central advancement
// - Rank 6: Slight penalty for overextension
// - Rank 7: Penalty for h/g pawns (weaken kingside)

// Knight PST
const int knight_pst[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

// Bishop PST (prefer long diagonals)
const int bishop_pst[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

// King PST (different for middlegame vs endgame!)
const int king_middlegame_pst[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,   0,   0,   0,   0,  20,  20,  // Rank 2 (castled)
     20,  30,  10,   0,   0,  10,  30,  20   // Rank 1 (encourage castling)
};

// WHY king in corner: Safety in middlegame
// Rank 1-2 encouraged = castling position

const int king_endgame_pst[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
};

// WHY centralized in endgame: King is strong attacker
// Central king helps shepherd passed pawns

int evaluate_pst(const Position& pos) {
    int score = 0;
    
    // White pawns
    uint64_t pawns = pos.WhitePawns;
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        score += 100 + pawn_pst[sq];  // Base value + position bonus
        pawns &= pawns - 1;
    }
    
    // Black pawns (mirror the table vertically)
    pawns = pos.BlackPawns;
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        score -= 100 + pawn_pst[63 - sq];  // Mirror: rank 8 becomes rank 1
        pawns &= pawns - 1;
    }
    
    // WHY 63 - sq: Mirrors vertically
    // sq=0 (a1) -> 63 (a8)
    // sq=8 (a2) -> 55 (a7)
    
    // Same for other pieces...
    
    return pos.whiteToMove ? score : -score;
}
```

### 5.4 Advanced Evaluation Terms

**Beyond material and position, strong engines consider:**

#### 5.4.1 Mobility

```cpp
int evaluate_mobility(const Position& pos) {
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    int mobility = moves.count;
    
    // WHY: More moves = more options = better position
    // Cramped positions have few legal moves
    
    return mobility * 10;  // Each move worth ~10 centipawns
}
```

#### 5.4.2 Pawn Structure

**Doubled Pawns (bad):**
```cpp
int evaluate_doubled_pawns(const Position& pos) {
    int penalty = 0;
    
    for (int file = 0; file < 8; file++) {
        // Count pawns on this file
        uint64_t file_mask = 0x0101010101010101ULL << file;
        int white_pawns = __builtin_popcountll(pos.WhitePawns & file_mask);
        
        if (white_pawns > 1) {
            penalty -= (white_pawns - 1) * 20;  // -20 per doubled pawn
        }
        
        // WHY doubled pawns are bad:
        // - They can't protect each other
        // - Less flexible
        // - Harder to advance
    }
    
    // Same for black...
    
    return penalty;
}
```

**Passed Pawns (good):**
```cpp
int evaluate_passed_pawns(const Position& pos) {
    int bonus = 0;
    uint64_t white_pawns = pos.WhitePawns;
    
    while (white_pawns) {
        int sq = __builtin_ctzll(white_pawns);
        int file = sq % 8;
        int rank = sq / 8;
        
        // Check if any black pawn can stop it
        uint64_t blocking_mask = 0;
        
        for (int r = rank + 1; r < 8; r++) {
            // Check three files: left, same, right
            if (file > 0) blocking_mask |= (1ULL << (r * 8 + file - 1));
            blocking_mask |= (1ULL << (r * 8 + file));
            if (file < 7) blocking_mask |= (1ULL << (r * 8 + file + 1));
        }
        
        if (!(blocking_mask & pos.BlackPawns)) {
            // No black pawn can stop it - it's passed!
            int distance_to_promotion = 7 - rank;
            bonus += 50 + (7 - distance_to_promotion) * 10;
            
            // WHY increasing bonus: Closer to promotion = more dangerous
            // A pawn on rank 6 is worth ~120 centipawns extra
        }
        
        white_pawns &= white_pawns - 1;
    }
    
    // Same for black...
    
    return bonus;
}
```

**Isolated Pawns (bad):**
```cpp
int evaluate_isolated_pawns(const Position& pos) {
    int penalty = 0;
    
    for (int file = 0; file < 8; file++) {
        uint64_t file_mask = 0x0101010101010101ULL << file;
        
        if (pos.WhitePawns & file_mask) {
            // There's a white pawn on this file
            
            // Check adjacent files for friendly pawns
            bool has_neighbor = false;
            
            if (file > 0) {
                uint64_t left_file = file_mask >> 1;
                if (pos.WhitePawns & left_file) has_neighbor = true;
            }
            
            if (file < 7) {
                uint64_t right_file = file_mask << 1;
                if (pos.WhitePawns & right_file) has_neighbor = true;
            }
            
            if (!has_neighbor) {
                penalty -= 15;  // Isolated pawn penalty
                
                // WHY isolated pawns are bad:
                // - Can't be defended by other pawns
                // - Easier to attack
                // - Creates weak squares
            }
        }
    }
    
    // Same for black...
    
    return penalty;
}
```

#### 5.4.3 King Safety

```cpp
int evaluate_king_safety(const Position& pos) {
    int safety = 0;
    
    // White king safety
    int wking_sq = __builtin_ctzll(pos.WhiteKing);
    int wking_file = wking_sq % 8;
    int wking_rank = wking_sq / 8;
    
    // Bonus for pawns in front of king (pawn shield)
    if (wking_rank < 3) {  // King is on back ranks
        for (int file = max(0, wking_file - 1); file <= min(7, wking_file + 1); file++) {
            uint64_t file_mask = 0x0101010101010101ULL << file;
            
            // Check for pawns on rank 2 or 3
            if (pos.WhitePawns & file_mask & 0xFF00) {  // Rank 2
                safety += 20;
            }
            if (pos.WhitePawns & file_mask & 0xFF0000) {  // Rank 3
                safety += 10;
            }
        }
        
        // WHY: Pawns shield king from attacks
        // Missing pawn = weakness (open file to king)
    }
    
    // Penalty for enemy pieces near king
    uint64_t king_zone = kingAttacks[wking_sq];
    
    // Extend to 2-square radius
    uint64_t extended_zone = king_zone;
    for (int i = 0; i < 64; i++) {
        if (king_zone & (1ULL << i)) {
            extended_zone |= kingAttacks[i];
        }
    }
    
    // Count enemy pieces in king zone
    int attackers = 0;
    attackers += __builtin_popcountll(pos.BlackKnights & extended_zone);
    attackers += __builtin_popcountll(pos.BlackBishops & extended_zone);
    attackers += __builtin_popcountll(pos.BlackRooks & extended_zone);
    attackers += __builtin_popcountll(pos.BlackQueen & extended_zone);
    
    safety -= attackers * 30;  // Each attacker is dangerous
    
    // WHY: Enemy pieces near king = potential attack
    
    // Same for black king...
    
    return safety;
}
```

### 5.5 Tapered Evaluation - Middlegame vs Endgame

**THE PROBLEM:** Some things change value as game progresses
- King: Should hide in middlegame, fight in endgame
- Rooks: More valuable in open endgame
- Knights: Better in closed middlegame

**SOLUTION: Tapered evaluation**

```cpp
// Calculate game phase (0 = endgame, 256 = opening)
int calculate_game_phase(const Position& pos) {
    int phase = 0;
    
    phase += __builtin_popcountll(pos.WhiteKnights | pos.BlackKnights) * 1;
    phase += __builtin_popcountll(pos.WhiteBishops | pos.BlackBishops) * 1;
    phase += __builtin_popcountll(pos.WhiteRooks | pos.BlackRooks) * 2;
    phase += __builtin_popcountll(pos.WhiteQueen | pos.BlackQueen) * 4;
    
    // WHY these weights:
    // Queens disappear first usually
    // Total in starting position = 4+4+4+8 = 24
    
    // Normalize to 0-256
    phase = (phase * 256 + 12) / 24;
    
    return min(phase, 256);
}

int tapered_eval(const Position& pos) {
    int mg_score = 0;  // Middlegame score
    int eg_score = 0;  // Endgame score
    
    // Evaluate with both sets of tables
    // ... (use king_middlegame_pst for mg, king_endgame_pst for eg)
    
    int phase = calculate_game_phase(pos);
    
    // Interpolate between middlegame and endgame
    int score = (mg_score * phase + eg_score * (256 - phase)) / 256;
    
    // WHY interpolate:
    // phase=256 (opening): 100% middlegame
    // phase=128 (midgame): 50% middlegame, 50% endgame
    // phase=0 (endgame): 100% endgame
    
    return score;
}
```

---

## Chapter 6: Search Algorithms

### 6.1 Why We Need Search

**Evaluation alone isn't enough:**

```
Position A: Material equal, score = 0
But white can win queen next move!
Without search, engine thinks position is equal

Position B: White is up a pawn, score = +100  
But black has checkmate in 2
Without search, engine plays on instead of resigning
```

**THE SOLUTION: Look ahead**
- Try all possible moves
- See what happens
- Choose the move that leads to the best outcome

### 6.2 Minimax - The Foundation

**The minimax principle:**
1. I want to maximize my score (play the best move for me)
2. My opponent wants to minimize my score (play the best move for them)
3. We alternate

**Minimax algorithm:**

```cpp
int minimax(Position& pos, int depth) {
    // BASE CASE: Reached depth limit
    if (depth == 0) {
        return evaluate(pos);
    }
    
    // Generate all legal moves
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    // TERMINAL POSITION: No legal moves
    if (moves.count == 0) {
        if (is_in_check(pos)) {
            // Checkmate - worst possible score
            return -30000;  // Negative because we're mated
        } else {
            // Stalemate - draw
            return 0;
        }
    }
    
    // RECURSIVE CASE: Try all moves
    int best_score = -999999;  // Start with worst possible
    
    for (int i = 0; i < moves.count; i++) {
        // Try this move
        makemove(pos, moves[i]);
        
        // See what happens (opponent's turn)
        int score = -minimax(pos, depth - 1);
        
        // WHY negative: From opponent's perspective
        // If opponent scores +500, we score -500
        
        // Undo the move
        undomove(pos, moves[i]);
        
        // Keep track of best
        if (score > best_score) {
            best_score = score;
        }
    }
    
    return best_score;
}

// WHY this is called Negamax:
// We negate scores instead of alternating min/max
// Simpler code, same result
```

**Example tree:**

```
             Position (my turn)
            /         |         \
        Move A     Move B     Move C
         |           |           |
      -minimax    -minimax    -minimax  <- Opponent's best response
       /  \         / \         / \
     ...  ...     ... ...     ... ...
```

**THE PROBLEM with minimax:**
- Depth 5: ~10^7 positions (10 million)
- Depth 10: ~10^15 positions (1 quadrillion!)
- Too slow!

### 6.3 Alpha-Beta Pruning - The Game Changer

**THE KEY INSIGHT:**

If you already found a good move, you can skip checking moves that can't be better.

**Example:**
```
Your turn. You've found a move that scores +5.

Now checking another move:
  Your move -> Opponent's turn
    Opponent finds a response scoring -6 (they're up 6 pawns)
    
STOP! Don't check opponent's other responses!

WHY: Opponent will play the -6 move (best for them)
     This means the overall move scores -6 for you
     But you already have a +5 move
     So this move is worse - skip it!
```

**Alpha-Beta Algorithm:**

```cpp
int alphabeta(Position& pos, int depth, int alpha, int beta) {
    // BASE CASE
    if (depth == 0) {
        return evaluate(pos);
    }
    
    // TERMINAL POSITION
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    if (moves.count == 0) {
        if (is_in_check(pos)) {
            return -30000 + ply;  // Prefer shorter mates
        }
        return 0;  // Stalemate
    }
    
    // WHY +ply in mate score: Prefer mate in 2 over mate in 10
    
    // SEARCH ALL MOVES
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves[i]);
        int score = -alphabeta(pos, depth - 1, -beta, -alpha);
        undomove(pos, moves[i]);
        
        // BETA CUTOFF
        if (score >= beta) {
            return beta;
            
            // WHY: Opponent won't allow this position
            // They have a better option elsewhere
            // This is called a "beta cutoff" or "fail-high"
        }
        
        // UPDATE ALPHA
        if (score > alpha) {
            alpha = score;
            
            // WHY: Found a new best move
            // Any future move must beat this
        }
    }
    
    return alpha;
}

// USAGE: alphabeta(pos, depth, -INFINITY, +INFINITY)
```

**How alpha and beta work:**

```
alpha = Best score we've found so far (lower bound)
beta  = Worst score opponent will allow (upper bound)

Example search:
  Initial: alpha = -∞, beta = +∞
  
  Move 1: score = +200
    Update alpha = +200 (we found a good move)
  
  Move 2: Recursing...
    Opponent finds a response scoring +300 for them
    This scores -300 for us
    -300 < alpha (worse than Move 1)
    But -300 > -∞, so keep checking
    
    Opponent finds another response scoring +400 for them
    This scores -400 for us
    -400 < -300, still worse
    No beta cutoff yet
    
  Move 3: Recursing...
    Opponent finds a response scoring +100 for them  
    This scores -100 for us
    -100 > alpha (+200)? No
    But wait - we're negating!
    
Actually in negamax form:
  Move 1: score = +200, alpha = +200
  
  Move 2: Search with (-beta, -alpha) = (-∞, -200)
    Opponent finds +300 (for them)
    We negate: -300
    -300 >= -200 (beta)? No
    Continue
    
  Move 3: Search with (-beta, -alpha)
    Opponent finds response scoring -250 (for them, +250 for us)
    We negate: +250
    +250 >= +200 (beta in their frame)
    BETA CUTOFF!
```

**Why alpha-beta is amazing:**

```
Without pruning: Search 10^9 positions
With alpha-beta:  Search 10^6 positions (1000x faster!)

In ideal case (best move searched first):
- Minimax: O(b^d) where b = branching factor, d = depth
- Alpha-beta: O(b^(d/2))

Depth 10 minimax = Depth 20 alpha-beta (same time!)
```

### 6.4 Quiescence Search - Avoiding the Horizon Effect

**THE PROBLEM:**

```
Position at depth 0:
  White just moved queen to a5
  Evaluation: +900 (up a queen!)
  
Position at depth 1:
  Black captures queen with pawn
  Evaluation: -900 (down a queen!)
  
The depth-0 evaluation was WRONG!
The queen was hanging, not won
```

This is called the **horizon effect** - stopping search at an unstable position gives wrong results.

**SOLUTION: Quiescence Search**

Only stop searching when position is "quiet" (no captures available).

```cpp
int quiescence(Position& pos, int alpha, int beta) {
    // STANDING PAT
    int stand_pat = evaluate(pos);
    
    // Can we already beat beta without making a move?
    if (stand_pat >= beta) {
        return beta;
    }
    
    // Update alpha if standing pat is better
    if (stand_pat > alpha) {
        alpha = stand_pat;
    }
    
    // WHY standing pat:
    // In quiet position, we can choose not to capture
    // If current position is already good enough, return it
    
    // SEARCH ONLY CAPTURES
    MoveList captures;
    generate_captures(pos, captures);  // Only generate capturing moves
    
    for (int i = 0; i < captures.count; i++) {
        // DELTA PRUNING
        // Skip captures that can't possibly improve alpha
        int captured_value = piece_value[get_piece_at(pos, captures[i].to)];
        if (stand_pat + captured_value + 200 < alpha) {
            continue;
            
            // WHY +200: Safety margin for positional compensation
            // If we're down 5 pawns, capturing a knight won't help
        }
        
        makemove(pos, captures[i]);
        
        // Check legality (did we leave king in check?)
        int king_sq = find_our_king(pos);
        if (is_square_attacked(pos, king_sq, opponent)) {
            undomove(pos, captures[i]);
            continue;
        }
        
        int score = -quiescence(pos, -beta, -alpha);
        undomove(pos, captures[i]);
        
        if (score >= beta) {
            return beta;
        }
        
        if (score > alpha) {
            alpha = score;
        }
    }
    
    return alpha;
}

// MODIFY MAIN SEARCH:
int alphabeta(Position& pos, int depth, int alpha, int beta) {
    if (depth == 0) {
        return quiescence(pos, alpha, beta);  // Changed!
    }
    
    // ... rest of search ...
}
```

**Why quiescence works:**

```
Before quiescence:
  Depth 0: Queen hanging - WRONG evaluation

With quiescence:
  Depth 0: Queen hanging
    Quiescence: Check if queen can be captured
      Yes! After capture, position is bad
      Return bad score - CORRECT evaluation
```

### 6.5 Iterative Deepening - Progressive Search

**THE PROBLEM:**

```
Search depth 10 takes 60 seconds
But we only have 5 seconds to move!

If we search depth 5, takes 0.1 seconds
We have 4.9 seconds wasted!
```

**SOLUTION: Iterative Deepening**

Search depth 1, then 2, then 3, ... until time runs out.

```cpp
Move iterative_deepening(Position& pos, int max_time_ms) {
    auto start_time = get_time();
    Move best_move;
    
    for (int depth = 1; depth <= MAX_DEPTH; depth++) {
        int score = alphabeta(pos, depth, -INFINITY, INFINITY, &best_move);
        
        auto elapsed = get_time() - start_time;
        
        // Send info to GUI
        cout << "info depth " << depth 
             << " score cp " << score
             << " time " << elapsed
             << " nodes " << nodes_searched
             << " pv " << move_to_uci(best_move) << endl;
        
        // Check if we're running out of time
        if (elapsed > max_time_ms * 0.7) {
            break;  // Use last completed depth
        }
    }
    
    return best_move;
}

// WHY 0.7: Don't start new depth if 70% of time used
// Next depth might not complete
```

**Why iterative deepening is not wasteful:**

```
Depth 1: 20 nodes
Depth 2: 400 nodes  
Depth 3: 8,000 nodes
Depth 4: 160,000 nodes
Depth 5: 3,200,000 nodes

Total for depth 1-4: ~168,420 nodes
Depth 5 alone: 3,200,000 nodes

Overhead of re-searching: ~5%
```

**BONUS: Move Ordering**

Results from shallower search help order moves at deeper search:

```cpp
// In alphabeta at depth d, check best move from depth d-1 first
// This causes more beta cutoffs = faster search!
```

---

## Chapter 7: Advanced Search Techniques

### 7.1 Transposition Tables - Cache Search Results

**THE PROBLEM:**

```
Same position can be reached by different move orders:

Path A: e2-e4, e7-e5, Ng1-f3, Nb8-c6
Path B: Ng1-f3, Nb8-c6, e2-e4, e7-e5

Same position! But we search it twice = wasted work
```

**SOLUTION: Transposition Table (TT)**

A hash table that stores search results:

```cpp
struct TTEntry {
    uint64_t key;          // Zobrist hash
    int depth;             // Search depth
    int score;             // Score
    int flag;              // EXACT, LOWER_BOUND, or UPPER_BOUND
    Move best_move;        // Best move found
    uint8_t age;           // For replacement scheme
};

const int TT_SIZE = 16777216;  // 16M entries = ~256 MB
TTEntry transposition_table[TT_SIZE];

// FLAGS explained:
// EXACT: This is the exact score
// LOWER_BOUND: Real score >= this (we had beta cutoff)
// UPPER_BOUND: Real score <= this (no move beat alpha)
```

**Using the transposition table:**

```cpp
int alphabeta(Position& pos, int depth, int alpha, int beta, Move* pv_move) {
    // PROBE TRANSPOSITION TABLE
    uint64_t index = pos.zobrist_key % TT_SIZE;
    TTEntry& entry = transposition_table[index];
    
    if (entry.key == pos.zobrist_key && entry.depth >= depth) {
        // Found a match!
        
        if (entry.flag == EXACT) {
            *pv_move = entry.best_move;
            return entry.score;
            
            // WHY: Exact score from previous search
            // No need to search again!
        }
        
        if (entry.flag == LOWER_BOUND && entry.score >= beta) {
            return beta;
            
            // WHY: Previous search had beta cutoff
            // This position is too good (opponent won't allow it)
        }
        
        if (entry.flag == UPPER_BOUND && entry.score <= alpha) {
            return alpha;
            
            // WHY: Previous search found nothing better than alpha
            // Won't help us now
        }
    }
    
    // ... DO SEARCH ...
    
    int score = alpha;
    Move best_move;
    
    for (Move m : moves) {
        makemove(pos, m);
        int s = -alphabeta(pos, depth - 1, -beta, -alpha, &pv);
        undomove(pos, m);
        
        if (s > score) {
            score = s;
            best_move = m;
        }
        
        if (score >= beta) {
            // STORE IN TT - BETA CUTOFF
            entry.key = pos.zobrist_key;
            entry.depth = depth;
            entry.score = beta;
            entry.flag = LOWER_BOUND;
            entry.best_move = best_move;
            entry.age = current_age;
            
            return beta;
        }
        
        if (score > alpha) {
            alpha = score;
        }
    }
    
    // STORE IN TT - EXACT or UPPER_BOUND
    entry.key = pos.zobrist_key;
    entry.depth = depth;
    entry.score = score;
    entry.flag = (score > original_alpha) ? EXACT : UPPER_BOUND;
    entry.best_move = best_move;
    entry.age = current_age;
    
    return score;
}
```

**Why TT is powerful:**

```
Without TT: Search 10M positions
With TT: Search 3M positions (7M hits!)

Speedup: 3-10x depending on position
```

### 7.2 Move Ordering - Search Best Moves First

**THE KEY INSIGHT:**

Alpha-beta works best when good moves are searched first!

```
Bad ordering:
  Move 1 (bad): score = -50
  Move 2 (bad): score = -20  
  Move 3 (good): score = +100  <- Found best, but searched 3 moves

Good ordering:
  Move 1 (good): score = +100  <- Found best immediately!
  Move 2 (bad): score = -50    <- Beta cutoff!
  Move 3 (bad): Not searched!  <- Saved time!
```

**Move ordering techniques:**

```cpp
void order_moves(MoveList& moves, Move tt_move, Move killers[2]) {
    for (int i = 0; i < moves.count; i++) {
        Move& m = moves[i];
        
        // 1. HASH MOVE (from transposition table)
        if (m == tt_move) {
            m.score = 10000000;
            continue;
        }
        
        // WHY first: TT move is likely best (from previous search)
        
        // 2. WINNING CAPTURES (MVV-LVA)
        if (is_capture(m)) {
            int victim = get_piece_at(pos, m.to);
            int attacker = get_piece_at(pos, m.from);
            
            // Most Valuable Victim - Least Valuable Attacker
            m.score = piece_value[victim] * 10 - piece_value[attacker];
            
            // WHY: Prefer QxP over PxP (win material)
            // Prefer PxQ over QxP (use cheap piece)
            
            // SEE (Static Exchange Evaluation)
            if (SEE(pos, m) >= 0) {
                m.score += 1000000;  // Winning/equal capture
            } else {
                m.score += 100000;   // Losing capture (still try it)
            }
            
            continue;
        }
        
        // 3. KILLER MOVES
        if (m == killers[0]) {
            m.score = 90000;
            continue;
        }
        if (m == killers[1]) {
            m.score = 80000;
            continue;
        }
        
        // WHY killers: Quiet moves that caused beta cutoffs
        // at same depth in sibling nodes
        // Often good in similar positions
        
        // 4. HISTORY HEURISTIC
        m.score = history[m.from][m.to];
        
        // WHY: Moves that caused cutoffs in the past
        // are likely good now
    }
    
    // Sort moves by score
    std::sort(moves.begin(), moves.end(), 
              [](Move a, Move b) { return a.score > b.score; });
}
```

**Killer moves:**

```cpp
Move killer_moves[MAX_PLY][2];  // 2 killers per ply

// When we get beta cutoff from quiet move:
if (!is_capture(move) && score >= beta) {
    killer_moves[ply][1] = killer_moves[ply][0];
    killer_moves[ply][0] = move;
}
```

**History heuristic:**

```cpp
int history[64][64];  // from_square -> to_square

// When quiet move causes cutoff:
if (!is_capture(move) && score >= beta) {
    history[move.from][move.to] += depth * depth;
    
    // WHY depth * depth: Deeper cutoffs more important
}
```

### 7.3 Null Move Pruning

**THE INSIGHT:**

If we can pass (do nothing) and still beat beta, position is really good.

```cpp
int alphabeta(Position& pos, int depth, int alpha, int beta) {
    // ... TT probe ...
    
    // NULL MOVE PRUNING
    if (depth >= 3 && 
        !is_in_check(pos) && 
        !is_endgame(pos) &&
        !last_move_was_null) {
        
        // "Pass" - switch sides without moving
        pos.whiteToMove = !pos.whiteToMove;
        pos.zobrist_key ^= zobrist_side;
        
        // Search with reduced depth
        int R = 2;  // Reduction factor
        int score = -alphabeta(pos, depth - R - 1, -beta, -beta + 1);
        
        // Undo null move
        pos.whiteToMove = !pos.whiteToMove;
        pos.zobrist_key ^= zobrist_side;
        
        if (score >= beta) {
            return beta;  // Cutoff!
            
            // WHY: If doing nothing beats beta,
            // we're in zugzwang or position is too good
        }
    }
    
    // WHY not in check: Passing in check is illegal
    // WHY not in endgame: Zugzwang more common
    
    // ... normal search ...
}
```

### 7.4 Late Move Reductions (LMR)

**THE INSIGHT:**

Later moves (after good move ordering) are probably bad. Search them with reduced depth.

```cpp
for (int i = 0; i < moves.count; i++) {
    makemove(pos, moves[i]);
    
    int score;
    
    if (i >= 4 &&  // Not first few moves
        depth >= 3 &&  // Deep enough
        !is_capture(moves[i]) &&  // Quiet move
        !is_check(pos) &&  // Not giving check
        !is_in_check_before_move) {  // Not escaping check
        
        // Search with reduced depth first
        int R = 1 + (i >= 6 ? 1 : 0) + (depth >= 6 ? 1 : 0);
        score = -alphabeta(pos, depth - R - 1, -alpha - 1, -alpha);
        
        // If it's better than expected, re-search full depth
        if (score > alpha) {
            score = -alphabeta(pos, depth - 1, -beta, -alpha);
        }
    } else {
        // Search normally (hash move, captures, checks)
        score = -alphabeta(pos, depth - 1, -beta, -alpha);
    }
    
    undomove(pos, moves[i]);
    
    // WHY: Most moves are bad
    // Searching reduced depth proves they're bad faster
    // If we're wrong, re-search with full depth
}
```

---

## Chapter 8: UCI Protocol

### 8.1 UCI Overview

UCI (Universal Chess Interface) is the standard protocol for chess engines.

**How it works:**
- GUI sends text commands via stdin
- Engine responds via stdout
- Simple, text-based, easy to debug

### 8.2 Essential Commands

```cpp
void uci_loop() {
    string line;
    Position pos;
    
    while (getline(cin, line)) {
        istringstream iss(line);
        string command;
        iss >> command;
        
        if (command == "uci") {
            // Identify engine
            cout << "id name OctoKnight" << endl;
            cout << "id author YourName" << endl;
            
            // Options
            cout << "option name Hash type spin default 64 min 1 max 4096" << endl;
            cout << "option name Threads type spin default 1 min 1 max 128" << endl;
            
            cout << "uciok" << endl;
        }
        
        else if (command == "isready") {
            // GUI checking if engine is ready
            cout << "readyok" << endl;
        }
        
        else if (command == "ucinewgame") {
            // New game starting
            pos.setStartingPosition();
            clear_transposition_table();
        }
        
        else if (command == "position") {
            string type;
            iss >> type;
            
            if (type == "startpos") {
                pos.setStartingPosition();
                string moves_token;
                iss >> moves_token;  // Skip "moves"
            }
            else if (type == "fen") {
                string fen;
                getline(iss, fen, delimiter);
                pos.setFromFEN(fen);
            }
            
            // Apply moves
            string move_str;
            while (iss >> move_str) {
                Move m = uci_to_move(move_str, pos);
                makemove(pos, m);
            }
        }
        
        else if (command == "go") {
            // Start thinking
            int depth = MAX_DEPTH;
            int movetime = -1;
            int wtime = -1, btime = -1;
            
            string param;
            while (iss >> param) {
                if (param == "depth") iss >> depth;
                else if (param == "movetime") iss >> movetime;
                else if (param == "wtime") iss >> wtime;
                else if (param == "btime") iss >> btime;
            }
            
            // Calculate time to use
            if (movetime == -1) {
                int mytime = pos.whiteToMove ? wtime : btime;
                movetime = mytime / 30;  // Simple time management
            }
            
            Move best = search(pos, depth, movetime);
            cout << "bestmove " << move_to_uci(best) << endl;
        }
        
        else if (command == "quit") {
            break;
        }
    }
}
```

**Info strings during search:**

```cpp
cout << "info"
     << " depth " << depth
     << " seldepth " << seldepth  // Selective depth (quiescence)
     << " score cp " << score
     << " time " << time_ms
     << " nodes " << nodes
     << " nps " << (nodes * 1000 / time_ms)
     << " pv " << pv_string  // Principal variation
     << endl;
```

---

## Chapter 10: Stockfish Techniques and NNUE

### 10.1 What Makes Stockfish Special?

Stockfish is the strongest open-source chess engine. Key innovations:

1. **NNUE** (Neural Network Efficiently Updatable Evaluation)
2. **Lazy SMP** (parallel search)
3. **Advanced pruning**
4. **Syzygy tablebases**

### 10.2 NNUE - Neural Network Evaluation

**Traditional evaluation: Hand-crafted features**
```cpp
score = material + pst + pawn_structure + king_safety + ...
// ~20-30 features, carefully tuned
```

**NNUE: Neural network learns from millions of games**

**Architecture:**
```
Input: 768 neurons (12 pieces × 64 squares)
  ↓
Hidden layer 1: 256 neurons (ReLU activation)
  ↓  
Hidden layer 2: 32 neurons (ReLU activation)
  ↓
Output: 1 neuron (position score)
```

**Key innovation: Incremental updates**

```cpp
// Traditional NN: Recompute all layers each move
// NNUE: Only update changed inputs!

// When piece moves from a5 to a6:
hidden[256] -= weights[piece][a5][:]  // Remove old
hidden[256] += weights[piece][a6][:]  // Add new

// Only 2 operations instead of 768!
```

**Implementation skeleton:**

```cpp
struct NNUE {
    // Network weights (loaded from file)
    int16_t feature_weights[768][256];
    int16_t hidden1_weights[256][32];
    int16_t hidden2_weights[32][1];
    
    // Accumulator (updated incrementally)
    int16_t accumulator[256];
    
    void reset(const Position& pos) {
        memset(accumulator, 0, sizeof(accumulator));
        
        // Initialize from position
        for each piece:
            int feature = piece_type * 64 + square;
            for (int i = 0; i < 256; i++):
                accumulator[i] += feature_weights[feature][i];
    }
    
    void update(Move m) {
        // Remove piece from old square
        int old_feature = piece * 64 + m.from;
        for (int i = 0; i < 256; i++):
            accumulator[i] -= feature_weights[old_feature][i];
        
        // Add piece to new square
        int new_feature = piece * 64 + m.to;
        for (int i = 0; i < 256; i++):
            accumulator[i] += feature_weights[new_feature][i];
    }
    
    int evaluate() {
        // Forward pass through network
        int32_t hidden2[32];
        for (int i = 0; i < 32; i++) {
            int32_t sum = 0;
            for (int j = 0; j < 256; j++) {
                // ReLU activation
                int16_t val = max(0, accumulator[j]);
                sum += val * hidden1_weights[j][i];
            }
            hidden2[i] = sum;
        }
        
        // Output layer
        int32_t output = 0;
        for (int i = 0; i < 32; i++) {
            output += max(0, hidden2[i]) * hidden2_weights[i][0];
        }
        
        return output / 128;  // Scale to centipawns
    }
};
```

### 10.3 Lazy SMP - Parallel Search

**Traditional parallel search: Complex**
- Master thread distributes work
- Synchronization overhead
- Load balancing issues

**Lazy SMP: Beautifully simple**
- Each thread searches independently
- Threads share transposition table
- No synchronization needed!

```cpp
void parallel_search(Position& pos, int depth) {
    vector<thread> threads;
    atomic<bool> stop_flag(false);
    Move best_moves[num_threads];
    
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&, i]() {
            Position local_pos = pos;  // Each thread has own position
            
            // Vary depth slightly to avoid same search
            int thread_depth = depth + (i % 2);
            
            best_moves[i] = search(local_pos, thread_depth, &stop_flag);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Use result from thread 0 (or thread with highest depth)
    return best_moves[0];
}

// WHY this works:
// - Threads help each other via shared TT
// - TT synchronization is lock-free (atomic operations)
// - Some redundant work, but parallelism benefit > cost
```

### 10.4 Advanced Pruning Techniques

**Futility Pruning:**
```cpp
// At low depths, if position is way behind, skip quiet moves
if (depth <= 3 && !is_in_check(pos)) {
    int margin = 150 * depth;  // 150, 300, 450
    if (eval + margin < alpha) {
        // Even best case won't improve alpha
        return quiescence(pos, alpha, beta);
    }
}
```

**Razoring:**
```cpp
// If evaluation is way below alpha, go straight to quiescence
if (depth <= 3 && eval + 300 < alpha) {
    int qscore = quiescence(pos, alpha, beta);
    if (qscore < alpha) return qscore;
}
```

**Multi-Cut:**
```cpp
// If multiple moves cause beta cutoff at reduced depth, assume one will at full depth
int cutoff_count = 0;
for (Move m : moves) {
    if (reduced_search(m) >= beta) {
        cutoff_count++;
        if (cutoff_count >= 3) return beta;
    }
}
```

---

## Conclusion

You now have everything needed to write a competitive chess engine!

**What you've learned:**
1. Bitboard representation and move generation
2. Make/unmake moves perfectly
3. Position evaluation (material, PSTs, pawn structure, king safety)
4. Search algorithms (minimax, alpha-beta, quiescence)
5. Advanced techniques (TT, move ordering, null move, LMR)
6. UCI protocol
7. Modern techniques (NNUE, Lazy SMP)

**Next steps:**
1. Implement a basic engine (bitboards + minimax)
2. Add alpha-beta and quiescence
3. Add transposition table
4. Implement move ordering
5. Add advanced pruning
6. Consider NNUE for evaluation
7. Add multi-threading

**Resources:**
- Chess Programming Wiki: https://www.chessprogramming.org/
- Stockfish source: https://github.com/official-stockfish/Stockfish
- Engine testing: http://www.computerchess.org.uk/ccrl/
- Community: TalkChess forums, Discord servers

**Final advice:**
- Start simple, add complexity gradually
- Test thoroughly (perft, tactical tests)
- Profile before optimizing
- Study strong engines' code
- Join the chess programming community!

Good luck with your chess engine! 🚀♟️
