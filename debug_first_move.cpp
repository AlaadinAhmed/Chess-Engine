#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include <iostream>

int main() {
    zkey.initKeys();
    
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Initial position, White to move" << std::endl;
    std::cout << "White King at: " << __builtin_ctzll(pos.WhiteKing) << std::endl;
    
    // Try the move that's being made at depth 1
    Move m;
    m.from = 11; // d2
    m.to = 4;    // e1
    m.moveType = NORMAL_MOVE;
    
    std::cout << "\nTrying move d2e1..." << std::endl;
    makemove(pos, m);
    
    std::cout << "After move, Black to move" << std::endl;
    std::cout << "White King at: " << __builtin_ctzll(pos.WhiteKing) << std::endl;
    
    int wking_sq = __builtin_ctzll(pos.WhiteKing);
    bool in_check = is_square_attacked(pos, wking_sq, false); // false = attacked by black
    
    std::cout << "White king in check: " << (in_check ? "YES" : "NO") << std::endl;
    
    return 0;
}
