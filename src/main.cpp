#include "bitboard.h"
#include <iostream>
#include <string>
#include <cstdint>
void printBoard(uint64_t pieces) {
    for (int i = 0; i < 64; i++) {
        // Print a piece if the current bit is set
        if (pieces & (1ULL << (63 - i))) {
            std::cout << "X ";  // "X" represents a piece
        } else {
            std::cout << ". ";  // "." represents an empty square
        }
        // After every 8 bits (one rank), print a new line
        if ((i + 1) % 8 == 0) {
            std::cout << std::endl;
        }
    }
}
int main() {
    // uint64_t board[12] = {
    //     BlackKnights, BlackBishops, BlackRooks, BlackQueen, BlackKing, BlackPawns,
    //     WhiteKnights, WhiteBishops, WhiteRooks, WhiteQueen, WhiteKing, WhitePawns
    // };
    // // Print each piece's position on the board
    // std::cout << "Black Pieces:" << std::endl;
    // printBoard(BlackKnights);
    // printBoard(BlackBishops);
    // printBoard(BlackRooks);
    // printBoard(BlackQueen);
    // printBoard(BlackKing);
    // printBoard(BlackPawns);
    // std::cout << "\nWhite Pieces:" << std::endl;
    // printBoard(WhiteKnights);
    // printBoard(WhiteBishops);
    // printBoard(WhiteRooks);
    // printBoard(WhiteQueen);
    // printBoard(WhiteKing);
    // printBoard(WhitePawns);
    for (int j = 0; j < 64; j++){
        printBoard(knightMasks[j]);
        std::cout << std::endl;
    }
    return 0;
}