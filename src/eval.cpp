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
const int DOUBLED_PAWN_PENALTY = 10;
const int PASSED_PAWN_BONUS = 20;
const int BISHOP_PAIR_BONUS = 30;

// File masks for pawn structure evaluation

// File masks for pawn structure evaluation
const uint64_t file_masks[8] = {
    0x0101010101010101ULL, 0x0202020202020202ULL, 0x0404040404040404ULL, 0x0808080808080808ULL,
    0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL
};

const uint64_t white_passed_pawn_masks[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0x0101010101010100ULL, 0x0202020202020200ULL, 0x0404040404040400ULL, 0x0808080808080800ULL,
    0x1010101010101000ULL, 0x2020202020202000ULL, 0x4040404040404000ULL, 0x8080808080808000ULL,
    0x0101010101010000ULL, 0x0202020202020000ULL, 0x0404040404040000ULL, 0x0808080808080000ULL,
    0x1010101010100000ULL, 0x2020202020200000ULL, 0x4040404040400000ULL, 0x8080808080800000ULL,
    0x0101010101000000ULL, 0x0202020202000000ULL, 0x0404040404000000ULL, 0x0808080808000000ULL,
    0x1010101010000000ULL, 0x2020202020000000ULL, 0x4040404040000000ULL, 0x8080808080000000ULL,
    0x0101010100000000ULL, 0x0202020200000000ULL, 0x0404040400000000ULL, 0x0808080800000000ULL,
    0x1010101000000000ULL, 0x2020202000000000ULL, 0x4040404000000000ULL, 0x8080808000000000ULL,
    0x0101010000000000ULL, 0x0202020000000000ULL, 0x0404040000000000ULL, 0x0808080000000000ULL,
    0x1010100000000000ULL, 0x2020200000000000ULL, 0x4040400000000000ULL, 0x8080800000000000ULL,
    0x0101000000000000ULL, 0x0202000000000000ULL, 0x0404000000000000ULL, 0x0808000000000000ULL,
    0x1010000000000000ULL, 0x2020000000000000ULL, 0x4040000000000000ULL, 0x8080000000000000ULL,
};

const uint64_t black_passed_pawn_masks[64] = {
    0x0001010101010101ULL, 0x0002020202020202ULL, 0x0004040404040404ULL, 0x0008080808080808ULL,
    0x0010101010101010ULL, 0x0020202020202020ULL, 0x0040404040404040ULL, 0x0080808080808080ULL,
    0x00010101010101ULL, 0x00020202020202ULL, 0x00040404040404ULL, 0x00080808080808ULL,
    0x00101010101010ULL, 0x00202020202020ULL, 0x00404040404040ULL, 0x00808080808080ULL,
    0x000101010101ULL, 0x000202020202ULL, 0x000404040404ULL, 0x000808080808ULL,
    0x001010101010ULL, 0x002020202020ULL, 0x004040404040ULL, 0x008080808080ULL,
    0x0001010101ULL, 0x0002020202ULL, 0x0004040404ULL, 0x0008080808ULL,
    0x0010101010ULL, 0x0020202020ULL, 0x0040404040ULL, 0x0080808080ULL,
    0x00010101ULL, 0x00020202ULL, 0x00040404ULL, 0x00080808ULL,
    0x00101010ULL, 0x00202020ULL, 0x00404040ULL, 0x00808080ULL,
    0x000101ULL, 0x000202ULL, 0x000404ULL, 0x000808ULL,
    0x001010ULL, 0x002020ULL, 0x004040ULL, 0x008080ULL,
    0x0001ULL, 0x0002ULL, 0x0004ULL, 0x0008ULL,
    0x0010ULL, 0x0020ULL, 0x0040ULL, 0x0080ULL,
    0, 0, 0, 0, 0, 0, 0, 0
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

int countDoubledPawns(const Position &pos, bool is_white) {
    int doubled_pawns = 0;
    uint64_t pawns = is_white ? pos.WhitePawns : pos.BlackPawns;

    for (int file = 0; file < 8; ++file) {
        if (__builtin_popcountll(pawns & file_masks[file]) > 1) {
            doubled_pawns++;
        }
    }
    return doubled_pawns;
}

int countPassedPawns(const Position &pos, bool is_white) {
    int passed_pawns = 0;
    uint64_t friendly_pawns = is_white ? pos.WhitePawns : pos.BlackPawns;
    uint64_t enemy_pawns = is_white ? pos.BlackPawns : pos.WhitePawns;

    uint64_t pawns = friendly_pawns;
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        if (is_white) {
            if (! (enemy_pawns & white_passed_pawn_masks[sq])) {
                passed_pawns++;
            }
        } else {
            if (! (enemy_pawns & black_passed_pawn_masks[sq])) {
                passed_pawns++;
            }
        }
        pawns &= pawns - 1;
    }
    return passed_pawns;
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

    score -= countIsolatedPawns(pos, true) * ISOLATED_PAWN_PENALTY;
    score += countIsolatedPawns(pos, false) * ISOLATED_PAWN_PENALTY;

    score -= countDoubledPawns(pos, true) * DOUBLED_PAWN_PENALTY;
    score += countDoubledPawns(pos, false) * DOUBLED_PAWN_PENALTY;

    score += countPassedPawns(pos, true) * PASSED_PAWN_BONUS;
    score -= countPassedPawns(pos, false) * PASSED_PAWN_BONUS;

    if (__builtin_popcountll(pos.WhiteBishops) >= 2) {
        score += BISHOP_PAIR_BONUS;
    }
    if (__builtin_popcountll(pos.BlackBishops) >= 2) {
        score -= BISHOP_PAIR_BONUS;
    }

    return pos.whiteToMove ? score : -score;
}