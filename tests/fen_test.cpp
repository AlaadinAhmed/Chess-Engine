#include "gtest/gtest.h"
#include "fen.hpp"

TEST(FenTest, StartingPosition) {
    Position board;
    parseFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(board.whiteToMove, true);
    ASSERT_EQ(board.castelingRights, 15);
    ASSERT_EQ(board.enPassant, 0);
    ASSERT_EQ(board.move50rule, 0);
    ASSERT_EQ(board.move, 1);
}

TEST(FenTest, Kiwipete) {
    Position board;
    parseFEN(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    ASSERT_EQ(board.whiteToMove, true);
    ASSERT_EQ(board.castelingRights, 15);
    ASSERT_EQ(board.enPassant, 0);
    ASSERT_EQ(board.move50rule, 0);
    ASSERT_EQ(board.move, 1);
}
