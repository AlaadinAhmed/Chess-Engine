/**
\mainpage How to Write a Chess Engine: A Comprehensive Guide

\tableofcontents

---

\section intro_sec Introduction

This comprehensive guide will teach you how to write a chess engine from scratch. A chess engine is a program that can play chess by evaluating positions and searching for the best moves. This document covers everything from basic board representation to advanced search techniques.

\subsection what_is_engine What is a Chess Engine?

A chess engine consists of several core components:
- **Board Representation**: How to store the position (pieces, castling rights, etc.)
- **Move Generation**: Finding all legal moves in a position
- **Position Evaluation**: Determining who is winning and by how much
- **Search Algorithm**: Looking ahead to find the best move
- **UCI Protocol**: Communicating with chess GUIs

This guide uses the OctoKnight engine as a reference implementation, but the concepts apply to any chess engine.

---

\section chapter1 Chapter 1: Board Representation

\subsection why_bitboards 1.1 Why Bitboards?

The first critical decision when writing a chess engine is choosing how to represent the board. The most efficient modern approach is **bitboards**.

A bitboard is a 64-bit integer (`uint64_t`) where each bit represents a square on the chessboard:

```
Bit Index:    Square:
56 57 58 ... 63    a8 b8 c8 ... h8
48 49 50 ... 55    a7 b7 c7 ... h7
...                ...
0  1  2  ... 7     a1 b1 c1 ... h1
```

**Advantages of bitboards:**
- Fast operations using CPU bitwise instructions
- Compact memory footprint
- Natural for calculating moves and attacks
- Easy to copy positions (just copy the bitboards)

\subsection bitboard_impl 1.2 Implementing Bitboards

Store one bitboard for each piece type and color:

```cpp
struct Position {
    uint64_t WhitePawns;
    uint64_t WhiteKnights;
    uint64_t WhiteBishops;
    uint64_t WhiteRooks;
    uint64_t WhiteQueen;
    uint64_t WhiteKing;
    
    uint64_t BlackPawns;
    uint64_t BlackKnights;
    uint64_t BlackBishops;
    uint64_t BlackRooks;
    uint64_t BlackQueen;
    uint64_t BlackKing;
    
    // Combined bitboards for fast access
    uint64_t WhiteoccupiedSquares;
    uint64_t BlackoccupiedSquares;
    uint64_t occupiedSquares;
    uint64_t emptySquares;
    
    // Game state
    bool whiteToMove;
    uint8_t castelingRights;  // KQkq encoded as bits
    uint64_t enPassant;        // Bitboard with target square
    int move50rule;            // Halfmove clock for 50-move rule
    uint64_t zobrist_key;      // Hash of the position
};
```

**Basic bitboard operations:**

```cpp
// Set a bit (add piece to square)
bitboard |= (1ULL << square_index);

// Clear a bit (remove piece from square)
bitboard &= ~(1ULL << square_index);

// Test if bit is set (is piece on square?)
bool is_set = (bitboard & (1ULL << square_index)) != 0;

// Count bits (how many pieces?)
int count = __builtin_popcountll(bitboard);

// Get least significant bit index (find first piece)
int square = __builtin_ctzll(bitboard);

// Clear least significant bit (iterate through pieces)
bitboard &= bitboard - 1;
```

\subsection fen_parsing 1.3 FEN Parsing

FEN (Forsyth-Edwards Notation) is the standard way to represent chess positions as strings. Example:
```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

This represents:
- **Piece placement**: rnbqkbnr/pppppppp/... (rank 8 to rank 1)
- **Side to move**: w (white)
- **Castling**: KQkq (white kingside, white queenside, black kingside, black queenside)
- **En passant**: - (no en passant square)
- **Halfmove clock**: 0
- **Fullmove number**: 1

**FEN Parsing Algorithm:**

```cpp
void parseFEN(Position& pos, const string& fen) {
    // 1. Clear all bitboards
    pos.WhitePawns = 0;
    // ... clear all others ...
    
    // 2. Parse piece placement
    int rank = 7, file = 0;  // Start at a8
    for (char c : fen_piece_section) {
        if (isalpha(c)) {
            // It's a piece - set the appropriate bitboard
            int square = rank * 8 + file;
            if (c == 'P') pos.WhitePawns |= (1ULL << square);
            else if (c == 'p') pos.BlackPawns |= (1ULL << square);
            // ... handle other pieces ...
            file++;
        }
        else if (isdigit(c)) {
            // Empty squares
            file += (c - '0');
        }
        else if (c == '/') {
            // Next rank
            rank--;
            file = 0;
        }
    }
    
    // 3. Parse side to move
    pos.whiteToMove = (side_char == 'w');
    
    // 4. Parse castling rights
    pos.castelingRights = 0;
    if (castling_string.find('K') != string::npos) pos.castelingRights |= 1;  // White kingside
    if (castling_string.find('Q') != string::npos) pos.castelingRights |= 2;  // White queenside
    if (castling_string.find('k') != string::npos) pos.castelingRights |= 4;  // Black kingside
    if (castling_string.find('q') != string::npos) pos.castelingRights |= 8;  // Black queenside
    
    // 5. Parse en passant square
    if (ep_string != "-") {
        int file = ep_string[0] - 'a';
        int rank = ep_string[1] - '1';
        pos.enPassant = (1ULL << (rank * 8 + file));
    }
    
    // 6. Update combined bitboards
    pos.WhiteoccupiedSquares = pos.WhitePawns | pos.WhiteKnights | ...;
    pos.BlackoccupiedSquares = pos.BlackPawns | pos.BlackKnights | ...;
    pos.occupiedSquares = pos.WhiteoccupiedSquares | pos.BlackoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;
    
    // 7. Compute Zobrist hash (covered later)
    pos.zobrist_key = compute_zobrist(pos);
}
```

---

\section chapter2 Chapter 2: Move Generation

\subsection movegen_overview 2.1 Overview

Move generation is finding all legal moves in a position. The standard approach is:
1. Generate pseudo-legal moves (moves that look legal but might leave the king in check)
2. Filter out illegal moves (those that leave your king in check)

\subsection move_structure 2.2 Move Structure

```cpp
struct Move {
    int from;              // Source square (0-63)
    int to;                // Destination square (0-63)
    Pieces promotion = NO_PIECE;  // Promotion piece (Q/R/B/N) or NO_PIECE
    int score = 0;         // For move ordering
};
```

\subsection attack_tables 2.3 Attack Tables

For non-sliding pieces (king, knight, pawn), pre-compute attack tables:

```cpp
uint64_t kingAttacks[64];     // King attacks from each square
uint64_t knightAttacks[64];   // Knight attacks from each square

void initKingAttacks() {
    for (int square = 0; square < 64; square++) {
        uint64_t attacks = 0;
        int rank = square / 8;
        int file = square % 8;
        
        // Check all 8 directions
        if (rank < 7) attacks |= (1ULL << (square + 8));           // North
        if (rank > 0) attacks |= (1ULL << (square - 8));           // South
        if (file < 7) attacks |= (1ULL << (square + 1));           // East
        if (file > 0) attacks |= (1ULL << (square - 1));           // West
        if (rank < 7 && file < 7) attacks |= (1ULL << (square + 9));  // NE
        if (rank < 7 && file > 0) attacks |= (1ULL << (square + 7));  // NW
        if (rank > 0 && file < 7) attacks |= (1ULL << (square - 7));  // SE
        if (rank > 0 && file > 0) attacks |= (1ULL << (square - 9));  // SW
        
        kingAttacks[square] = attacks;
    }
}
```

For knights, use similar logic with the L-shape pattern.

\subsection magic_bitboards 2.4 Magic Bitboards for Sliding Pieces

Bishops, rooks, and queens are **sliding pieces** - their attacks depend on blockers. The efficient way to handle this is **magic bitboards**.

The idea:
1. For each square, pre-compute all possible blocker configurations
2. Use a "magic number" to hash the blockers into an attack table
3. Lookup is instant: `attacks = table[square][magic_hash(blockers)]`

**Simplified explanation:**
```cpp
uint64_t get_rook_attacks(int square, uint64_t occupied) {
    // Get relevant occupancy (only squares that matter for this rook)
    uint64_t relevant = rookMasks[square] & occupied;
    
    // Hash using magic number
    uint64_t hash = (relevant * rookMagics[square]) >> (64 - rookBits[square]);
    
    // Lookup in pre-computed table
    return rookAttacks[square][hash];
}
```

Computing magic numbers is complex - see the Chess Programming Wiki for details. You can also use pre-computed magic numbers.

\subsection pawn_moves 2.5 Pawn Move Generation

Pawns are special - they move differently than they capture:

```cpp
void generate_pawn_moves(Position& pos, MoveList& moves) {
    uint64_t pawns = pos.whiteToMove ? pos.WhitePawns : pos.BlackPawns;
    int direction = pos.whiteToMove ? 8 : -8;  // North for white, south for black
    
    while (pawns) {
        int from = __builtin_ctzll(pawns);
        int to_rank = (from + direction) / 8;
        
        // Single push
        int push_square = from + direction;
        if (!(pos.occupiedSquares & (1ULL << push_square))) {
            if (to_rank == 7 || to_rank == 0) {
                // Promotion - generate 4 moves
                add_promotion_moves(moves, from, push_square);
            } else {
                moves.add({from, push_square});
            }
            
            // Double push from starting rank
            if ((pos.whiteToMove && from / 8 == 1) || (!pos.whiteToMove && from / 8 == 6)) {
                int double_square = from + 2 * direction;
                if (!(pos.occupiedSquares & (1ULL << double_square))) {
                    moves.add({from, double_square});
                }
            }
        }
        
        // Captures
        uint64_t attacks = GetPawnAttacks(pos, from, pos.whiteToMove);
        uint64_t captures = attacks & (pos.whiteToMove ? pos.BlackoccupiedSquares : pos.WhiteoccupiedSquares);
        
        while (captures) {
            int capture_square = __builtin_ctzll(captures);
            if (capture_square / 8 == 7 || capture_square / 8 == 0) {
                add_promotion_moves(moves, from, capture_square);
            } else {
                moves.add({from, capture_square});
            }
            captures &= captures - 1;
        }
        
        // En passant
        if (pos.enPassant && (attacks & pos.enPassant)) {
            int ep_square = __builtin_ctzll(pos.enPassant);
            moves.add({from, ep_square});
        }
        
        pawns &= pawns - 1;
    }
}

void add_promotion_moves(MoveList& moves, int from, int to) {
    // Always generate all 4 promotions
    moves.add({from, to, pos.whiteToMove ? W_QUEEN : B_QUEEN});
    moves.add({from, to, pos.whiteToMove ? W_ROOK : B_ROOK});
    moves.add({from, to, pos.whiteToMove ? W_BISHOP : B_BISHOP});
    moves.add({from, to, pos.whiteToMove ? W_KNIGHT : B_KNIGHT});
}
```

**Key points:**
- Pawns push forward but capture diagonally
- Promotions generate 4 separate moves (Q, R, B, N)
- Don't forget en passant and double pushes!

\subsection piece_moves 2.6 Other Piece Moves

For other pieces, the pattern is similar:

```cpp
void generate_knight_moves(Position& pos, MoveList& moves) {
    uint64_t knights = pos.whiteToMove ? pos.WhiteKnights : pos.BlackKnights;
    uint64_t targets = ~(pos.whiteToMove ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares);
    
    while (knights) {
        int from = __builtin_ctzll(knights);
        uint64_t attacks = knightAttacks[from] & targets;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            moves.add({from, to});
            attacks &= attacks - 1;
        }
        
        knights &= knights - 1;
    }
}
```

\subsection castling_moves 2.7 Castling

Castling is a special move with specific rules:
- King and rook haven't moved (check castling rights)
- Squares between king and rook are empty
- King is not in check
- King doesn't pass through check
- King doesn't end in check

```cpp
void generate_castling_moves(Position& pos, MoveList& moves) {
    if (pos.whiteToMove) {
        // White kingside (e1-g1)
        if ((pos.castelingRights & 1) &&  // Has right
            !(pos.occupiedSquares & 0x60ULL) &&  // f1, g1 empty
            !is_square_attacked(pos, 4, false) &&  // e1 not attacked
            !is_square_attacked(pos, 5, false) &&  // f1 not attacked
            !is_square_attacked(pos, 6, false)) {  // g1 not attacked
            moves.add({4, 6});  // e1 to g1
        }
        
        // White queenside (e1-c1)
        if ((pos.castelingRights & 2) &&  // Has right
            !(pos.occupiedSquares & 0xEULL) &&  // b1, c1, d1 empty
            !is_square_attacked(pos, 4, false) &&  // e1 not attacked
            !is_square_attacked(pos, 3, false) &&  // d1 not attacked
            !is_square_attacked(pos, 2, false)) {  // c1 not attacked
            moves.add({4, 2});  // e1 to c1
        }
    }
    // Similar for black...
}
```

\subsection legality_check 2.8 Legality Checking

After generating pseudo-legal moves, check if they're truly legal:

```cpp
void generate_legal_moves(Position& pos, MoveList& legal_moves) {
    MoveList pseudo_legal;
    generate_pseudo_legal_moves(pos, pseudo_legal);
    
    for (int i = 0; i < pseudo_legal.count; i++) {
        Move m = pseudo_legal.moves[i];
        
        // Make move
        makemove(pos, m);
        
        // Find our king
        int king_square = __builtin_ctzll(pos.whiteToMove ? pos.BlackKing : pos.WhiteKing);
        
        // Check if king is attacked
        bool in_check = is_square_attacked(pos, king_square, pos.whiteToMove);
        
        // Undo move
        undomove(pos, m);
        
        // Add if legal
        if (!in_check) {
            legal_moves.add(m);
        }
    }
}
```

---

\section chapter3 Chapter 3: Making and Unmaking Moves

\subsection makemove_overview 3.1 Overview

To search efficiently, you need to:
1. Make a move (update the position)
2. Search the resulting position
3. Unmake the move (restore the original position)

This must be fast and perfectly reversible.

\subsection undo_info 3.2 Undo Information

Store information needed to undo a move:

```cpp
struct UndoInfo {
    uint64_t oldHashKey;
    uint8_t oldCastelingRights;
    uint64_t oldEnPassant;
    int oldHalfMove;
    Pieces oldCapturedPiece;
    bool isEnPassant;
    int enPassantCapturedPawnSquare;
    bool isCastling;
    int castlingRookFrom;
    int castlingRookTo;
    bool side;
    Pieces movedPiece;
};
```

\subsection makemove_impl 3.3 Making a Move

```cpp
void makemove(Position& pos, Move m) {
    // 1. Save undo information
    UndoInfo undo;
    undo.oldHashKey = pos.zobrist_key;
    undo.oldCastelingRights = pos.castelingRights;
    undo.oldEnPassant = pos.enPassant;
    undo.movedPiece = get_piece_at(pos, m.from);
    undo.oldCapturedPiece = get_piece_at(pos, m.to);
    
    // 2. Update Zobrist hash (remove old state)
    pos.zobrist_key ^= zobrist_side_to_move;
    if (pos.enPassant) pos.zobrist_key ^= zobrist_enpassant[__builtin_ctzll(pos.enPassant)];
    pos.zobrist_key ^= zobrist_castling[pos.castelingRights];
    
    // 3. Clear old en passant
    pos.enPassant = 0;
    
    // 4. Move the piece
    uint64_t from_bb = (1ULL << m.from);
    uint64_t to_bb = (1ULL << m.to);
    
    // Remove from source
    remove_piece(pos, undo.movedPiece, m.from);
    pos.zobrist_key ^= zobrist_pieces[undo.movedPiece][m.from];
    
    // Capture if needed
    if (undo.oldCapturedPiece != NO_PIECE) {
        remove_piece(pos, undo.oldCapturedPiece, m.to);
        pos.zobrist_key ^= zobrist_pieces[undo.oldCapturedPiece][m.to];
        pos.move50rule = 0;  // Reset on capture
    }
    
    // Place on destination
    if (m.promotion != NO_PIECE) {
        add_piece(pos, m.promotion, m.to);
        pos.zobrist_key ^= zobrist_pieces[m.promotion][m.to];
    } else {
        add_piece(pos, undo.movedPiece, m.to);
        pos.zobrist_key ^= zobrist_pieces[undo.movedPiece][m.to];
    }
    
    // 5. Handle special moves (castling, en passant, pawn double push)
    // ... (details omitted for brevity)
    
    // 6. Update game state
    pos.whiteToMove = !pos.whiteToMove;
    pos.zobrist_key ^= zobrist_side_to_move;
    pos.zobrist_key ^= zobrist_castling[pos.castelingRights];
    
    // 7. Update occupancy bitboards
    update_occupancy(pos);
    
    // Store undo info
    history[history_ply++] = undo;
}
```

\subsection unmakemove_impl 3.4 Unmaking a Move

```cpp
void undomove(Position& pos, Move m) {
    // Retrieve undo info
    UndoInfo undo = history[--history_ply];
    
    // 1. Switch side back
    pos.whiteToMove = !pos.whiteToMove;
    
    // 2. Move piece back
    remove_piece(pos, m.promotion != NO_PIECE ? m.promotion : undo.movedPiece, m.to);
    add_piece(pos, undo.movedPiece, m.from);
    
    // 3. Restore captured piece
    if (undo.oldCapturedPiece != NO_PIECE) {
        add_piece(pos, undo.oldCapturedPiece, m.to);
    }
    
    // 4. Restore game state
    pos.zobrist_key = undo.oldHashKey;
    pos.castelingRights = undo.oldCastelingRights;
    pos.enPassant = undo.oldEnPassant;
    pos.move50rule = undo.oldHalfMove;
    
    // 5. Handle special moves
    // ... (undo castling, en passant, etc.)
    
    // 6. Update occupancy
    update_occupancy(pos);
}
```

---

\section chapter4 Chapter 4: Position Evaluation

\subsection eval_overview 4.1 Overview

Evaluation assigns a numeric score to a position. Positive scores favor white, negative favor black. The score is in centipawns (1 pawn = 100).

\subsection material_eval 4.2 Material Counting

The simplest evaluation:

```cpp
int evaluate(const Position& pos) {
    int score = 0;
    
    // Piece values
    score += __builtin_popcountll(pos.WhitePawns) * 100;
    score += __builtin_popcountll(pos.WhiteKnights) * 320;
    score += __builtin_popcountll(pos.WhiteBishops) * 330;
    score += __builtin_popcountll(pos.WhiteRooks) * 500;
    score += __builtin_popcountll(pos.WhiteQueen) * 900;
    
    score -= __builtin_popcountll(pos.BlackPawns) * 100;
    score -= __builtin_popcountll(pos.BlackKnights) * 320;
    score -= __builtin_popcountll(pos.BlackBishops) * 330;
    score -= __builtin_popcountll(pos.BlackRooks) * 500;
    score -= __builtin_popcountll(pos.BlackQueen) * 900;
    
    return pos.whiteToMove ? score : -score;
}
```

\subsection pst_eval 4.3 Piece-Square Tables

Pieces are worth more in certain positions:

```cpp
// Pawn position bonus (encourage center control, advancement)
const int pawn_pst[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     50, 50, 50, 50, 50, 50, 50, 50,
     10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

int evaluate_with_pst(const Position& pos) {
    int score = 0;
    
    // White pawns
    uint64_t pawns = pos.WhitePawns;
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        score += 100 + pawn_pst[sq];
        pawns &= pawns - 1;
    }
    
    // Black pawns (mirror the table)
    pawns = pos.BlackPawns;
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        score -= 100 + pawn_pst[63 - sq];  // Mirror vertically
        pawns &= pawns - 1;
    }
    
    // ... similar for other pieces ...
    
    return pos.whiteToMove ? score : -score;
}
```

\subsection advanced_eval 4.4 Advanced Evaluation Terms

Strong engines consider:
- **Mobility**: Number of legal moves
- **King safety**: Pawns in front of king, open files near king
- **Pawn structure**: Doubled pawns, isolated pawns, passed pawns
- **Piece activity**: Rooks on open files, bishops on long diagonals
- **Control of center**: Pawns and pieces controlling central squares

Example - passed pawn bonus:

```cpp
int evaluate_passed_pawns(const Position& pos) {
    int score = 0;
    uint64_t white_pawns = pos.WhitePawns;
    
    while (white_pawns) {
        int sq = __builtin_ctzll(white_pawns);
        int file = sq % 8;
        int rank = sq / 8;
        
        // Check if no black pawn can stop it
        uint64_t blocking_mask = 0;
        for (int r = rank + 1; r < 8; r++) {
            if (file > 0) blocking_mask |= (1ULL << (r * 8 + file - 1));
            blocking_mask |= (1ULL << (r * 8 + file));
            if (file < 7) blocking_mask |= (1ULL << (r * 8 + file + 1));
        }
        
        if (!(blocking_mask & pos.BlackPawns)) {
            // It's a passed pawn!
            score += 50 + (rank - 1) * 10;  // Bonus increases as it advances
        }
        
        white_pawns &= white_pawns - 1;
    }
    
    // ... similar for black ...
    
    return score;
}
```

---

\section chapter5 Chapter 5: Search Algorithms

\subsection search_overview 5.1 Overview

Search explores future positions to find the best move. The basic idea:
1. Try each legal move
2. For each move, recursively search the opponent's responses
3. Assume the opponent plays the best move (minimax)
4. Return the move that gives the best result

\subsection minimax 5.2 Minimax Algorithm

```cpp
int minimax(Position& pos, int depth) {
    // Base case - evaluate position
    if (depth == 0) {
        return evaluate(pos);
    }
    
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    // Checkmate or stalemate
    if (moves.count == 0) {
        if (is_in_check(pos)) return -30000;  // Checkmate
        return 0;  // Stalemate
    }
    
    int best_score = -999999;
    
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves[i]);
        int score = -minimax(pos, depth - 1);  // Negamax
        undomove(pos, moves[i]);
        
        if (score > best_score) {
            best_score = score;
        }
    }
    
    return best_score;
}
```

**Negamax trick**: Negate the score from the opponent's perspective. This simplifies the code (no separate min/max).

\subsection alphabeta 5.3 Alpha-Beta Pruning

Minimax searches every branch. Alpha-beta prunes branches that can't affect the result:

```cpp
int alphabeta(Position& pos, int depth, int alpha, int beta) {
    if (depth == 0) {
        return evaluate(pos);
    }
    
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    if (moves.count == 0) {
        if (is_in_check(pos)) return -30000 + ply;  // Prefer shorter mates
        return 0;
    }
    
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves[i]);
        int score = -alphabeta(pos, depth - 1, -beta, -alpha);
        undomove(pos, moves[i]);
        
        if (score >= beta) {
            return beta;  // Beta cutoff - opponent won't allow this
        }
        
        if (score > alpha) {
            alpha = score;  // New best move
        }
    }
    
    return alpha;
}
```

**Key insight**: If we find a move that's "too good", the opponent won't let us reach this position (they had a better choice earlier). We can stop searching this branch.

Alpha-beta typically reduces the search tree by 50-90%!

\subsection quiescence 5.4 Quiescence Search

Stopping the search at an arbitrary depth causes the **horizon effect**. Example:
- Depth 5: Position looks quiet (score: +1)
- Depth 6: Queen hangs (score: -9)

Solution: **Quiescence search** - only stop searching when the position is "quiet" (no captures).

```cpp
int quiescence(Position& pos, int alpha, int beta) {
    // Stand pat - can we improve without moving?
    int stand_pat = evaluate(pos);
    
    if (stand_pat >= beta) {
        return beta;
    }
    
    if (stand_pat > alpha) {
        alpha = stand_pat;
    }
    
    // Only search captures
    MoveList captures;
    generate_captures(pos, captures);
    
    for (int i = 0; i < captures.count; i++) {
        makemove(pos, captures[i]);
        
        // Check if move is legal
        int king_sq = find_king(pos);
        if (is_square_attacked(pos, king_sq, !pos.whiteToMove)) {
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
```

Modify main search to call quiescence:

```cpp
int alphabeta(Position& pos, int depth, int alpha, int beta) {
    if (depth == 0) {
        return quiescence(pos, alpha, beta);  // Changed!
    }
    // ... rest of search ...
}
```

\subsection iterative_deepening 5.5 Iterative Deepening

Instead of searching to a fixed depth, search progressively deeper:

```cpp
Move find_best_move(Position& pos, int max_time_ms) {
    Move best_move;
    auto start_time = now();
    
    for (int depth = 1; depth <= MAX_DEPTH; depth++) {
        int score = alphabeta(pos, depth, -INFINITY, INFINITY, best_move);
        
        auto elapsed = now() - start_time;
        
        cout << "info depth " << depth 
             << " score cp " << score
             << " time " << elapsed
             << " pv " << move_to_string(best_move) << endl;
        
        if (elapsed >= max_time_ms * 0.8) {
            break;  // Almost out of time
        }
    }
    
    return best_move;
}
```

**Why iterative deepening?**
- Time management: Stop when time is up
- Move ordering: Use results from shallow search to order moves in deeper search
- Provides updates to GUI

---

\section chapter6 Chapter 6: Optimizations

\subsection transposition_table 6.1 Transposition Tables

Many positions can be reached by different move orders. Cache results:

```cpp
struct TTEntry {
    uint64_t key;        // Zobrist hash
    int depth;           // Search depth
    int score;           // Score
    int flag;            // EXACT, LOWER_BOUND, or UPPER_BOUND
    Move best_move;      // Best move found
};

TTEntry transposition_table[TABLE_SIZE];

int alphabeta(Position& pos, int depth, int alpha, int beta) {
    // Probe transposition table
    uint64_t index = pos.zobrist_key % TABLE_SIZE;
    TTEntry& entry = transposition_table[index];
    
    if (entry.key == pos.zobrist_key && entry.depth >= depth) {
        if (entry.flag == EXACT) {
            return entry.score;
        }
        if (entry.flag == LOWER_BOUND && entry.score >= beta) {
            return beta;
        }
        if (entry.flag == UPPER_BOUND && entry.score <= alpha) {
            return alpha;
        }
    }
    
    // ... do search ...
    
    // Store in transposition table
    entry.key = pos.zobrist_key;
    entry.depth = depth;
    entry.score = best_score;
    entry.best_move = best_move;
    
    if (best_score >= beta) {
        entry.flag = LOWER_BOUND;
    } else if (best_score <= alpha) {
        entry.flag = UPPER_BOUND;
    } else {
        entry.flag = EXACT;
    }
    
    return best_score;
}
```

\subsection zobrist_hashing 6.2 Zobrist Hashing

Zobrist hashing creates a unique key for each position:

```cpp
uint64_t zobrist_pieces[12][64];  // Random numbers for each piece on each square
uint64_t zobrist_castling[16];    // Random numbers for each castling state
uint64_t zobrist_enpassant[64];   // Random numbers for each en passant file
uint64_t zobrist_side_to_move;    // Random number for side to move

void init_zobrist() {
    // Initialize with random numbers
    for (int piece = 0; piece < 12; piece++) {
        for (int sq = 0; sq < 64; sq++) {
            zobrist_pieces[piece][sq] = random_uint64();
        }
    }
    // ... initialize others ...
}

uint64_t compute_zobrist(const Position& pos) {
    uint64_t hash = 0;
    
    // XOR in each piece
    for (int sq = 0; sq < 64; sq++) {
        Pieces piece = get_piece_at(pos, sq);
        if (piece != NO_PIECE) {
            hash ^= zobrist_pieces[piece][sq];
        }
    }
    
    // XOR in game state
    hash ^= zobrist_castling[pos.castelingRights];
    if (pos.enPassant) hash ^= zobrist_enpassant[get_file(pos.enPassant)];
    if (pos.whiteToMove) hash ^= zobrist_side_to_move;
    
    return hash;
}
```

**Incremental updates**: Update hash during make/unmake instead of recomputing.

\subsection move_ordering 6.3 Move Ordering

Alpha-beta works best when good moves are searched first:

```cpp
void order_moves(MoveList& moves, Move tt_move) {
    for (int i = 0; i < moves.count; i++) {
        Move& m = moves[i];
        
        // Hash move (from transposition table) - search first
        if (m == tt_move) {
            m.score = 1000000;
        }
        // Good captures (MVV-LVA: Most Valuable Victim - Least Valuable Attacker)
        else if (is_capture(m)) {
            int victim_value = piece_value[get_piece_at(pos, m.to)];
            int attacker_value = piece_value[get_piece_at(pos, m.from)];
            m.score = 10000 + victim_value - attacker_value;
        }
        // Killer moves (moves that caused cutoffs at same depth)
        else if (m == killer_moves[ply][0]) {
            m.score = 9000;
        }
        else if (m == killer_moves[ply][1]) {
            m.score = 8000;
        }
        // History heuristic
        else {
            m.score = history[m.from][m.to];
        }
    }
    
    // Sort moves by score (partial sort as we search)
    std::sort(moves.begin(), moves.end(), [](Move a, Move b) {
        return a.score > b.score;
    });
}
```

\subsection null_move_pruning 6.4 Null Move Pruning

If doing nothing (passing the turn) still gives us a good position, the current position is likely very good:

```cpp
int alphabeta(Position& pos, int depth, int alpha, int beta) {
    // ... transposition table probe ...
    
    // Null move pruning
    if (depth >= 3 && !is_in_check(pos) && has_non_pawn_material(pos)) {
        // Make null move (pass turn)
        pos.whiteToMove = !pos.whiteToMove;
        
        // Search with reduced depth
        int score = -alphabeta(pos, depth - 3, -beta, -beta + 1);
        
        // Unmake null move
        pos.whiteToMove = !pos.whiteToMove;
        
        if (score >= beta) {
            return beta;  // Cutoff
        }
    }
    
    // ... regular search ...
}
```

---

\section chapter7 Chapter 7: UCI Protocol

\subsection uci_overview 7.1 Overview

UCI (Universal Chess Interface) is the standard protocol for chess engines. The GUI sends text commands, the engine responds with text.

\subsection uci_commands 7.2 Main Commands

```cpp
void uci_loop() {
    Position pos;
    string line;
    
    while (getline(cin, line)) {
        istringstream iss(line);
        string command;
        iss >> command;
        
        if (command == "uci") {
            cout << "id name MyEngine" << endl;
            cout << "id author YourName" << endl;
            cout << "option name Hash type spin default 64 min 1 max 1024" << endl;
            cout << "uciok" << endl;
        }
        else if (command == "isready") {
            cout << "readyok" << endl;
        }
        else if (command == "ucinewgame") {
            pos.reset();
            transposition_table.clear();
        }
        else if (command == "position") {
            string type;
            iss >> type;
            
            if (type == "startpos") {
                pos.set_startpos();
                string moves_token;
                iss >> moves_token;  // "moves"
            }
            else if (type == "fen") {
                string fen;
                getline(iss, fen);
                pos.set_fen(fen);
            }
            
            // Apply moves
            string move_str;
            while (iss >> move_str) {
                Move m = parse_uci_move(move_str);
                makemove(pos, m);
            }
        }
        else if (command == "go") {
            // Parse go parameters
            int depth = MAX_DEPTH;
            int movetime = -1;
            
            string param;
            while (iss >> param) {
                if (param == "depth") {
                    iss >> depth;
                }
                else if (param == "movetime") {
                    iss >> movetime;
                }
                // ... parse wtime, btime, etc. ...
            }
            
            // Start search
            Move best_move = search(pos, depth, movetime);
            cout << "bestmove " << move_to_uci(best_move) << endl;
        }
        else if (command == "quit") {
            break;
        }
    }
}
```

\subsection uci_info 7.3 Info Strings

During search, send updates to GUI:

```cpp
void send_info(int depth, int score, int time_ms, long nodes, string pv) {
    cout << "info depth " << depth
         << " score cp " << score
         << " time " << time_ms
         << " nodes " << nodes
         << " nps " << (nodes * 1000 / max(time_ms, 1))
         << " pv " << pv
         << endl;
}
```

\subsection uci_moves 7.4 UCI Move Format

```cpp
string move_to_uci(Move m) {
    string uci = square_to_string(m.from) + square_to_string(m.to);
    
    if (m.promotion != NO_PIECE) {
        if (m.promotion == W_QUEEN || m.promotion == B_QUEEN) uci += 'q';
        else if (m.promotion == W_ROOK || m.promotion == B_ROOK) uci += 'r';
        else if (m.promotion == W_BISHOP || m.promotion == B_BISHOP) uci += 'b';
        else if (m.promotion == W_KNIGHT || m.promotion == B_KNIGHT) uci += 'n';
    }
    
    return uci;  // e.g., "e2e4" or "e7e8q"
}

Move parse_uci_move(string uci) {
    int from = string_to_square(uci.substr(0, 2));
    int to = string_to_square(uci.substr(2, 2));
    
    Pieces promotion = NO_PIECE;
    if (uci.length() == 5) {
        char promo = uci[4];
        bool white = (to / 8 == 7);
        if (promo == 'q') promotion = white ? W_QUEEN : B_QUEEN;
        else if (promo == 'r') promotion = white ? W_ROOK : B_ROOK;
        else if (promo == 'b') promotion = white ? W_BISHOP : B_BISHOP;
        else if (promo == 'n') promotion = white ? W_KNIGHT : B_KNIGHT;
    }
    
    return {from, to, promotion};
}
```

---

\section chapter8 Chapter 8: Testing and Debugging

\subsection perft_testing 8.1 Perft Testing

Perft (performance test) counts nodes at each depth. Compare with known results to verify move generation:

```cpp
long long perft(Position& pos, int depth) {
    if (depth == 0) return 1;
    
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    if (depth == 1) return moves.count;
    
    long long nodes = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves[i]);
        nodes += perft(pos, depth - 1);
        undomove(pos, moves[i]);
    }
    
    return nodes;
}

void test_perft() {
    Position pos;
    pos.set_startpos();
    
    // Known results for starting position
    assert(perft(pos, 1) == 20);
    assert(perft(pos, 2) == 400);
    assert(perft(pos, 3) == 8902);
    assert(perft(pos, 4) == 197281);
    assert(perft(pos, 5) == 4865609);
}
```

\subsection test_positions 8.2 Test Positions

Test your engine with known tactical positions:

```cpp
void test_tactics() {
    // Mate in 2
    Position pos;
    pos.set_fen("r1bqkb1r/pppp1ppp/2n2n2/4p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 0 1");
    Move best = search(pos, 4, -1);
    assert(move_to_uci(best) == "h5f7");  // Qxf7+
}
```

\subsection debugging_tips 8.3 Debugging Tips

**Print board state:**
```cpp
void print_board(const Position& pos) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            Pieces p = get_piece_at(pos, sq);
            cout << piece_to_char(p) << " ";
        }
        cout << endl;
    }
}
```

**Verify make/unmake:**
```cpp
void test_make_unmake() {
    Position pos;
    pos.set_startpos();
    
    uint64_t original_hash = pos.zobrist_key;
    
    MoveList moves;
    generate_legal_moves(pos, moves);
    
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves[i]);
        undomove(pos, moves[i]);
        
        assert(pos.zobrist_key == original_hash);
    }
}
```

---

\section chapter9 Chapter 9: Advanced Topics

\subsection parallel_search 9.1 Parallel Search

Use multiple threads to search faster:

```cpp
// Lazy SMP - each thread searches independently
void parallel_search(Position& pos, int depth) {
    vector<thread> threads;
    vector<Move> best_moves(num_threads);
    
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&, i]() {
            Position local_pos = pos;
            best_moves[i] = search(local_pos, depth, -1);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Use result from thread 0
    return best_moves[0];
}
```

\subsection late_move_reductions 9.2 Late Move Reductions

Reduce search depth for moves that are likely bad:

```cpp
for (int i = 0; i < moves.count; i++) {
    makemove(pos, moves[i]);
    
    int score;
    if (i >= 4 && depth >= 3 && !is_capture(moves[i]) && !is_check(pos)) {
        // Search with reduced depth first
        score = -alphabeta(pos, depth - 2, -alpha - 1, -alpha);
        
        if (score > alpha) {
            // Re-search with full depth
            score = -alphabeta(pos, depth - 1, -beta, -alpha);
        }
    } else {
        score = -alphabeta(pos, depth - 1, -beta, -alpha);
    }
    
    undomove(pos, moves[i]);
    
    // ... update alpha/beta ...
}
```

\subsection endgame_tablebases 9.3 Endgame Tablebases

For positions with few pieces, use pre-computed databases (Syzygy, Gaviota):

```cpp
if (count_pieces(pos) <= 5) {
    int result = probe_tablebase(pos);
    if (result != TB_RESULT_FAILED) {
        return tablebase_score(result);
    }
}
```

---

\section conclusion Conclusion

You now have the knowledge to write a chess engine! Key steps:

1. **Start simple**: Get bitboards and move generation working
2. **Test thoroughly**: Use perft and tactical puzzles
3. **Add search**: Start with minimax, add alpha-beta, then quiescence
4. **Optimize**: Add transposition tables, move ordering, null move pruning
5. **Implement UCI**: Connect to chess GUIs
6. **Iterate**: Test, profile, and improve

**Resources:**
- Chess Programming Wiki: https://www.chessprogramming.org/
- Engine testing: https://www.chessprogramming.org/Perft_Results
- Stockfish source code: https://github.com/official-stockfish/Stockfish
- UCI protocol: https://www.chessprogramming.org/UCI

**Final advice:**
- Write clean, well-tested code
- Profile before optimizing
- Study strong engines' source code
- Join the computer chess community (TalkChess, Discord)

Good luck with your chess engine!

*/
