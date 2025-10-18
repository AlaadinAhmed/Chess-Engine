#include "gtest/gtest.h"
#include "fen.hpp"
#include "position.hpp"

TEST(FenTest, StartingPositionParseFEN) {
    Position board;
    parseFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(board.whiteToMove, true);
    ASSERT_EQ(board.castelingRights, 15);
    ASSERT_EQ(board.enPassant, 0);
    ASSERT_EQ(board.move50rule, 0);
    ASSERT_EQ(board.move, 1);
    ASSERT_EQ(board.WhitePawns, 0x000000000000FF00ULL);
    ASSERT_EQ(board.BlackPawns, 0x00FF000000000000ULL);
    ASSERT_EQ(board.WhiteRooks, 0x0000000000000081ULL);
    ASSERT_EQ(board.BlackRooks, 0x8100000000000000ULL);
    ASSERT_EQ(board.WhiteKnights, 0x0000000000000042ULL);
    ASSERT_EQ(board.BlackKnights, 0x4200000000000000ULL);
    ASSERT_EQ(board.WhiteBishops, 0x0000000000000024ULL);
    ASSERT_EQ(board.BlackBishops, 0x2400000000000000ULL);
    ASSERT_EQ(board.WhiteQueen, 0x0000000000000008ULL);
    ASSERT_EQ(board.BlackQueen, 0x0800000000000000ULL);
    ASSERT_EQ(board.WhiteKing, 0x0000000000000010ULL);
    ASSERT_EQ(board.BlackKing, 0x1000000000000000ULL);
}

TEST(FenTest, KiwipeteParseFEN) {
    Position board;
    parseFEN(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    ASSERT_EQ(board.whiteToMove, true);
    ASSERT_EQ(board.castelingRights, 15);
    ASSERT_EQ(board.enPassant, 0);
    ASSERT_EQ(board.move50rule, 0);
    ASSERT_EQ(board.move, 1);
    ASSERT_EQ(board.WhitePawns, 0x000000081000E700ULL);
    ASSERT_EQ(board.BlackPawns, 0x002D500002800000ULL);
    ASSERT_EQ(board.WhiteRooks, 0x0000000000000081ULL);
    ASSERT_EQ(board.BlackRooks, 0x8100000000000000ULL);
    ASSERT_EQ(board.WhiteKnights, 0x000001000040000ULL);
    ASSERT_EQ(board.BlackKnights, 0x220000000000ULL);
    ASSERT_EQ(board.WhiteBishops, 0x0000000000001800ULL);
    ASSERT_EQ(board.BlackBishops, 0x0040010000000000ULL);
    ASSERT_EQ(board.WhiteQueen, 0x0000000000200000ULL);
    ASSERT_EQ(board.BlackQueen, 0x0010000000000000ULL);
    ASSERT_EQ(board.WhiteKing, 0x0000000000000010ULL);
    ASSERT_EQ(board.BlackKing, 0x1000000000000000ULL);
}

TEST(PositionTest, SetStartingPosition) {
    Position board;
    board.setStartingPosition();
    // Assertions for starting position
    ASSERT_EQ(board.WhitePawns, 0x000000000000FF00ULL);
    ASSERT_EQ(board.BlackPawns, 0x00FF000000000000ULL);
    ASSERT_EQ(board.WhiteRooks, 0x0000000000000081ULL);
    ASSERT_EQ(board.BlackRooks, 0x8100000000000000ULL);
    ASSERT_EQ(board.WhiteKnights, 0x0000000000000042ULL);
    ASSERT_EQ(board.BlackKnights, 0x4200000000000000ULL);
    ASSERT_EQ(board.WhiteBishops, 0x0000000000000024ULL);
    ASSERT_EQ(board.BlackBishops, 0x2400000000000000ULL);
    ASSERT_EQ(board.WhiteQueen, 0x0000000000000008ULL);
    ASSERT_EQ(board.BlackQueen, 0x0800000000000000ULL);
    ASSERT_EQ(board.WhiteKing, 0x0000000000000010ULL);
    ASSERT_EQ(board.BlackKing, 0x1000000000000000ULL);
    ASSERT_EQ(board.whiteToMove, true);
    ASSERT_EQ(board.castelingRights, 15);
    ASSERT_EQ(board.enPassant, 0);
    ASSERT_EQ(board.move50rule, 0);
    ASSERT_EQ(board.move, 1);
}

TEST(PositionTest, SetFen) {
    Position board;
    board.setFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    // Assertions for Kiwipete FEN
    ASSERT_EQ(board.whiteToMove, true);
    ASSERT_EQ(board.castelingRights, 15);
    ASSERT_EQ(board.enPassant, 0);
    ASSERT_EQ(board.move50rule, 0);
    ASSERT_EQ(board.move, 1);
    ASSERT_EQ(board.WhitePawns, 0x000000081000E700ULL);
    ASSERT_EQ(board.BlackPawns, 0x002D500002800000ULL);
    ASSERT_EQ(board.WhiteRooks, 0x0000000000000081ULL);
    ASSERT_EQ(board.BlackRooks, 0x8100000000000000ULL);
    ASSERT_EQ(board.WhiteKnights, 0x000001000040000ULL);
    ASSERT_EQ(board.BlackKnights, 0x220000000000ULL);
    ASSERT_EQ(board.WhiteBishops, 0x0000000000001800ULL);
    ASSERT_EQ(board.BlackBishops, 0x0040010000000000ULL);
    ASSERT_EQ(board.WhiteQueen, 0x0000000000200000ULL);
    ASSERT_EQ(board.BlackQueen, 0x0010000000000000ULL);
    ASSERT_EQ(board.WhiteKing, 0x0000000000000010ULL);
    ASSERT_EQ(board.BlackKing, 0x1000000000000000ULL);
}
