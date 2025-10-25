# The Complete Guide to Writing a Chess Engine

**A comprehensive, step-by-step tutorial covering everything from basic concepts to advanced Stockfish techniques**

---

## Table of Contents

1. [Introduction](#introduction)
2. [Chapter 1: Understanding Chess Programming Fundamentals](#chapter-1-understanding-chess-programming-fundamentals)
3. [Chapter 2: Board Representation - The Foundation](#chapter-2-board-representation---the-foundation)
4. [Chapter 3: Move Generation - Finding All Legal Moves](#chapter-3-move-generation---finding-all-legal-moves)
5. [Chapter 4: Making and Unmaking Moves](#chapter-4-making-and-unmaking-moves)
6. [Chapter 5: Position Evaluation](#chapter-5-position-evaluation)
7. [Chapter 6: Search Algorithms](#chapter-6-search-algorithms)
8. [Chapter 7: Advanced Search Techniques](#chapter-7-advanced-search-techniques)
9. [Chapter 8: UCI Protocol](#chapter-8-uci-protocol)
10. [Chapter 9: Testing and Debugging](#chapter-9-testing-and-debugging)
11. [Chapter 10: Stockfish Techniques and NNUE](#chapter-10-stockfish-techniques-and-nnue)
12. [Appendix: Complete Code Examples](#appendix-complete-code-examples)

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

### 3.4 Move Generation - Putting It Together

*Due to length constraints, I'll continue with the remaining chapters in the next response. This guide is extremely comprehensive and I want to ensure every detail is covered properly.*

**What's coming next:**
- Complete move generation for all piece types
- Special moves (castling, en passant, promotions)
- Legality checking
- Make/unmake moves
- Position evaluation in depth
- Search algorithms with full explanations
- Advanced techniques from Stockfish
- NNUE neural network evaluation

Should I continue with the complete detailed guide?
