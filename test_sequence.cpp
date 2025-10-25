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
    
    std::cout << "Starting position (after a2a3):" << std::endl;
    std::cout << "  BlackKing: 0x" << std::hex << pos.BlackKing << std::dec << std::endl;
    
    // Try a7a5
    Move m1 = {.from=48, .to=32, .promotion=NO_PIECE};
    makemove(pos, m1);
    std::cout << "After a7a5: BlackKing=0x" << std::hex << pos.BlackKing << std::dec << std::endl;
    undomove(pos, m1);
    std::cout << "After unmake: BlackKing=0x" << std::hex << pos.BlackKing << std::dec << std::endl;
    
    // Try a7a6
    Move m2 = {.from=48, .to=40, .promotion=NO_PIECE};
    makemove(pos, m2);
    std::cout << "After a7a6: BlackKing=0x" << std::hex << pos.BlackKing << std::dec << std::endl;
    undomove(pos, m2);
    std::cout << "After unmake: BlackKing=0x" << std::hex << pos.BlackKing << std::dec << std::endl;
    
    // Try b7b5
    Move m3 = {.from=49, .to=33, .promotion=NO_PIECE};
    makemove(pos, m3);
    int king_sq = __builtin_ctzll(pos.BlackKing);
    bool in_check = is_square_attacked(pos, king_sq, true);
    std::cout << "After b7b5: BlackKing=0x" << std::hex << pos.BlackKing << std::dec 
              << " in_check=" << in_check << std::endl;
    
    return 0;
}
