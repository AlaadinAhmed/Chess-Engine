#include "eval.hpp"
#include "pst.hpp"
#include "bitboard.hpp"
#include "magics.hpp"
#include "movegen.hpp"
#include "utils.hpp"

const EvalConfig default_eval_config = {
    .isolated_pawn_penalty = 15,
    .doubled_pawn_penalty = 10,
    .passed_pawn_bonus = 20,
    .bishop_pair_bonus = 30,
    .tempo_bonus = 10,
    .rook_open_file_bonus = 15,
    .rook_semi_open_file_bonus = 7,
};

const int pawn_pst_mg[64] = {
    0,  0,  0,  0,   0,   0,  0,  0,  50, 50, 50,  50, 50, 50,  50, 50,
    10, 10, 20, 30,  30,  20, 10, 10, 5,  5,  10,  25, 25, 10,  5,  5,
    0,  0,  0,  20,  20,  0,  0,  0,  5,  -5, -10, 0,  0,  -10, -5, 5,
    5,  10, 10, -20, -20, 10, 10, 5,  0,  0,  0,   0,  0,  0,   0,  0};

const int pawn_pst_eg[64] = {0,  0,  0,  0,  0,  0,  0,  0,  80, 80, 80, 80, 80,
                             80, 80, 80, 50, 50, 50, 50, 50, 50, 50, 50, 30, 30,
                             30, 30, 30, 30, 30, 30, 20, 20, 20, 20, 20, 20, 20,
                             20, 10, 10, 10, 10, 10, 10, 10, 10, 5,  5,  5,  5,
                             5,  5,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0};

const int knight_pst_mg[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,
    0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
    15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
    -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   5,
    5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50,
};

const int knight_pst_eg[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   5,   5,
    0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
    15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
    -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   0,
    0,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50,
};

const int bishop_pst_mg[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,
    0,   0,   -10, -10, 0,   5,   10,  10,  5,   0,   -10, -10, 5,
    5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  0,
    -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 5,   0,   0,
    0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20,
};

const int bishop_pst_eg[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 5,   0,   0,   0,
    0,   5,   -10, -10, 10,  5,   10,  10,  5,   10,  -10, -10, 5,
    5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  0,
    -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 5,   0,   0,
    0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20,
};

const int rook_pst_mg[64] = {0,  0,  0, 0,  0, 0,  0,  0, 5,  10, 10, 10, 10,
                             10, 10, 5, -5, 0, 0,  0,  0, 0,  0,  -5, -5, 0,
                             0,  0,  0, 0,  0, -5, -5, 0, 0,  0,  0,  0,  0,
                             -5, -5, 0, 0,  0, 0,  0,  0, -5, -5, 0,  0,  0,
                             0,  0,  0, -5, 0, 0,  0,  5, 5,  0,  0,  0};

const int rook_pst_eg[64] = {
    0,  0,  0,  5,  5,  0,  0,  0,  -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0,  0,  0,  0,  0,  0,  -5, -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0,  0,  0,  0,  0,  0,  -5, -5, 0, 0, 0, 0, 0, 0, -5,
    5,  10, 10, 10, 10, 10, 10, 5,  0,  0, 0, 0, 0, 0, 0, 0,
};

const int king_pst_mg[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50,
    -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40,
    -40, -50, -50, -40, -40, -30, -20, -30, -30, -40, -40, -30, -30,
    -20, -10, -20, -20, -20, -20, -20, -20, -10, 20,  20,  0,   0,
    0,   0,   20,  20,  20,  30,  10,  0,   0,   10,  30,  20};

const int king_pst_eg[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50, -30, -20, -10, 0,   0,
    -10, -20, -30, -30, -10, 20,  30,  30,  20,  -10, -30, -30, -10,
    30,  40,  40,  30,  -10, -30, -30, -10, 30,  40,  40,  30,  -10,
    -30, -30, -10, 20,  30,  30,  20,  -10, -30, -30, -20, -10, 0,
    0,   -10, -20, -30, -50, -40, -30, -20, -20, -30, -40, -50};

// File masks for pawn structure evaluation
const uint64_t file_masks[8] = {0x0101010101010101ULL, 0x0202020202020202ULL,
                                0x0404040404040404ULL, 0x0808080808080808ULL,
                                0x1010101010101010ULL, 0x2020202020202020ULL,
                                0x4040404040404040ULL, 0x8080808080808080ULL};

const uint64_t white_passed_pawn_masks[64] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x0101010101010100ULL,
    0x0202020202020200ULL,
    0x0404040404040400ULL,
    0x0808080808080800ULL,
    0x1010101010101000ULL,
    0x2020202020202000ULL,
    0x4040404040404000ULL,
    0x8080808080808000ULL,
    0x0101010101010000ULL,
    0x0202020202020000ULL,
    0x0404040404040000ULL,
    0x0808080808080000ULL,
    0x1010101010100000ULL,
    0x2020202020200000ULL,
    0x4040404040400000ULL,
    0x8080808080800000ULL,
    0x0101010101000000ULL,
    0x0202020202000000ULL,
    0x0404040404000000ULL,
    0x0808080808000000ULL,
    0x1010101010000000ULL,
    0x2020202020000000ULL,
    0x4040404040000000ULL,
    0x8080808080000000ULL,
    0x0101010100000000ULL,
    0x0202020200000000ULL,
    0x0404040400000000ULL,
    0x0808080800000000ULL,
    0x1010101000000000ULL,
    2020202000000000ULL,
    0x4040404000000000ULL,
    0x8080808000000000ULL,
    0x0101010000000000ULL,
    0x0202020000000000ULL,
    0x0404040000000000ULL,
    0x0808080000000000ULL,
    0x1010100000000000ULL,
    0x2020200000000000ULL,
    0x4040400000000000ULL,
    0x8080800000000000ULL,
    0x0101000000000000ULL,
    0x0202000000000000ULL,
    0x0404000000000000ULL,
    0x0808000000000000ULL,
    0x1010000000000000ULL,
    0x2020000000000000ULL,
    0x4040000000000000ULL,
    0x8080000000000000ULL,
};

const uint64_t black_passed_pawn_masks[64] = {0x0001010101010101ULL,
                                              0x0002020202020202ULL,
                                              0x0004040404040404ULL,
                                              0x0008080808080808ULL,
                                              0x0010101010101010ULL,
                                              0x0020202020202020ULL,
                                              0x0040404040404040ULL,
                                              0x0080808080808080ULL,
                                              0x00010101010101ULL,
                                              0x00020202020202ULL,
                                              0x00040404040404ULL,
                                              0x00080808080808ULL,
                                              0x00101010101010ULL,
                                              0x00202020202020ULL,
                                              0x00404040404040ULL,
                                              0x00808080808080ULL,
                                              0x000101010101ULL,
                                              0x000202020202ULL,
                                              0x000404040404ULL,
                                              0x000808080808ULL,
                                              0x001010101010ULL,
                                              0x002020202020ULL,
                                              0x004040404040ULL,
                                              0x008080808080ULL,
                                              0x0001010101ULL,
                                              0x0002020202ULL,
                                              0x0004040404ULL,
                                              0x0008080808ULL,
                                              0x0010101010ULL,
                                              0x0020202020ULL,
                                              0x0040404040ULL,
                                              0x0080808080ULL,
                                              0x00010101ULL,
                                              0x00020202ULL,
                                              0x00040404ULL,
                                              0x00080808ULL,
                                              0x00101010ULL,
                                              0x00202020ULL,
                                              0x00404040ULL,
                                              0x00808080ULL,
                                              0x000101ULL,
                                              0x000202ULL,
                                              0x000404ULL,
                                              0x000808ULL,
                                              0x001010ULL,
                                              0x002020ULL,
                                              0x004040ULL,
                                              0x008080ULL,
                                              0x0001ULL,
                                              0x0002ULL,
                                              0x0004ULL,
                                              0x0008ULL,
                                              0x0010ULL,
                                              0x0020ULL,
                                              0x0040ULL,
                                              0x0080ULL,
                                              0,
                                              0,
                                              0,
                                              0,
                                              0,
                                              0,
                                              0,
                                              0};

int countIsolatedPawns(const Position &pos, bool is_white) {
  int isolated_pawns = 0;
  uint64_t pawns = is_white ? pos.WhitePawns : pos.BlackPawns;
  uint64_t friendly_pawns = pawns;

  while (pawns) {
    int sq = __builtin_ctzll(pawns);
    int file = sq % 8;

    uint64_t adjacent_files = 0ULL;
    if (file > 0)
      adjacent_files |= file_masks[file - 1];
    if (file < 7)
      adjacent_files |= file_masks[file + 1];

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
      if (!(enemy_pawns & white_passed_pawn_masks[sq])) {
        passed_pawns++;
      }
    } else {
      if (!(enemy_pawns & black_passed_pawn_masks[sq])) {
        passed_pawns++;
      }
    }
    pawns &= pawns - 1;
  }
  return passed_pawns;
}

int calculateGamePhase(const Position &pos) {
  // Tapered eval phase in [0,24]: more means middlegame; less means endgame
  int phase = 0;
  phase += __builtin_popcountll(pos.WhiteKnights);
  phase += __builtin_popcountll(pos.BlackKnights);
  phase += __builtin_popcountll(pos.WhiteBishops);
  phase += __builtin_popcountll(pos.BlackBishops);
  phase += 2 * __builtin_popcountll(pos.WhiteRooks);
  phase += 2 * __builtin_popcountll(pos.BlackRooks);
  phase += 4 * __builtin_popcountll(pos.WhiteQueen);
  phase += 4 * __builtin_popcountll(pos.BlackQueen);
  if (phase > 24)
    phase = 24;
  return phase;
}

void evaluate_mobility(Position &pos, int &score, int phase) {
  auto popcount = [](uint64_t x) { return __builtin_popcountll(x); };
  auto mobility_side = [&](bool white) {
    int mob = 0;
    uint64_t bbk = white ? pos.WhiteKnights : pos.BlackKnights;
    while (bbk) {
      int sq = __builtin_ctzll(bbk);
      uint64_t attacks =
          knightAttacks[sq] &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares);
      mob += popcount(attacks);
      bbk &= bbk - 1;
    }
    uint64_t bbb = white ? pos.WhiteBishops : pos.BlackBishops;
    while (bbb) {
      int sq = __builtin_ctzll(bbb);
      uint64_t attacks =
          get_bishop_attacks(sq, pos.occupiedSquares) &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares);
      mob += popcount(attacks);
      bbb &= bbb - 1;
    }
    uint64_t bbr = white ? pos.WhiteRooks : pos.BlackRooks;
    while (bbr) {
      int sq = __builtin_ctzll(bbr);
      uint64_t attacks =
          get_rook_attacks(sq, pos.occupiedSquares) &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares);
      mob += popcount(attacks);
      bbr &= bbr - 1;
    }
    uint64_t bbq = white ? pos.WhiteQueen : pos.BlackQueen;
    while (bbq) {
      int sq = __builtin_ctzll(bbq);
      uint64_t attacks =
          (get_bishop_attacks(sq, pos.occupiedSquares) |
           get_rook_attacks(sq, pos.occupiedSquares)) &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares);
      mob += popcount(attacks);
      bbq &= bbq - 1;
    }
    return mob;
  };
  // Weights MG/EG
  int mob_w = ((phase * 2) + ((24 - phase) * 1)) / 24; // slightly higher in MG
  score += mob_w * (mobility_side(true) - mobility_side(false));
}

void evaluate_king_shield(const Position &pos, int &score, int phase) {
  auto king_pawn_shield = [&](bool white) {
    uint64_t king = white ? pos.WhiteKing : pos.BlackKing;
    if (!king)
      return;
    int ks = __builtin_ctzll(king);
    int rank = ks / 8;
    int file = ks % 8;
    int dir = white ? -1 : 1; // forward direction towards opponent
    int shield_rank = rank + dir;
    int bonus = 0;
    if (shield_rank >= 0 && shield_rank < 8) {
      for (int df = -1; df <= 1; ++df) {
        int f = file + df;
        if (f < 0 || f > 7)
          continue;
        int sq = shield_rank * 8 + f;
        uint64_t mask = 1ULL << sq;
        if (white) {
          if (pos.WhitePawns & mask)
            bonus += 8;
        } else {
          if (pos.BlackPawns & mask)
            bonus += 8;
        }
      }
    }
    // Taper bonus to middlegame
    score += white ? (bonus * phase) / 24 : -(bonus * phase) / 24;
  };
  king_pawn_shield(true);
  king_pawn_shield(false);
}

void evaluate_rook_files(const Position &pos, int &score,
                         const EvalConfig &config) {
  auto apply_rook_file_bonus = [&](bool white) {
    uint64_t rooks = white ? pos.WhiteRooks : pos.BlackRooks;
    while (rooks) {
      int sq = __builtin_ctzll(rooks);
      int file = sq % 8;
      bool friendly_pawn =
          __builtin_popcountll((white ? pos.WhitePawns : pos.BlackPawns) &
                               file_masks[file]) > 0;
      bool enemy_pawn =
          __builtin_popcountll((white ? pos.BlackPawns : pos.WhitePawns) &
                               file_masks[file]) > 0;
      if (!friendly_pawn && !enemy_pawn) {
        score +=
            white ? config.rook_open_file_bonus : -config.rook_open_file_bonus;
      } else if (!friendly_pawn && enemy_pawn) {
        score += white ? config.rook_semi_open_file_bonus
                       : -config.rook_semi_open_file_bonus;
      }
      rooks &= rooks - 1;
    }
  };
  apply_rook_file_bonus(true);
  apply_rook_file_bonus(false);
}

void evaluate_bishop_pair(const Position &pos, int &score,
                          const EvalConfig &config) {
  if (__builtin_popcountll(pos.WhiteBishops) >= 2) {
    score += config.bishop_pair_bonus;
  }
  if (__builtin_popcountll(pos.BlackBishops) >= 2) {
    score -= config.bishop_pair_bonus;
  }
}

void evaluate_pawns(const Position &pos, int &score, const EvalConfig &config) {
  score -= countIsolatedPawns(pos, true) * config.isolated_pawn_penalty;
  score += countIsolatedPawns(pos, false) * config.isolated_pawn_penalty;

  score -= countDoubledPawns(pos, true) * config.doubled_pawn_penalty;
  score += countDoubledPawns(pos, false) * config.doubled_pawn_penalty;

  score += countPassedPawns(pos, true) * config.passed_pawn_bonus;
  score -= countPassedPawns(pos, false) * config.passed_pawn_bonus;
}

void evaluate_material_and_pst(const Position &pos, int &score_mg,
                               int &score_eg) {
  uint64_t bb = pos.WhitePawns;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg += 100 + pawn_pst_mg[sq];
    score_eg += 100 + pawn_pst_eg[sq];
    bb &= bb - 1;
  }
  bb = pos.WhiteKnights;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg += 300 + knight_pst_mg[sq];
    score_eg += 300 + knight_pst_eg[sq];
    bb &= bb - 1;
  }
  bb = pos.WhiteBishops;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg += 320 + bishop_pst_mg[sq];
    score_eg += 320 + bishop_pst_eg[sq];
    bb &= bb - 1;
  }
  bb = pos.WhiteRooks;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg += 500 + rook_pst_mg[sq];
    score_eg += 500 + rook_pst_eg[sq];
    bb &= bb - 1;
  }
  bb = pos.WhiteQueen;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg += 900;
    score_eg += 900;
    bb &= bb - 1;
  }
  bb = pos.WhiteKing;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg += king_pst_mg[sq];
    score_eg += king_pst_eg[sq];
    bb &= bb - 1;
  }

  bb = pos.BlackPawns;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg -= 100 + pawn_pst_mg[63 - sq];
    score_eg -= 100 + pawn_pst_eg[63 - sq];
    bb &= bb - 1;
  }
  bb = pos.BlackKnights;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg -= 300 + knight_pst_mg[63 - sq];
    score_eg -= 300 + knight_pst_eg[63 - sq];
    bb &= bb - 1;
  }
  bb = pos.BlackBishops;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg -= 320 + bishop_pst_mg[63 - sq];
    score_eg -= 320 + bishop_pst_eg[63 - sq];
    bb &= bb - 1;
  }
  bb = pos.BlackRooks;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg -= 500 + rook_pst_mg[63 - sq];
    score_eg -= 500 + rook_pst_eg[63 - sq];
    bb &= bb - 1;
  }
  bb = pos.BlackQueen;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg -= 900;
    score_eg -= 900;
    bb &= bb - 1;
  }
  bb = pos.BlackKing;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    score_mg -= king_pst_mg[63 - sq];
    score_eg -= king_pst_eg[63 - sq];
    bb &= bb - 1;
  }
}

int evaluate(Position &pos, const EvalConfig &config) {
  int score_mg = pos.psq_score_mg;
  int score_eg = pos.psq_score_eg;
  int phase = calculateGamePhase(pos); // 0..24

  // evaluate_material_and_pst(pos, score_mg, score_eg); // Replaced by incremental update

  int score = (score_mg * phase + score_eg * (24 - phase)) / 24;

  evaluate_pawns(pos, score, config);

  evaluate_bishop_pair(pos, score, config);

  evaluate_rook_files(pos, score, config);

  evaluate_king_shield(pos, score, phase);

  evaluate_mobility(pos, score, phase);

  // Tempo - bonus for side to move (add to white's score before flipping
  // perspective)
  score += config.tempo_bonus;

  // Return score from side-to-move perspective
  return pos.whiteToMove ? score : -score;
}
