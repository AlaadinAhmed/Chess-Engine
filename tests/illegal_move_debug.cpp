#include "gtest/gtest.h"
#include "movegen.hpp"
#include "fen.hpp"
#include "search.hpp"
#include "utils.hpp"

TEST(IllegalMoveDebug, KiwipetePosition) {
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    // Generate all legal moves
    MoveList legal_moves;
    generate_moves(pos, legal_moves);
    
    std::cout << "Legal moves in Kiwipete position: ";
    for (int i = 0; i < legal_moves.count; ++i) {
        std::cout << move_to_uci(legal_moves.moves[i]) << " ";
    }
    std::cout << std::endl;
    
    // Check if a3b4 is in the legal moves
    bool a3b4_found = false;
    for (int i = 0; i < legal_moves.count; ++i) {
        if (legal_moves.moves[i].from == 16 && legal_moves.moves[i].to == 25) { // a3 to b4
            a3b4_found = true;
            break;
        }
    }
    
    EXPECT_FALSE(a3b4_found) << "a3b4 should not be a legal move (no piece on a3)";
    
    // Test search
    Move best_move;
    std::cout << "Best move before search: from=" << best_move.from << " to=" << best_move.to << std::endl;
    int score = search(pos, 5, 1000, best_move);
    
    std::cout << "Search returned: " << move_to_uci(best_move) << " with score " << score << std::endl;
    std::cout << "Best move after search: from=" << best_move.from << " to=" << best_move.to << std::endl;
    
    // Verify the best move is legal
    bool best_move_legal = false;
    for (int i = 0; i < legal_moves.count; ++i) {
        if (legal_moves.moves[i].from == best_move.from && 
            legal_moves.moves[i].to == best_move.to &&
            legal_moves.moves[i].promotion == best_move.promotion) {
            best_move_legal = true;
            break;
        }
    }
    
    EXPECT_TRUE(best_move_legal) << "Search returned illegal move: " << move_to_uci(best_move);
}
