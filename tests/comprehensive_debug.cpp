#include <iostream>
#include "movegen.hpp"
#include "fen.hpp"
#include "search.hpp"
#include "utils.hpp"

int main() {
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Testing move generation and validation..." << std::endl;
    
    // Generate all legal moves
    MoveList legal_moves;
    generate_moves(pos, legal_moves);
    
    std::cout << "Legal moves (" << legal_moves.count << "): ";
    for (int i = 0; i < legal_moves.count; ++i) {
        std::cout << move_to_uci(legal_moves.moves[i]) << " ";
    }
    std::cout << std::endl;
    
    // Check if a3b4 is in the legal moves
    bool found_a3b4 = false;
    for (int i = 0; i < legal_moves.count; ++i) {
        if (legal_moves.moves[i].from == 16 && legal_moves.moves[i].to == 25) {
            found_a3b4 = true;
            std::cout << "ERROR: a3b4 found in legal moves!" << std::endl;
            break;
        }
    }
    
    if (!found_a3b4) {
        std::cout << "a3b4 is NOT in legal moves - this is correct" << std::endl;
    }
    
    // Test search with depth 5
    std::cout << "\nTesting search with depth 5..." << std::endl;
    Move best_move;
    int score = search(pos, 5, 1000, best_move);
    
    std::cout << "Search returned: " << move_to_uci(best_move) << " with score " << score << std::endl;
    
    // Verify the move is legal
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
    
    if (!is_legal) {
        std::cout << "ERROR: Search returned illegal move!" << std::endl;
        std::cout << "Move details: from=" << best_move.from << " to=" << best_move.to << std::endl;
    }
    
    return 0;
}
