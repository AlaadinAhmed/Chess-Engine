#include "gtest/gtest.h"
#include "position.hpp"
#include "movegen.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "bitboard.hpp" // For get_lsb_index
#include "utils.hpp" // For move_to_uci

TEST(FenIllegalMoveTest, CheckIllegalMoves) {
    std::string fen_string = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Position pos;
    pos.setFen(fen_string);

    MoveList move_list;
    generate_moves(pos, move_list);

    int illegal_moves_found = 0;

    for (int i = 0; i < move_list.count; ++i) {
        Move move = move_list.moves[i];
        
        // Store current state to restore later
        Position temp_pos = pos;

        makemove(pos, move);

        int king_square;
        if (temp_pos.whiteToMove) {
            king_square = get_lsb_index(pos.WhiteKing);
        } else {
            king_square = get_lsb_index(pos.BlackKing);
        }

        if (is_square_attacked(pos, king_square, !temp_pos.whiteToMove)) {
            std::cout << "Illegal move found in FEN: " << fen_string << std::endl;
            std::cout << "Move: " << move.from << " to " << move.to << std::endl;
            illegal_moves_found++;
        }
        
        pos = temp_pos; // Restore the position
    }

    ASSERT_EQ(illegal_moves_found, 0) << "Found " << illegal_moves_found << " illegal moves.";
}
