#include <iostream>
#include <string>
#include <cstdint>
#include "print.hpp"
#include "bitboard.hpp"
#include "fen.hpp"
#include "position.hpp"
#include "movegen.hpp"

void testMoveGeneration() {
    Position board;
    // Test starting position
    parseFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    std::cout << "Testing from starting position:\n\n";
    
    // Test pawn moves
    std::cout << "White pawn moves from e2:\n";
    uint64_t whitePawnMoves = GetPawnMoves(board, 12); // e2 square
    printBoard(whitePawnMoves);
    std::cout << "Expected moves: e3 and e4\n\n";
    
    std::cout << "Black pawn moves from e7:\n";
    board.whiteToMove = false;
    uint64_t blackPawnMoves = GetPawnMoves(board, 52); // e7 square
    printBoard(blackPawnMoves);
    std::cout << "Expected moves: e6 and e5\n\n";
    
    // Test king moves
    board.whiteToMove = true;
    std::cout << "White king moves (should be empty in starting position):\n";
    uint64_t whiteKingMoves = GetKingMoves(board, 0);
    printBoard(whiteKingMoves);
    std::cout << "\n";
    
    // Test knight moves
    std::cout << "Knight attacks from b1:\n";
    uint64_t knightAttacks = GetKnightAttacks(board,57 );
    printBoard(knightAttacks);
    std::cout << "\n";
    
    // Test attacked squares
    std::cout << "Squares attacked by black:\n";
    board.whiteToMove = true;
    uint64_t attackedSquares = peekAttackedSquares(board);
    printBoard(attackedSquares);
    std::cout << "\n";
    
    // Test middle game position
    std::cout << "Testing from middle game position:\n";
    parseFEN(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    printBoard(board.occupiedSquares);
    std::cout << "\n";
    
    board.whiteToMove = true;
    std::cout << "White king moves in middle game:\n";
    whiteKingMoves = GetKingMoves(board, 0);
    printBoard(whiteKingMoves);
    std::cout << "\n";
}

int main() {
    Position pos;
parseFEN(pos, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
printBoard(pos.WhitePawns);  // Should show pawns on rank 2
printBoard(pos.BlackPawns);  // Should show pawns on rank
    testMoveGeneration();
    return 0;
}