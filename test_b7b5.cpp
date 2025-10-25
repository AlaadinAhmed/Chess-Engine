#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include <iostream>

int main() {
    history_ply = 0;
    
    Position pos;
    parseFEN(pos, "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1"); // After a2a3
    
    // Try b7b5
    Move m;
    m.from = 49;  // b7
    m.to = 33;    // b5
    m.promotion = NO_PIECE;
    
    std::cout << "Before move:" << std::endl;
    std::cout << "  WhiteKing: 0x" << std::hex << pos.WhiteKing << std::dec << std::endl;
    std::cout << "  BlackKing: 0x" << std::hex << pos.BlackKing << std::dec << std::endl;
    std::cout << "  enPassant: 0x" << std::hex << pos.enPassant << std::dec << std::endl;
    
    makemove(pos, m);
    
    std::cout << "\nAfter makemove b7b5:" << std::endl;
    std::cout << "  WhiteKing: 0x" << std::hex << pos.WhiteKing << std::dec << std::endl;
    std::cout << "  BlackKing: 0x" << std::hex << pos.BlackKing << std::dec << std::endl;
    std::cout << "  enPassant: 0x" << std::hex << pos.enPassant << std::dec << std::endl;
    std::cout << "  whiteToMove: " << pos.whiteToMove << std::endl;
    
    // Check if Black king is in check
    int king_sq = __builtin_ctzll(pos.BlackKing);
    bool in_check = is_square_attacked(pos, king_sq, true); // attacked by white?
    
    std::cout << "  Black king at square " << king_sq << std::endl;
    std::cout << "  In check: " << (in_check ? "YES" : "NO") << std::endl;
    
    return in_check ? 1 : 0;
}
