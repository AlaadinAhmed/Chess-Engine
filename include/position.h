#pragma once
#include <cstdint>
struct Position{
    uint64_t BlackKnights;
    uint64_t BlackBishops;
    uint64_t BlackRooks;
    uint64_t BlackQueen;
    uint64_t BlackKing;
    uint64_t BlackPawns;

    uint64_t WhiteKnights;
    uint64_t WhiteBishops;
    uint64_t WhiteRooks;
    uint64_t WhiteQueen;
    uint64_t WhiteKing;
    uint64_t WhitePawns;

    bool whiteToMove;
    uint64_t unpassant;
    int move50rule;
    int move;
    uint8_t castlingRights;

    uint64_t BlackoccupiedSquares;
    uint64_t WhiteoccupiedSquares;
    uint64_t occupiedSquares;
    uint64_t emptySquares;
};