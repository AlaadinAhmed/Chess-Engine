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
    
    MoveList moves;
    generate_moves(pos, moves);
    
    std::cout << "Generated " << moves.count << " moves" << std::endl;
    
    // Check if a1e1 is in the list
    bool found_a1e1 = false;
    for (int i = 0; i < moves.count; i++) {
        if (moves.moves[i].from == 0 && moves.moves[i].to == 4) {
            found_a1e1 = true;
            std::cout << "Found a1e1 at index " << i << std::endl;
            break;
        }
    }
    
    if (!found_a1e1) {
        std::cout << "a1e1 is NOT in the move list (good!)" << std::endl;
    }
    
    // Print first 10 moves
    std::cout << "\nFirst 10 moves:" << std::endl;
    for (int i = 0; i < moves.count && i < 10; i++) {
        std::cout << i << ": " << move_to_uci(moves.moves[i]) << std::endl;
    }
    
    return 0;
}
