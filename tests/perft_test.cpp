#include "gtest/gtest.h"
#include "movegen.hpp"
#include "fen.hpp"
#include "utils.hpp"
#include "hash.hpp"

struct Initializer {
    Initializer() {
        initKingAttacks();
        initPawnAttacks();
        initPawnMoves();
        zkey.initKeys();
    }
};
Initializer init;

// Perft test - counts all legal moves to a given depth
long long perft(Position &pos, int depth) {
    if (depth == 0) return 1;
    
    MoveList moves;
    generate_moves(pos, moves);

    long long nodes = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        // Check legality
        int king_sq = pos.whiteToMove ? __builtin_ctzll(pos.BlackKing) : __builtin_ctzll(pos.WhiteKing);
        if (!is_square_attacked(pos, king_sq, pos.whiteToMove)) {
            nodes += perft(pos, depth - 1);
        }
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
    
    std::cout << "Kiwipete Depth 2 Divide:" << std::endl;
    MoveList moves;
    generate_moves(pos, moves);
    long long total_d2 = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        int king_sq = pos.whiteToMove ? __builtin_ctzll(pos.BlackKing) : __builtin_ctzll(pos.WhiteKing);
        if (!is_square_attacked(pos, king_sq, pos.whiteToMove)) {
            long long c = perft(pos, 1);
            total_d2 += c;
            std::cout << move_to_uci(moves.moves[i]) << ": " << c << std::endl;
        }
        undomove(pos, moves.moves[i]);
    }
    EXPECT_EQ(total_d2, 2039);

    std::cout << "Kiwipete Depth 3 Divide:" << std::endl;
    // Reuse moves list
    generate_moves(pos, moves);
    long long total = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        int king_sq = pos.whiteToMove ? __builtin_ctzll(pos.BlackKing) : __builtin_ctzll(pos.WhiteKing);
        if (!is_square_attacked(pos, king_sq, pos.whiteToMove)) {
            long long c = perft(pos, 2);
            total += c;
            std::cout << move_to_uci(moves.moves[i]) << ": " << c << std::endl;
        }
        undomove(pos, moves.moves[i]);
    }
    EXPECT_EQ(total, 97862);
}

TEST(PerftTest, Position3) {
    Position pos;
    parseFEN(pos, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    
    EXPECT_EQ(perft(pos, 1), 14);
    EXPECT_EQ(perft(pos, 2), 191);
    
    // Detailed breakdown for depth 3
    MoveList moves;
    generate_moves(pos, moves);
    long long total = 0;
    std::cout << "Position3 Depth 3 Breakdown:" << std::endl;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        int king_sq = pos.whiteToMove ? __builtin_ctzll(pos.BlackKing) : __builtin_ctzll(pos.WhiteKing);
        if (!is_square_attacked(pos, king_sq, pos.whiteToMove)) {
            long long c = perft(pos, 2);
            total += c;
            std::cout << move_to_uci(moves.moves[i]) << ": " << c << std::endl;
        }
        undomove(pos, moves.moves[i]);
    }
    EXPECT_EQ(total, 2812);
}

TEST(PerftTest, Position3_b4f4) {
    Position pos;
    // FEN after b4f4: 8/2p5/3p4/KP5r/5R1k/8/4P1P1/8 b - - 0 1
    parseFEN(pos, "8/2p5/3p4/KP5r/5R1k/8/4P1P1/8 b - - 0 1");
    
    // Depth 2 from here corresponds to Depth 3 from root (for this branch)
    // Actually, b4f4 is depth 1 move.
    // So we need perft(pos, 2).
    // My engine got 41.
    std::cout << "Testing b4f4 branch..." << std::endl;
    MoveList moves;
    generate_moves(pos, moves);
    long long total = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        int king_sq = pos.whiteToMove ? __builtin_ctzll(pos.BlackKing) : __builtin_ctzll(pos.WhiteKing);
        if (!is_square_attacked(pos, king_sq, pos.whiteToMove)) {
            long long c = perft(pos, 1);
            total += c;
            std::cout << move_to_uci(moves.moves[i]) << ": " << c << std::endl;
        }
        undomove(pos, moves.moves[i]);
    }
    std::cout << "b4f4 nodes: " << total << std::endl;
}

TEST(PerftTest, Position3_e2e4) {
    Position pos;
    // FEN after e2e4: 8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - e3 0 1
    parseFEN(pos, "8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - e3 0 1");
    
    std::cout << "Testing e2e4 branch..." << std::endl;
    long long nodes = perft(pos, 2);
    std::cout << "e2e4 nodes: " << nodes << std::endl;
}

TEST(PerftTest, Kiwipete_a2a4) {
    Position pos;
    // FEN after a2a4: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    
    std::cout << "Testing Kiwipete a2a4 branch..." << std::endl;
    MoveList moves;
    generate_moves(pos, moves);
    std::cout << "Generated " << moves.count << " moves:" << std::endl;
    for (int i = 0; i < moves.count; i++) {
        std::cout << move_to_uci(moves.moves[i]) << " ";
    }
    std::cout << std::endl;
    
    long long nodes = perft(pos, 1);
    std::cout << "a2a4 nodes: " << nodes << std::endl;
}

TEST(PerftTest, Kiwipete_f3f6) {
    Position pos;
    // FEN after f3f6: r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/7p/PPPBBPPP/R3K2R b KQkq - 0 1
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/7p/PPPBBPPP/R3K2R b KQkq - 0 1");
    
    std::cout << "Testing Kiwipete f3f6 branch..." << std::endl;
    MoveList moves;
    generate_moves(pos, moves);
    long long total = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        int king_sq = pos.whiteToMove ? __builtin_ctzll(pos.BlackKing) : __builtin_ctzll(pos.WhiteKing);
        if (!is_square_attacked(pos, king_sq, pos.whiteToMove)) {
            long long c = perft(pos, 2);
            total += c;
            std::cout << move_to_uci(moves.moves[i]) << ": " << c << std::endl;
        }
        undomove(pos, moves.moves[i]);
    }
    std::cout << "f3f6 nodes: " << total << std::endl;
}

TEST(PerftTest, Kiwipete_b4a3) {
    Position pos;
    // FEN after b4a3 (ep): r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/ppN2Q1p/1PPBBPPP/R3K2R w KQkq - 0 1
    parseFEN(pos, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/ppN2Q1p/1PPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Testing Kiwipete b4a3 branch..." << std::endl;
    long long nodes = perft(pos, 1);
    std::cout << "b4a3 nodes: " << nodes << std::endl;
}

TEST(PerftTest, Position3_g2g4) {
    Position pos;
    // FEN after g2g4: 8/2p5/3p4/KP5r/1R3pPk/8/4P3/8 b - g3 0 1
    parseFEN(pos, "8/2p5/3p4/KP5r/1R3pPk/8/4P3/8 b - g3 0 1");
    
    std::cout << "Testing Position3 g2g4 branch..." << std::endl;
    MoveList moves;
    generate_moves(pos, moves);
    long long total = 0;
    for (int i = 0; i < moves.count; i++) {
        makemove(pos, moves.moves[i]);
        int king_sq = pos.whiteToMove ? __builtin_ctzll(pos.BlackKing) : __builtin_ctzll(pos.WhiteKing);
        bool legal = !is_square_attacked(pos, king_sq, pos.whiteToMove);
        if (legal) {
            long long c = perft(pos, 1);
            total += c;
            // std::cout << move_to_uci(moves.moves[i]) << ": " << c << std::endl;
        }
        undomove(pos, moves.moves[i]);
    }
    std::cout << "g2g4 nodes: " << total << std::endl;
}


