#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include <iostream>

int main() {
    Position pos;
    parse_fen(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    MoveList moves;
    generate_moves(pos, moves);
    
    std::cout << "Generated " << moves.count << " moves:" << std::endl;
    for (int i = 0; i < moves.count && i < 10; i++) {
        std::cout << "  Move " << i << ": from=" << moves.moves[i].from 
                  << " to=" << moves.moves[i].to << std::endl;
    }
    
    return 0;
}
