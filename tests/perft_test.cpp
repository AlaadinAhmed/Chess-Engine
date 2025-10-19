#include "gtest/gtest.h"
#include "movegen.hpp"
#include "fen.hpp"
#include "utils.hpp"

// Perft test - counts all legal moves to a given depth
long long perft(Position &pos, int depth) {
    if (depth == 0) return 1;
    
    MoveList moves;
    generate_moves(pos, moves);
    
    long long nodes = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        nodes += perft(pos, depth - 1);
        undomove(pos, moves.moves[i]);
    }
    return nodes;
}

TEST(PerftTest, StartingPosition) {
    Position pos;
    parseFEN(pos, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    EXPECT_EQ(perft(pos, 1), 20);
    EXPECT_EQ(perft(pos, 2), 400);
    EXPECT_EQ(perft(pos, 3), 8902);
}

TEST(PerftTest, Kiwipete) {
    Position pos;
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    EXPECT_EQ(perft(pos, 1), 48);
    EXPECT_EQ(perft(pos, 2), 2039);
}

TEST(PerftTest, Position3) {
    Position pos;
    parseFEN(pos, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    
    EXPECT_EQ(perft(pos, 1), 14);
    EXPECT_EQ(perft(pos, 2), 191);
    EXPECT_EQ(perft(pos, 3), 2812);
}
