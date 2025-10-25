#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "utils.hpp"
#include <iostream>

int main() {
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    MoveList moves;
    generate_moves(pos, moves);
    
    std::cout << "Generated " << moves.count << " pseudo-legal moves" << std::endl;
    
    int legal_count = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        
        // Check if move leaves king in check
        bool mover_is_white = !pos.whiteToMove; // After making move, side flipped
        uint64_t king_bb = mover_is_white ? pos.WhiteKing : pos.BlackKing;
        if (king_bb == 0) {
            std::cout << "ERROR: No king after move " << move_to_uci(moves.moves[i]) << std::endl;
            undomove(pos, moves.moves[i]);
            continue;
        }
        int king_sq = __builtin_ctzll(king_bb);
        if (is_square_attacked(pos, king_sq, !mover_is_white)) {
            undomove(pos, moves.moves[i]);
            continue;
        }
        
        legal_count++;
        if (legal_count <= 10) {
            std::cout << "  Legal move " << legal_count << ": " << move_to_uci(moves.moves[i]) << std::endl;
        }
        undomove(pos, moves.moves[i]);
    }
    
    std::cout << "Total legal moves: " << legal_count << std::endl;
    
    return 0;
}
