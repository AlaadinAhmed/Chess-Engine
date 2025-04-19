#pragma once
#include <bit>
#include <cstdint>
#include <vector>


// Const Values
uint64_t const FileA = 0x0101010101010101ULL;
uint64_t const FileB = FileA << 1;
uint64_t const FileC = FileA << 2;
uint64_t const FileD = FileA << 3;
uint64_t const FileE = FileA << 4;
uint64_t const FileF = FileA << 5;
uint64_t const FileG = FileA << 6;
uint64_t const FileH = FileA << 7;

uint64_t const Rank1 = 0xFFULL;
uint64_t const Rank2 = Rank1 << (8*1);
uint64_t const Rank3 = Rank1 << (8*2);
uint64_t const Rank4 = Rank1 << (8*3);
uint64_t const Rank5 = Rank1 << (8*4);
uint64_t const Rank6 = Rank1 << (8*5);
uint64_t const Rank7 = Rank1 << (8*6);
uint64_t const Rank8 = Rank1 << (8*7);

// Default values
uint64_t BlackKnights = 0x4200000000000000ULL;
uint64_t BlackBishops = 0x2400000000000000ULL;
uint64_t BlackRooks = 0x8100000000000000ULL;
uint64_t BlackQueen = 0x0800000000000000ULL;
uint64_t BlackKing = 0x1000000000000000ULL;
uint64_t BlackPawns = 0x00FF000000000000ULL;

uint64_t WhiteKnights = BlackKnights >> (8*7);
uint64_t WhiteBishops = BlackBishops >> (8*7);
uint64_t WhiteRooks = BlackRooks >> (8*7);
uint64_t WhiteQueen = BlackQueen >> (8*7);
uint64_t WhiteKing = BlackKing >> (8*7);
uint64_t WhitePawns = BlackPawns >> (8*6);

uint64_t occupiedSquares = BlackKnights | BlackBishops | BlackKing | BlackPawns | BlackQueen | BlackRooks | WhiteBishops | WhiteKnights | WhiteKing | WhitePawns | WhiteQueen | WhiteRooks;

uint64_t emptySquares = ~occupiedSquares;

uint8_t WhitePawns2Sqaures = 0xFF;
uint8_t BlackPawns2Squares = 0xFF;

const uint64_t knightMasks[64] = {
    0x0020400000000000ULL, 0x0010A00000000000ULL, 0x0088500000000000ULL, 0x0044280000000000ULL, // a8,b8,c8,d8
    0x0022120000000000ULL, 0x00110A0000000000ULL, 0x0008050000000000ULL, 0x0004020000000000ULL, // e8,f8,g8,h8
    0x2000204000000000ULL, 0x100010A000000000ULL, 0x8800885000000000ULL, 0x4400442800000000ULL, // a7,b7,c7,d7
    0x2200221400000000ULL, 0x1100110500000000ULL, 0x0800080500000000ULL, 0x0400040200000000ULL, // e7,f7,g7,h7
    0xA0100010A0000000ULL, 0x0040800000000000ULL, 0x5088008850000000ULL, 0x2844004428000000ULL, // a6,b6,c6,d6
    0x1422002214000000ULL, 0x0A1100110A000000ULL, 0x0508000805000000ULL, 0x0204000402000000ULL, // e6,f6,g6,h6
    0x0040200020400000ULL, 0x00A0100010A00000ULL, 0x0050880088500000ULL, 0x0028440044280000ULL, // a5,b5,c5,d5
    0x0014220022140000ULL, 0x000A1100110A0000ULL, 0x0005080008050000ULL, 0x0002040004020000ULL, // e5,f5,g5,h5
    0x0000402000204000ULL, 0x0000A0100010A000ULL, 0x0000508800885000ULL, 0x0000284400442800ULL, // a4,b4,c4,d4
    0x0000142200221400ULL, 0x00000A1100110A00ULL, 0x0000050800080500ULL, 0x0000020400040200ULL, // e4,f4,g4,h4
    0x0000004020002040ULL, 0x000000A0100010A0ULL, 0x0000005088008850ULL, 0x0000002844004428ULL, // a3,b3,c3,d3
    0x0000001422002214ULL, 0x0000000A1100110AULL, 0x0000000508000805ULL, 0x0000000204000402ULL, // e3,f3,g3,h3
    0x0000000040200020ULL, 0x00000000A0100010ULL, 0x0000000050880088ULL, 0x0000000028440044ULL, // a2,b2,c2,d2
    0x0000000014220022ULL, 0x000000000A110011ULL, 0x0000000005080008ULL, 0x0000000002040004ULL, // e2,f2,g2,h2
    0x0000000000402000ULL, 0x0000000000A01000ULL, 0x0000000000508800ULL, 0x0000000000284400ULL, // a1,b1,c1,d1
    0x0000000000142200ULL, 0x00000000000A1100ULL, 0x0000000000050800ULL, 0x0000000000020400ULL, // e1,f1,g1,h1
};

struct  Move{
    uint8_t from;
    uint8_t to;
};

bool getBit(uint64_t bitboard, int rank, int file);
void setBit(uint64_t &bitboard, int rank, int file);
void ToggleBit(uint64_t &bitboard, int rank, int file);

