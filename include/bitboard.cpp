#include <bitboard.h>

bool getBit( uint64_t bitboard, int rank, int file){
    uint64_t temporaryBitboard = 1ULL << (rank*8 + file);
    return bitboard & temporaryBitboard;
}
void setBit(uint64_t &bitboard, int rank, int file){
    uint64_t temporaryBitboard = 1ULL << (rank*8 + file);
    bitboard = bitboard | temporaryBitboard;
}
void ToggleBit(uint64_t &bitboard, int rank, int file){
    uint64_t temporaryBitboard = 1ULL << (rank*8 + file);
    bitboard = bitboard ^ temporaryBitboard;
}