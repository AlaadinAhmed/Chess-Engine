#pragma once
#include <cstdint>
struct Position{
    uint64_t BlackKnights = 0;
    uint64_t BlackBishops = 0;
    uint64_t BlackRooks = 0;
    uint64_t BlackQueen = 0;
    uint64_t BlackKing = 0;
    uint64_t BlackPawns = 0;

    uint64_t WhiteKnights = 0;
    uint64_t WhiteBishops = 0;
    uint64_t WhiteRooks = 0;
    uint64_t WhiteQueen = 0;
    uint64_t WhiteKing = 0;
    uint64_t WhitePawns = 0;

    bool whiteToMove ;
    uint64_t enPassant = 0;
    int move50rule = 0;
    int move = 0;
    uint8_t castelingRights = 0;

    uint64_t BlackoccupiedSquares = 0;
    uint64_t WhiteoccupiedSquares = 0;
    uint64_t occupiedSquares = 0;
    uint64_t emptySquares = 0;
};