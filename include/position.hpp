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
    
    void reset(){
    BlackKnights = 0;
    BlackBishops = 0;
    BlackRooks = 0;
    BlackQueen = 0;
    BlackKing = 0;
    BlackPawns = 0;

    WhiteKnights = 0;
    WhiteBishops = 0;
    WhiteRooks = 0;
    WhiteQueen = 0;
    WhiteKing = 0;
    WhitePawns = 0;

    enPassant = 0;
    move50rule = 0;
    move = 0;
    castelingRights = 0;

    BlackoccupiedSquares = 0;
    WhiteoccupiedSquares = 0;
    occupiedSquares = 0;
    emptySquares = 0;
    }
};