/**
 * @file nnue.cpp
 * @brief NNUE wrapper implementation using nnue-probe library
 */

#include "nnue.hpp"
#include <iostream>

// Include nnue.h with renamed Position to avoid conflict
#define Position NNUEPosition
#include "../extern/nnue-probe/src/nnue.h"
#undef Position

namespace nnue {

static bool initialized = false;

bool init(const char* network_file) {
    if (initialized) {
        return true;
    }
    
    try {
        nnue_init(network_file);
        initialized = true;
        std::cout << "info string NNUE loaded: " << network_file << std::endl;
        return true;
    } catch (...) {
        std::cerr << "info string Failed to load NNUE: " << network_file << std::endl;
        return false;
    }
}

bool is_initialized() {
    return initialized;
}

void shutdown() {
    initialized = false;
}

int evaluate(const Position& pos) {
    if (!initialized) {
        return 0; // Fallback: return 0 if not initialized
    }
    
    // Build piece and square arrays for nnue_evaluate
    // Format: piece[0] = white king, square[0] = its location
    //         piece[1] = black king, square[1] = its location
    //         ... other pieces in any order ...
    //         piece[n+1] = 0 to mark end
    
    int pieces[33];   // Max 32 pieces + 1 terminator
    int squares[33];
    int idx = 0;
    
    // White King first
    if (pos.WhiteKing) {
        pieces[idx] = 1;  // wking
        squares[idx] = __builtin_ctzll(pos.WhiteKing);
        idx++;
    }
    
    // Black King second
    if (pos.BlackKing) {
        pieces[idx] = 7;  // bking
        squares[idx] = __builtin_ctzll(pos.BlackKing);
        idx++;
    }
    
    // White pieces (using correct singular member names)
    uint64_t bb;
    
    bb = pos.WhiteQueen;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 2;  // wqueen
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.WhiteRooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 3;  // wrook
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.WhiteBishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 4;  // wbishop
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.WhiteKnights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 5;  // wknight
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.WhitePawns;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 6;  // wpawn
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Black pieces
    bb = pos.BlackQueen;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 8;  // bqueen
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.BlackRooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 9;  // brook
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.BlackBishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 10;  // bbishop
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.BlackKnights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 11;  // bknight
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    bb = pos.BlackPawns;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        pieces[idx] = 12;  // bpawn
        squares[idx] = sq;
        idx++;
        bb &= bb - 1;
    }
    
    // Terminator
    pieces[idx] = 0;
    
    // Call nnue_evaluate
    // player: 0 = white to move, 1 = black to move
    int player = pos.whiteToMove ? 0 : 1;
    int score = nnue_evaluate(player, pieces, squares);
    
    return score;
}

} // namespace nnue

// Extern function for eval.cpp to call (outside namespace for C linkage)
extern "C" int nnue_evaluate_position(Position& pos) {
    return nnue::evaluate(pos);
}
