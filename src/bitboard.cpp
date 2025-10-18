#include "bitboard.hpp"
#include <cstdint>

uint64_t kingAttacks[64];
uint64_t knightAttacks[64];

const uint64_t FileA = 0x0101010101010101ULL;
const uint64_t FileH = 0x8080808080808080ULL;
const uint64_t Rank1 = 0x00000000000000FFULL;
const uint64_t Rank8 = 0xFF00000000000000ULL;

bool getBit( uint64_t bitboard, int square){
    uint64_t temporaryBitboard = 1ULL << square;
    return (bitboard & temporaryBitboard);
}
void setBit(uint64_t &bitboard, int square){
    uint64_t temporaryBitboard = 1ULL << square;
    bitboard = bitboard | temporaryBitboard;
}
void ToggleBit(uint64_t &bitboard, int square){
    uint64_t temporaryBitboard = 1ULL << square;
    bitboard = bitboard ^ temporaryBitboard;
}
uint64_t setBitboard(int square){
    uint64_t bitboard = 1ULL << square;
    return bitboard;
}