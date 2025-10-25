#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include <iostream>

int main() {
    history_ply = 0;
    
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    // Test the move g2h3 - from g2(14) to h3(23)
    Move m;
    m.from = 14;  
    m.to = 23;    
    m.promotion = NO_PIECE;
    
    std::cout << "Testing move: " << move_to_uci(m) << std::endl;
    std::cout << "Piece at g2: " << get_piece_at(pos, 14) << std::endl;
    std::cout << "Piece at h3: " << get_piece_at(pos, 23) << std::endl;
    
    uint64_t orig_wking = pos.WhiteKing;
    std::cout << "Before: WhiteKing = 0x" << std::hex << orig_wking << std::dec << std::endl;
    
    makemove(pos, m);
    std::cout << "After make: WhiteKing = 0x" << std::hex << pos.WhiteKing << std::dec << std::endl;
    
    if (pos.WhiteKing == 0) {
        std::cout << "ERROR: King captured!" << std::endl;
        return 1;
    }
    
    undomove(pos, m);
    std::cout << "After unmake: WhiteKing = 0x" << std::hex << pos.WhiteKing << std::dec << std::endl;
    
    if (pos.WhiteKing != orig_wking) {
        std::cout << "ERROR: WhiteKing not restored!" << std::endl;
        return 1;
    }
    
    std::cout << "SUCCESS!" << std::endl;
    return 0;
}
