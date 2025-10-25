#include "position.hpp"
#include "eval.hpp"
#include "utils.hpp"
#include "fen.hpp"
#include "magics.hpp"
#include "movegen.hpp"
#include <iostream>

int main() {
    init_magics();
    initKingAttacks();
    
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    int score = evaluate(pos);
    
    std::cout << "Position: Kiwipete" << std::endl;
    std::cout << "Side to move: " << (pos.whiteToMove ? "White" : "Black") << std::endl;
    std::cout << "Eval score (from side-to-move perspective): " << score << " cp" << std::endl;
    std::cout << "Eval score (from white's perspective): " << (pos.whiteToMove ? score : -score) << " cp" << std::endl;
    
    return 0;
}
