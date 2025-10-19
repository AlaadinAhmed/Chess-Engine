#include <iostream>
#include "movegen.hpp"
#include "fen.hpp"
#include "utils.hpp"

int main() {
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Position loaded. White to move: " << pos.whiteToMove << std::endl;
    
    // Check what's on square a3 (square 16)
    std::cout << "Square a3 (16): ";
    if (pos.WhitePawns & (1ULL << 16)) std::cout << "White Pawn";
    else if (pos.BlackPawns & (1ULL << 16)) std::cout << "Black Pawn";
    else if (pos.WhiteKnights & (1ULL << 16)) std::cout << "White Knight";
    else if (pos.BlackKnights & (1ULL << 16)) std::cout << "Black Knight";
    else if (pos.WhiteBishops & (1ULL << 16)) std::cout << "White Bishop";
    else if (pos.BlackBishops & (1ULL << 16)) std::cout << "Black Bishop";
    else if (pos.WhiteRooks & (1ULL << 16)) std::cout << "White Rook";
    else if (pos.BlackRooks & (1ULL << 16)) std::cout << "Black Rook";
    else if (pos.WhiteQueen & (1ULL << 16)) std::cout << "White Queen";
    else if (pos.BlackQueen & (1ULL << 16)) std::cout << "Black Queen";
    else if (pos.WhiteKing & (1ULL << 16)) std::cout << "White King";
    else if (pos.BlackKing & (1ULL << 16)) std::cout << "Black King";
    else std::cout << "Empty";
    std::cout << std::endl;
    
    // Check what's on square b4 (square 25)
    std::cout << "Square b4 (25): ";
    if (pos.WhitePawns & (1ULL << 25)) std::cout << "White Pawn";
    else if (pos.BlackPawns & (1ULL << 25)) std::cout << "Black Pawn";
    else if (pos.WhiteKnights & (1ULL << 25)) std::cout << "White Knight";
    else if (pos.BlackKnights & (1ULL << 25)) std::cout << "Black Knight";
    else if (pos.WhiteBishops & (1ULL << 25)) std::cout << "White Bishop";
    else if (pos.BlackBishops & (1ULL << 25)) std::cout << "Black Bishop";
    else if (pos.WhiteRooks & (1ULL << 25)) std::cout << "White Rook";
    else if (pos.BlackRooks & (1ULL << 25)) std::cout << "Black Rook";
    else if (pos.WhiteQueen & (1ULL << 25)) std::cout << "White Queen";
    else if (pos.BlackQueen & (1ULL << 25)) std::cout << "Black Queen";
    else if (pos.WhiteKing & (1ULL << 25)) std::cout << "White King";
    else if (pos.BlackKing & (1ULL << 25)) std::cout << "Black King";
    else std::cout << "Empty";
    std::cout << std::endl;
    
    // Generate moves and look for a3b4
    MoveList moves;
    generate_moves(pos, moves);
    
    std::cout << "Total moves generated: " << moves.count << std::endl;
    
    bool found_a3b4 = false;
    for (int i = 0; i < moves.count; ++i) {
        if (moves.moves[i].from == 16 && moves.moves[i].to == 25) {
            found_a3b4 = true;
            std::cout << "FOUND a3b4 move: from=" << moves.moves[i].from << " to=" << moves.moves[i].to 
                      << " promotion=" << moves.moves[i].promotion << std::endl;
            break;
        }
    }
    
    if (!found_a3b4) {
        std::cout << "a3b4 is NOT in the generated moves" << std::endl;
    }
    
    return 0;
}
