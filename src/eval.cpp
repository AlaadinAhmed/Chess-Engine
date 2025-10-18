#include "eval.hpp"

const int pawn_pst[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int knight_pst[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

const int bishop_pst[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

const int rook_pst[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      5, 10, 10, 10, 10, 10, 10,  5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
      0,  0,  0,  5,  5,  0,  0,  0
};

const int king_pst[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

int evaluate(Position &pos) {
    int score = 0;

    uint64_t bb = pos.WhitePawns;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score += 100 + pawn_pst[sq];
        bb &= bb - 1;
    }
    bb = pos.WhiteKnights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score += 300 + knight_pst[sq];
        bb &= bb - 1;
    }
    bb = pos.WhiteBishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score += 320 + bishop_pst[sq];
        bb &= bb - 1;
    }
    bb = pos.WhiteRooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score += 500 + rook_pst[sq];
        bb &= bb - 1;
    }
    bb = pos.WhiteQueen;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score += 900;
        bb &= bb - 1;
    }
    bb = pos.WhiteKing;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score += king_pst[sq];
        bb &= bb - 1;
    }

    bb = pos.BlackPawns;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score -= 100 + pawn_pst[63 - sq];
        bb &= bb - 1;
    }
    bb = pos.BlackKnights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score -= 300 + knight_pst[63 - sq];
        bb &= bb - 1;
    }
    bb = pos.BlackBishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score -= 320 + bishop_pst[63 - sq];
        bb &= bb - 1;
    }
    bb = pos.BlackRooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score -= 500 + rook_pst[63 - sq];
        bb &= bb - 1;
    }
    bb = pos.BlackQueen;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score -= 900;
        bb &= bb - 1;
    }
    bb = pos.BlackKing;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        score -= king_pst[63 - sq];
        bb &= bb - 1;
    }

    return pos.whiteToMove ? score : -score;
}