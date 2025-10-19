#include "gtest/gtest.h"
#include "movegen.hpp"
#include "fen.hpp"
#include "globals.hpp"
#include "utils.hpp"

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
    if (move_list.count != 48) {
        printf("Kiwipete generated %d moves (expected 48). Listing moves:\n", move_list.count);
        for (int i = 0; i < move_list.count; ++i) {
            std::string u = move_to_uci(move_list.moves[i]);
            printf("%s ", u.c_str());
        }
        printf("\n");
    }
    ASSERT_EQ(move_list.count, 48);
}