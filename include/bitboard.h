#include <cstdint>
uint64_t FileA = 0x0101010101010101ULL;
uint64_t FileB = FileA << 1;
uint64_t FileC = FileA << 2;
uint64_t FileD = FileA << 3;
uint64_t FileE = FileA << 4;
uint64_t FileF = FileA << 5;
uint64_t FileG = FileA << 6;
uint64_t FileH = FileA << 7;

uint64_t Rank1 = 0xFFULL;
uint64_t Rank2 = Rank1 << (8*1);
uint64_t Rank3 = Rank1 << (8*2);
uint64_t Rank4 = Rank1 << (8*3);
uint64_t Rank5 = Rank1 << (8*4);
uint64_t Rank6 = Rank1 << (8*5);
uint64_t Rank7 = Rank1 << (8*6);
uint64_t Rank8 = Rank1 << (8*7);
                      //  1 2 3 4 5 6 7 8
uint64_t BlackKnights = 0x4200000000000000;
uint64_t BlackBishops = 0x2400000000000000;
uint64_t BlackRooks = 0x8100000000000000;
uint64_t BlackQueen = 0x0800000000000000;
uint64_t BlackKing = 0x1000000000000000;

uint64_t WhiteKnights = BlackKnights >> (8*7);
uint64_t WhiteBishops = BlackBishops >> (8*7);
uint64_t WhiteRooks = BlackRooks >> (8*7);
uint64_t WhiteQueen = BlackQueen >> (8*7);
uint64_t WhiteKing = BlackKing >> (8*7);
