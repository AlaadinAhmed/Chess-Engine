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

const int king_pst_endgame[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -50,-40,-30,-20,-20,-30,-40,-50
};

const int ISOLATED_PAWN_PENALTY = 15;

// File masks for pawn structure evaluation

// File masks for pawn structure evaluation
const uint64_t file_masks[8] = {
    0x0101010101010101ULL, 0x0202020202020202ULL, 0x0404040404040404ULL, 0x0808080808080808ULL,
    0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL
};

int countIsolatedPawns(const Position &pos, bool is_white) {
    int isolated_pawns = 0;
    uint64_t pawns = is_white ? pos.WhitePawns : pos.BlackPawns;
    uint64_t friendly_pawns = pawns;

    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        int file = sq % 8;

        uint64_t adjacent_files = 0ULL;
        if (file > 0) adjacent_files |= file_masks[file - 1];
        if (file < 7) adjacent_files |= file_masks[file + 1];

        if (!((friendly_pawns & adjacent_files))) {
            isolated_pawns++;
        }
        pawns &= pawns - 1;
    }
    return isolated_pawns;
}

int calculateGamePhase(const Position &pos) {
    int total_material = 0;
    total_material += __builtin_popcountll(pos.WhiteKnights) * 3;
    total_material += __builtin_popcountll(pos.BlackKnights) * 3;
    total_material += __builtin_popcountll(pos.WhiteBishops) * 3;
    total_material += __builtin_popcountll(pos.BlackBishops) * 3;
    total_material += __builtin_popcountll(pos.WhiteRooks) * 5;
    total_material += __builtin_popcountll(pos.BlackRooks) * 5;
    total_material += __builtin_popcountll(pos.WhiteQueen) * 9;
    total_material += __builtin_popcountll(pos.BlackQueen) * 9;

    if (total_material > 20) return 0; // Opening/Middlegame
    if (total_material > 5) return 1;  // Endgame
    return 2; // Heavy Endgame
}

int evaluate(Position &pos) {
    int score = 0;
    int game_phase = calculateGamePhase(pos);

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
        if (game_phase == 0) { // Opening/Middlegame
            score += king_pst[sq];
        } else { // Endgame
            score += king_pst_endgame[sq];
        }
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
        if (game_phase == 0) { // Opening/Middlegame
            score -= king_pst[63 - sq];
        } else { // Endgame
            score -= king_pst_endgame[63 - sq];
        }
        bb &= bb - 1;
    }

    score += countIsolatedPawns(pos, true) * ISOLATED_PAWN_PENALTY;
    score -= countIsolatedPawns(pos, false) * ISOLATED_PAWN_PENALTY;

    return pos.whiteToMove ? score : -score;
}