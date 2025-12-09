/**
 * @file nnue.cpp
 * @brief NNUE wrapper implementation using stockfish_nnue_probe library
 */

#include "nnue.hpp"
#include "probe.h"
#include <iostream>

namespace nnue {

static bool initialized = false;

bool init(const char* big_net_file, const char* small_net_file) {
    if (initialized) {
        return true;
    }
    
    // Check if files exist first
    FILE* f1 = fopen(big_net_file, "rb");
    FILE* f2 = fopen(small_net_file, "rb");
    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return false;
    }
    fclose(f1);
    fclose(f2);
    
    Stockfish::Probe::init(big_net_file, small_net_file);
    initialized = true;
    std::cout << "info string NNUE loaded: " << big_net_file << " + " << small_net_file << std::endl;
    return true;
}

bool init(const char* network_file) {
    // Legacy single-file init - try to find companion file
    // Not supported by new library, return false
    return false;
}

bool is_initialized() {
    return initialized;
}

void shutdown() {
    initialized = false;
}

/**
 * Convert OctoKnight piece type to stockfish_nnue_probe piece code
 * stockfish_nnue_probe uses:
 *   0 = empty
 *   1-6 = white pawn/knight/bishop/rook/queen/king
 *   9-14 = black pawn/knight/bishop/rook/queen/king
 */
static int piece_to_nnue(Pieces p) {
    switch (p) {
        case W_PAWN:   return 1;
        case W_KNIGHT: return 2;
        case W_BISHOP: return 3;
        case W_ROOK:   return 4;
        case W_QUEEN:  return 5;
        case W_KING:   return 6;
        case B_PAWN:   return 9;
        case B_KNIGHT: return 10;
        case B_BISHOP: return 11;
        case B_ROOK:   return 12;
        case B_QUEEN:  return 13;
        case B_KING:   return 14;
        default:       return 0;
    }
}

int evaluate(const Position& pos) {
    if (!initialized) {
        return 0; // Fallback: return 0 if not initialized
    }
    
    // Build piece and square arrays for Stockfish::Probe::eval
    int pieces[32];
    int squares[32];
    int idx = 0;
    
    // Iterate through all pieces on the board
    uint64_t bb;
    
    // White King
    if (pos.WhiteKing) {
        pieces[idx] = 6;
        squares[idx] = __builtin_ctzll(pos.WhiteKing);
        idx++;
    }
    
    // White Queen
    bb = pos.WhiteQueen;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 5;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // White Rooks
    bb = pos.WhiteRooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 4;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // White Bishops
    bb = pos.WhiteBishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 3;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // White Knights
    bb = pos.WhiteKnights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 2;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // White Pawns
    bb = pos.WhitePawns;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 1;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Black King
    if (pos.BlackKing) {
        pieces[idx] = 14;
        squares[idx] = __builtin_ctzll(pos.BlackKing);
        idx++;
    }
    
    // Black Queen
    bb = pos.BlackQueen;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 13;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Black Rooks
    bb = pos.BlackRooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 12;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Black Bishops
    bb = pos.BlackBishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 11;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Black Knights
    bb = pos.BlackKnights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 10;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Black Pawns
    bb = pos.BlackPawns;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 9;
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Call Stockfish::Probe::eval
    // side: 0 for white, 1 for black
    // rule50: halfmove clock (0-100)
    bool side = !pos.whiteToMove;  // 0 = white, 1 = black
    int rule50 = pos.move50rule;
    
    int score = Stockfish::Probe::eval(pieces, squares, idx, side, rule50);
    
    // Score is from white's perspective, convert to side-to-move perspective
    return pos.whiteToMove ? score : -score;
}

} // namespace nnue

// Extern functions for eval.cpp to call (outside namespace)
int nnue_evaluate_position(Position& pos) {
    return nnue::evaluate(pos);
}

bool nnue_is_initialized() {
    return nnue::is_initialized();
}
