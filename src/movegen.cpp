#include <cstdlib>
#include <iostream>
#include "position.hpp"
#include "movegen.hpp"
#include "magics.hpp"
#include "print.hpp"
// uint64_t peekAttackedSquares(Position pos) {
//     uint64_t attackedSquares = 0ULL;
    
//     // Loop through all squares
//     for (int square = 0; square < 64; square++) {
//         uint64_t bitboard = setBitboard(square);
        
//         // Pawn attacks
//         if (pos.whiteToMove) {
//             if (pos.BlackPawns & bitboard) {
//                 attackedSquares |= GetPawnAttacks(pos, square);
//             }
//         } else {
//             if (pos.WhitePawns & bitboard) {
//                 attackedSquares |= GetPawnAttacks(pos, square);
//             }
//         }

//         // Knight attacks
//         if (pos.whiteToMove) {
//             if (pos.BlackKnights & bitboard) {
//                 attackedSquares |= knightAttacks[square];
//             }
//         } else {
//             if (pos.WhiteKnights & bitboard) {
//                 attackedSquares |= knightAttacks[square];
//             }
//         }

//         // Bishop attacks
//         if (pos.whiteToMove) {
//             if (pos.BlackBishops & bitboard) {
//                 attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares);
//             }
//         } else {
//             if (pos.WhiteBishops & bitboard) {
//                 attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares);
//             }
//         }

//         // Rook attacks
//         if (pos.whiteToMove) {
//             if (pos.BlackRooks & bitboard) {
//                 attackedSquares |= get_rook_attacks(square, pos.occupiedSquares);
//             }
//         } else {
//             if (pos.WhiteRooks & bitboard) {
//                 attackedSquares |= get_rook_attacks(square, pos.occupiedSquares);
//             }
//         }

//         // Queen attacks (combines rook and bishop attacks)
//     if (pos.whiteToMove) {
//         if (pos.BlackQueen & bitboard) {
//             attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares) | 
//                              get_rook_attacks(square, pos.occupiedSquares);
//         }
//     } else {
//         if (pos.WhiteQueen & bitboard) {
//             attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares) | 
//                              get_rook_attacks(square, pos.occupiedSquares);
//         }
//     }
    
//     // King attacks
//     if (pos.whiteToMove) {
//         if (pos.BlackKing & bitboard) {
//             attackedSquares |= kingAttacks[square];
//         }
//     } else {
//         if (pos.WhiteKing & bitboard) {
//             attackedSquares |= kingAttacks[square];
//         }
//     }
    
// }
// return attackedSquares;
// }
uint64_t peekAttackedSquares(Position pos) {
    uint64_t attackedSquares = 0ULL;

    // Loop through all squares
    for (int square = 0; square < 64; square++) {
        uint64_t bitboard = setBitboard(square);

        // Pawn attacks - check opponent's pawns
        if (pos.whiteToMove) {  // Opponent is white
            if (pos.WhitePawns & bitboard) {
                attackedSquares |= GetPawnAttacks(pos, square);
            }
        } else {  // Opponent is black
            if (pos.BlackPawns & bitboard) {
                attackedSquares |= GetPawnAttacks(pos, square);
            }
        }
        
        // Knight attacks - check opponent's knights
        if (pos.whiteToMove) {  // Opponent is white
            if (pos.WhiteKnights & bitboard) {
                attackedSquares |= knightAttacks[square];
            }
        } else {  // Opponent is black
            if (pos.BlackKnights & bitboard) {
                attackedSquares |= knightAttacks[square];
            }
        }

        // Bishop attacks - check opponent's bishops
        if (pos.whiteToMove) {  // Opponent is white
            if (pos.WhiteBishops & bitboard) {
                attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares);
            }
        } else {  // Opponent is black
            if (pos.BlackBishops & bitboard) {
                attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares);
            }
        }

        // Rook attacks - check opponent's rooks
        if (pos.whiteToMove) {  // Opponent is white
            if (pos.WhiteRooks & bitboard) {
                attackedSquares |= get_rook_attacks(square, pos.occupiedSquares);
            }
        } else {  // Opponent is black
            if (pos.BlackRooks & bitboard) {
                attackedSquares |= get_rook_attacks(square, pos.occupiedSquares);
            }
        }

        // Queen attacks - check opponent's queens
        if (pos.whiteToMove) {  // Opponent is white
            if (pos.WhiteQueen & bitboard) {
                attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares) |
                                   get_rook_attacks(square, pos.occupiedSquares);
            }
        } else {  // Opponent is black
            if (pos.BlackQueen & bitboard) {
                attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares) |
                                   get_rook_attacks(square, pos.occupiedSquares);
            }
        }

        // King attacks - check opponent's king
        if (pos.whiteToMove) {  // Opponent is white
            if (pos.WhiteKing & bitboard) {
                attackedSquares |= kingAttacks[square];
            }
        } else {  // Opponent is black
            if (pos.BlackKing & bitboard) {
                attackedSquares |= kingAttacks[square];
            }
        }
        // std::cout << square << ": attacked squares\n";
        // printBoard(attackedSquares);
    }
    
    return attackedSquares;
}

uint64_t GetPawnMoves(Position pos, int square) {
    uint64_t moves = 0ULL;
    uint64_t pawnBitboard = 1ULL << square;
    
    if (pos.whiteToMove) {
        // Check if square has a white pawn
        if (pawnBitboard & pos.WhitePawns) {
            // Single push (up one rank)
            if ((pawnBitboard << 8) & pos.emptySquares) {
                moves |= (pawnBitboard << 8);
                
                // Double push (only from rank 2)
                if ((square >= 8 && square <= 15) && ((pawnBitboard << 16) & pos.emptySquares)) {
                    moves |= (pawnBitboard << 16);
                }
            }
            
            // Captures to the left (if not on a-file)
            if ((square % 8) != 0 && ((pawnBitboard << 7) & pos.BlackoccupiedSquares)) {
                moves |= (pawnBitboard << 7);
            }
            
            // Captures to the right (if not on h-file)
            if ((square % 8) != 7 && ((pawnBitboard << 9) & pos.BlackoccupiedSquares)) {
                moves |= (pawnBitboard << 9);
            }
        }
    } else {
        // Check if square has a black pawn
        if (pawnBitboard & pos.BlackPawns) {
            // Single push (down one rank)
            if ((pawnBitboard >> 8) & pos.emptySquares) {
                moves |= (pawnBitboard >> 8);
                
                // Double push (only from rank 7)
                if ((square >= 48 && square <= 55) && ((pawnBitboard >> 16) & pos.emptySquares)) {
                    moves |= (pawnBitboard >> 16);
                }
            }
            
            // Captures to the left (if not on a-file)
            if ((square % 8) != 0 && ((pawnBitboard >> 9) & pos.WhiteoccupiedSquares)) {
                moves |= (pawnBitboard >> 9);
            }
            
            // Captures to the right (if not on h-file)
            if ((square % 8) != 7 && ((pawnBitboard >> 7) & pos.WhiteoccupiedSquares)) {
                moves |= (pawnBitboard >> 7);
            }
        }
    }
    
    return moves;
}

uint64_t GetPawnAttacks(Position pos, int square) {
    uint64_t attacks = 0ULL;
    uint64_t pawnBitboard = setBitboard(square);

    if (pos.whiteToMove) {
        // Regular white pawn attacks
        if ((pawnBitboard & ~FileA) != 0) {
            attacks |= (pawnBitboard << 7);
        }
        if ((pawnBitboard & ~FileH) != 0) {
            attacks |= (pawnBitboard << 9);
        }
    } else {
        // Regular black pawn attacks
        if ((pawnBitboard & ~FileA) != 0) {
            attacks |= (pawnBitboard >> 9);
        }
        if ((pawnBitboard & ~FileH) != 0) {
            attacks |= (pawnBitboard >> 7);
        }
    }

    return attacks;
}
uint64_t GetKingMoves(Position pos) {
    uint64_t moves = 0ULL;
    int kingSquare;
    
    if (pos.whiteToMove) {
        kingSquare = __builtin_ctzll(pos.WhiteKing);
        // Get basic king moves from lookup table
        moves = kingAttacks[kingSquare];
        
        // Create temporary position to check opponent's attacks
        Position tempPos = pos;
        tempPos.whiteToMove = false;
        uint64_t attackedSquares = peekAttackedSquares(tempPos);
        // std::cout << "attacked squares\n";
        // printBoard(attackedSquares);

        // Remove squares occupied by friendly pieces and attacked squares
        moves &= ~(pos.WhiteoccupiedSquares | attackedSquares);
    } else {
        kingSquare = __builtin_ctzll(pos.BlackKing);
        moves = kingAttacks[kingSquare];
        
        Position tempPos = pos;
        tempPos.whiteToMove = true;
        uint64_t attackedSquares = peekAttackedSquares(tempPos);
        
        moves &= ~(pos.BlackoccupiedSquares | attackedSquares);
    }
    
    return moves;
}

// Also need to fix kingAttacks initialization
void initKingAttacks() {
    for (int square = 0; square < 64; square++) {
        uint64_t bb = 1ULL << square;
        uint64_t attacks = 0ULL;
        
        // Generate king moves in all 8 directions
        attacks |= (bb << 8) & ~Rank8;  // North
        attacks |= (bb >> 8) & ~Rank1;  // South
        attacks |= (bb << 1) & ~FileH;  // East
        attacks |= (bb >> 1) & ~FileA;  // West
        attacks |= (bb << 9) & ~(Rank8 | FileH);  // Northeast
        attacks |= (bb << 7) & ~(Rank8 | FileA);  // Northwest
        attacks |= (bb >> 7) & ~(Rank1 | FileH);  // Southeast
        attacks |= (bb >> 9) & ~(Rank1 | FileA);  // Southwest
        
        kingAttacks[square] = attacks;
    }
}
// uint64_t GetKingMoves(Position pos, uint64_t taboo) {
//     uint64_t moves = 0ULL;
//     int kingSquare;
    
//     // Store original whiteToMove value
//     bool originalTurn = pos.whiteToMove;
    
//     // Switch turn to get opponent's attacks
//     pos.whiteToMove = !pos.whiteToMove;
//     uint64_t attackedSquares = peekAttackedSquares(pos);
    
//     // Restore original turn
//     pos.whiteToMove = originalTurn;
    
//     if (pos.whiteToMove) {
//         kingSquare = __builtin_ctzll(pos.WhiteKing);
//         moves = kingAttacks[kingSquare];
//         // Remove squares occupied by friendly pieces and attacked squares
//         moves &= ~(pos.WhiteoccupiedSquares | attackedSquares);
//     } else {
//         kingSquare = __builtin_ctzll(pos.BlackKing);
//         moves = kingAttacks[kingSquare];
//         // Remove squares occupied by friendly pieces and attacked squares
//         moves &= ~(pos.BlackoccupiedSquares | attackedSquares);
//     }
    
//     return moves;
// }
// uint64_t GetKnightAttacks (Position pos, int square){
//     if (pos.whiteToMove){
//         return knightAttacks[square] & pos.WhiteoccupiedSquares;
//     }
//     return knightAttacks[square] & pos.BlackoccupiedSquares;
// }
uint64_t GetKnightAttacks(Position pos, int square) {
    if (pos.whiteToMove) {
        return knightAttacks[square] & ~(pos.WhiteoccupiedSquares | pos.BlackoccupiedSquares);
    }
    return knightAttacks[square] & ~(pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares);
}