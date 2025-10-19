#include <iostream>
#include "movegen.hpp"
#include "fen.hpp"
#include "utils.hpp"

int main() {
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    MoveList moves;
    generate_moves(pos, moves);
    
    bool found_a3b4 = false;
    for (int i = 0; i < moves.count; ++i) {
        if (moves.moves[i].from == 16 && moves.moves[i].to == 25) {
            found_a3b4 = true;
            break;
        }
    }
    
    if (found_a3b4) {
        std::cout << "a3b4 is a legal move" << std::endl;
    } else {
        std::cout << "a3b4 is an illegal move" << std::endl;
    }
    
    return 0;
}
