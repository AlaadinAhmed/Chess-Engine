#include "bitboard.hpp"
#include <cstdint>

uint64_t kingAttacks[64];

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
