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
    
    // Test the move a1e1
    Move m;
    m.from = 0;  // a1
    m.to = 4;    // e1
    m.promotion = NO_PIECE;
    
    std::cout << "Testing move: " << move_to_uci(m) << std::endl;
    std::cout << "Piece at a1: " << get_piece_at(pos, 0) << std::endl;
    std::cout << "Piece at e1: " << get_piece_at(pos, 4) << std::endl;
    
    uint64_t orig_wking = pos.WhiteKing;
    std::cout << "Before: WhiteKing = 0x" << std::hex << orig_wking << std::dec << std::endl;
    
    makemove(pos, m);
    std::cout << "After make: WhiteKing = 0x" << std::hex << pos.WhiteKing << std::dec << std::endl;
    std::cout << "After make: whiteToMove = " << pos.whiteToMove << std::endl;
    
    int wking_sq = __builtin_ctzll(pos.WhiteKing);
    bool in_check = is_square_attacked(pos, wking_sq, false);
    std::cout << "White king in check after makemove: " << (in_check ? "YES" : "NO") << std::endl;
    
    undomove(pos, m);
    std::cout << "After unmake: WhiteKing = 0x" << std::hex << pos.WhiteKing << std::dec << std::endl;
    std::cout << "After unmake: whiteToMove = " << pos.whiteToMove << std::endl;
    
    wking_sq = __builtin_ctzll(pos.WhiteKing);
    in_check = is_square_attacked(pos, wking_sq, !pos.whiteToMove);
    std::cout << "White king in check after undomove: " << (in_check ? "YES" : "NO") << std::endl;
    
    if (pos.WhiteKing != orig_wking) {
        std::cout << "ERROR: WhiteKing not restored!" << std::endl;
        return 1;
    }
    
    std::cout << "SUCCESS: Position restored correctly" << std::endl;
    return 0;
}
