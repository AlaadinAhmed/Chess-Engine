#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "utils.hpp"
#include "search.hpp"
#include "globals.hpp"
#include <iostream>

int main() {
    init_magics();
    init_zobrist();
    
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    Move best_move;
    int score = search(pos, 2, -1, best_move);
    
    std::cout << "\nSearch completed" << std::endl;
    std::cout << "Best move: " << move_to_uci(best_move) << std::endl;
    std::cout << "Score: " << score << std::endl;
    std::cout << "Nodes: " << nodes_searched << std::endl;
    
    return 0;
}
