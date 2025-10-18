#pragma once

#include <cstdint>
#include "position.hpp" // Include position.hpp for Move and Pieces
#include "tt.hpp"

struct UndoInfo {
    Move move;
    Pieces oldCapturedPiece;
    uint64_t oldEnPassant;
    uint8_t oldCastelingRights;
    int oldHalfMove;
    uint64_t oldHashKey;
    bool side; // whiteToMove
    bool isEnPassant;
    int enPassantCapturedPawnSquare;
    bool isCastling;
    int castlingRookFrom;
    int castlingRookTo;
};

extern uint64_t currentHashKey;
extern TranspositionTable tt;
extern int history_ply;
extern bool debug_mode;
