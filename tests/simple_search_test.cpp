#include <iostream>
#include "movegen.hpp"
#include "fen.hpp"
#include "search.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include "hash.hpp"

int main() {
    zkey.initKeys();
    initKingAttacks();
    initPawnAttacks();
    initPawnMoves();
    debug_mode = false;
    Position pos;
    pos.setStartingPosition();
    
    std::cout << "Testing search with depth 5..." << std::endl;
    
    Move best_move;
    int score = search(pos, 10, 10000, best_move);
    
    std::cout << "Search returned: " << move_to_uci(best_move) << " with score " << score << std::endl;
    std::cout << "Move details: from=" << best_move.from << " to=" << best_move.to << std::endl;
    
    // Verify the move is legal
    MoveList legal_moves;
    generate_moves(pos, legal_moves);
    
    bool is_legal = false;
    for (int i = 0; i < legal_moves.count; ++i) {
        if (legal_moves.moves[i].from == best_move.from && 
            legal_moves.moves[i].to == best_move.to &&
            legal_moves.moves[i].promotion == best_move.promotion) {
            is_legal = true;
            break;
        }
    }
    
    std::cout << "Move is legal: " << (is_legal ? "YES" : "NO") << std::endl;
    
    return 0;
}
