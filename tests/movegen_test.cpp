#include "gtest/gtest.h"
#include "movegen.hpp"
#include "fen.hpp"
#include "globals.hpp"

TEST(MovegenTest, StartingPosition) {
    Position board;
    parseFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    MoveList move_list;
    generate_moves(board, move_list);
    ASSERT_EQ(move_list.count, 20);
}

TEST(MovegenTest, Kiwipete) {
    Position board;
    parseFEN(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    MoveList move_list;
    generate_moves(board, move_list);
    if (debug_mode) { printf("DEBUG: Final move_list.count: %d\n", move_list.count); }
    ASSERT_EQ(move_list.count, 48);
}