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
    .backward_pawn_penalty = 10,
    .connected_pawn_bonus = 5,
    .king_open_file_penalty = 20,
    .pawn_storm_penalty = 10,
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

int countConnectedPawns(const Position &pos, bool is_white) {
    int connected_pawns = 0;
    uint64_t pawns = is_white ? pos.WhitePawns : pos.BlackPawns;
    uint64_t friendly_pawns = pawns;

    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        int file = sq % 8;
        int rank = sq / 8;

        // Check adjacent files for friendly pawns
        bool connected = false;
        if (file > 0) {
            if (friendly_pawns & file_masks[file - 1]) connected = true;
        }
        if (!connected && file < 7) {
            if (friendly_pawns & file_masks[file + 1]) connected = true;
        }

        if (connected) {
            connected_pawns++;
        }
        pawns &= pawns - 1;
    }
    return connected_pawns;
}

int countBackwardPawns(const Position &pos, bool is_white) {
    int backward_pawns = 0;
    uint64_t pawns = is_white ? pos.WhitePawns : pos.BlackPawns;
    uint64_t friendly_pawns = pawns;
    uint64_t enemy_pawns = is_white ? pos.BlackPawns : pos.WhitePawns;
    
    // Helper to check if a square is controlled by enemy pawns
    auto is_controlled_by_enemy_pawns = [&](int s) {
        if (is_white) return (blackPawnAttacks[s] & enemy_pawns) != 0;
        else return (whitePawnAttacks[s] & enemy_pawns) != 0;
    };

    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        int file = sq % 8;
        int rank = sq / 8;
        
        // 1. Check if it has friendly pawns on adjacent files behind or on same rank
        // If it does, it's not backward (it can be supported)
        bool supported = false;
        if (file > 0) {
            // Check file-1 for pawns on rank <= current rank
            // Mask for file-1: file_masks[file-1]
            // Mask for ranks <= rank: ... hard to construct efficiently without precalc
            // Let's iterate or use bit manipulation
            // Simpler: Check if any friendly pawn on adjacent file is behind or equal
            // Actually, backward pawn definition:
            // - No friendly pawn on adjacent files is further back or on same rank?
            // - Or: Cannot be safely advanced.
            
            // Let's use a standard definition:
            // A pawn is backward if:
            // 1. No friendly pawn on adjacent files is on the same rank or further back.
            // 2. The stop square (square in front) is controlled by an enemy pawn.
            
            uint64_t adj_mask = file_masks[file - 1];
            // Mask for ranks behind or equal: 
            // White (rank increasing): ranks 0 to rank
            // Black (rank decreasing): ranks rank to 7
            
            // This is getting complicated to do with just file masks.
            // Let's loop for now or use simple bit logic.
            
            // Optimization: 
            // White: (friendly_pawns & adj_mask) & ~((1ULL << (sq + 1)) - 1) ... wait, sq is index.
            // Rank mask: 
        }
        
        // Simplified Backward Pawn:
        // 1. No friendly pawn on adjacent files behind it.
        // 2. Stop square attacked by enemy pawn.
        
        bool has_support_behind = false;
        if (file > 0) {
             uint64_t file_bb = friendly_pawns & file_masks[file - 1];
             if (is_white) {
                 // Check for pawns on ranks < rank (actually <= rank usually)
                 // If there is a pawn on rank <= rank, it's supported.
                 // sq is current square. 
                 // We want to check if any bit in file_bb has index <= sq (roughly, but different file)
                 // Actually, just check if there is ANY friendly pawn on adjacent file that is BEHIND or EQUAL.
                 // White: index < sq (on adjacent file) -> rank is lower? No, file is different.
                 // Rank of sq: sq / 8.
                 // We need to check if file_bb contains any square with rank <= sq/8.
                 
                 // Let's iterate bits of file_bb
                 while (file_bb) {
                     int s = __builtin_ctzll(file_bb);
                     if (s / 8 <= rank) { has_support_behind = true; break; }
                     file_bb &= file_bb - 1;
                 }
             } else {
                 // Black: Check for pawns on ranks >= rank
                 while (file_bb) {
                     int s = __builtin_ctzll(file_bb);
                     if (s / 8 >= rank) { has_support_behind = true; break; }
                     file_bb &= file_bb - 1;
                 }
             }
        }
        if (!has_support_behind && file < 7) {
             uint64_t file_bb = friendly_pawns & file_masks[file + 1];
             if (is_white) {
                 while (file_bb) {
                     int s = __builtin_ctzll(file_bb);
                     if (s / 8 <= rank) { has_support_behind = true; break; }
                     file_bb &= file_bb - 1;
                 }
             } else {
                 while (file_bb) {
                     int s = __builtin_ctzll(file_bb);
                     if (s / 8 >= rank) { has_support_behind = true; break; }
                     file_bb &= file_bb - 1;
                 }
             }
        }

        if (!has_support_behind) {
            // Check stop square
            int stop_sq = is_white ? sq + 8 : sq - 8;
            if (stop_sq >= 0 && stop_sq < 64) {
                if (is_controlled_by_enemy_pawns(stop_sq)) {
                    backward_pawns++;
                }
            }
        }

        pawns &= pawns - 1;
    }
    return backward_pawns;
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
    
    // Calculate squares attacked by enemy pawns
    uint64_t enemy_pawns = white ? pos.BlackPawns : pos.WhitePawns;
    uint64_t unsafe_squares = 0;
    // We can iterate enemy pawns or use a precomputed attack map for all pawns?
    // Iterating is slow.
    // Better: Shift enemy pawns to get their attacks.
    // White pawns attack +7 and +9. Black pawns attack -7 and -9.
    // Wait, bitboard shifts.
    // White pawns at index i attack i+7 and i+9 (if not file wrap).
    // Black pawns at index i attack i-7 and i-9.
    // Let's use a helper or simple shifts.
    
    if (white) {
        // Enemy is Black. Black pawns attack "down" (lower indices).
        // Attacks: (pawns >> 7) & ~FileH | (pawns >> 9) & ~FileA
        // Wait, standard mapping:
        // Rank 0 is A1..H1. Rank 7 is A8..H8.
        // Black pawns on Rank 6 move to Rank 5 (-8).
        // Attack: -9 (Right/East? No, index decreases) and -7.
        // Let's verify:
        // Square 63 (H8). -9 = 54 (G7). -7 = 56 (A7? No, 56 is A8).
        // Wait, 63 is H8. 56 is A8.
        // 63 - 8 = 55 (H7).
        // 63 - 9 = 54 (G7). Correct.
        // 63 - 7 = 56 (A7? No, 56 is A8. 63-7 = 56).
        // Wait, 63-7 = 56. 56 is A8.
        // So H8 attacks A8? No.
        // H8 pawn attacks G7 and ... nothing off board.
        // H8 (63) -> G7 (54).
        // A8 (56) -> B7 (49).
        // A8 - 7 = 49 (B7).
        // A8 - 9 = 47 (H6? No, wrap).
        
        // Correct shifts for Black attacks:
        // (pawns >> 9) & ~FileH (if moving from higher to lower, right shift)
        // (pawns >> 7) & ~FileA
        
        // Let's double check.
        // Black pawn at B7 (49). Attacks A6 (40) and C6 (42).
        // 49 >> 9 = 40. Correct.
        // 49 >> 7 = 42. Correct.
        // Black pawn at A7 (48). Attacks B6 (41).
        // 48 >> 9 = 39 (H5? Wrap). Mask ~FileH needed?
        // 48 is on File A.
        // 48 >> 9 = 39. 39 is H5.
        // We need to mask out wraps.
        // If on File A, >> 9 wraps to H. So mask ~FileH.
        // If on File H, >> 7 wraps to A. So mask ~FileA.
        
        unsafe_squares = ((enemy_pawns >> 9) & ~FileH) | ((enemy_pawns >> 7) & ~FileA);
    } else {
        // Enemy is White. White pawns attack "up" (higher indices).
        // Attacks: (pawns << 9) & ~FileA | (pawns << 7) & ~FileH
        // White pawn at B2 (9). Attacks A3 (16) and C3 (18).
        // 9 << 7 = 16. Correct.
        // 9 << 9 = 18. Correct.
        // White pawn at A2 (8). Attacks B3 (17).
        // 8 << 7 = 15 (H1? Wrap). Mask ~FileH needed?
        // 8 is on File A.
        // 8 << 7 = 128? No.
        // 8 << 7 = 1024? No.
        // 8 + 7 = 15.
        // 15 is H1.
        // So A2 attacks H1? No.
        // We need to mask out wraps.
        // If on File A, << 7 wraps to H. So mask ~FileH.
        // If on File H, << 9 wraps to A. So mask ~FileA.
        
        unsafe_squares = ((enemy_pawns << 9) & ~FileA) | ((enemy_pawns << 7) & ~FileH);
    }

    uint64_t safe_mask = ~unsafe_squares;

    uint64_t bbk = white ? pos.WhiteKnights : pos.BlackKnights;
    while (bbk) {
      int sq = __builtin_ctzll(bbk);
      uint64_t attacks =
          knightAttacks[sq] &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares) &
          safe_mask;
      mob += popcount(attacks);
      bbk &= bbk - 1;
    }
    uint64_t bbb = white ? pos.WhiteBishops : pos.BlackBishops;
    while (bbb) {
      int sq = __builtin_ctzll(bbb);
      uint64_t attacks =
          get_bishop_attacks(sq, pos.occupiedSquares) &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares) &
          safe_mask;
      mob += popcount(attacks);
      bbb &= bbb - 1;
    }
    uint64_t bbr = white ? pos.WhiteRooks : pos.BlackRooks;
    while (bbr) {
      int sq = __builtin_ctzll(bbr);
      uint64_t attacks =
          get_rook_attacks(sq, pos.occupiedSquares) &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares) &
          safe_mask;
      mob += popcount(attacks);
      bbr &= bbr - 1;
    }
    uint64_t bbq = white ? pos.WhiteQueen : pos.BlackQueen;
    while (bbq) {
      int sq = __builtin_ctzll(bbq);
      uint64_t attacks =
          (get_bishop_attacks(sq, pos.occupiedSquares) |
           get_rook_attacks(sq, pos.occupiedSquares)) &
          ~(white ? pos.WhiteoccupiedSquares : pos.BlackoccupiedSquares) &
          safe_mask;
      mob += popcount(attacks);
      bbq &= bbq - 1;
    }
    return mob;
  };
  // Weights MG/EG
  int mob_w = ((phase * 2) + ((24 - phase) * 1)) / 24; // slightly higher in MG
  score += mob_w * (mobility_side(true) - mobility_side(false));
}

void evaluate_king_safety(const Position &pos, int &score, int phase, const EvalConfig &config) {
  auto king_safety_side = [&](bool white) {
    uint64_t king = white ? pos.WhiteKing : pos.BlackKing;
    if (!king)
      return;
    int ks = __builtin_ctzll(king);
    int rank = ks / 8;
    int file = ks % 8;
    int dir = white ? -1 : 1; // forward direction towards opponent (rank index change)
    // Wait, rank index: White 0-7. Forward for white is +1?
    // Board representation:
    // Rank 0: a1-h1. Rank 7: a8-h8.
    // White pawns move +8 (rank +1).
    // So forward for white is +1.
    // My previous code said: `int dir = white ? -1 : 1;`
    // Let's check `evaluate_king_shield` original code.
    // `int shield_rank = rank + dir;`
    // If white king is at rank 0, shield is at rank -1? That's wrong.
    // White king at rank 0, shield at rank 1. So dir should be +1 for white.
    // Let's verify `evaluate_king_shield` original code.
    // It was: `int dir = white ? -1 : 1;`
    // If white is true, dir is -1.
    // If rank is 0, shield_rank is -1.
    // Then `if (shield_rank >= 0 ...)` -> false.
    // So white king at rank 0 got NO shield bonus? That seems like a bug in previous code or I misunderstood rank layout.
    // Standard: Rank 0 is White pieces. Rank 7 is Black pieces.
    // White pawns move from Rank 1 to Rank 2...
    // So forward for White is +1.
    // Forward for Black is -1.
    
    int forward = white ? 1 : -1;
    
    int safety_score = 0;

    // 1. Pawn Shield & Storm
    // Check files: file-1, file, file+1
    for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
        uint64_t file_mask = file_masks[f];
        
        // Pawn Shield (Friendly pawns)
        bool has_shield = false;
        // Check immediate squares in front of king
        for (int r = rank + forward; r >= 0 && r < 8; r += forward) {
             // Just check the square immediately in front for shield
             // Or maybe 2 squares?
             // Original code checked `rank + dir`.
             // Let's check rank + forward.
             if (r == rank + forward) {
                 if ((white ? pos.WhitePawns : pos.BlackPawns) & (1ULL << (r * 8 + f))) {
                     safety_score += 10; // Shield bonus
                     has_shield = true;
                 }
             }
             // Stop after one square for shield? Or check further?
             // Let's keep it simple.
             if (has_shield) break;
        }

        // Open File Penalty (No friendly pawns on file)
        if (!((white ? pos.WhitePawns : pos.BlackPawns) & file_mask)) {
            safety_score -= config.king_open_file_penalty;
            // Semi-open (enemy pawns present)?
             if (!((white ? pos.BlackPawns : pos.WhitePawns) & file_mask)) {
                 // Fully open
                 safety_score -= 10; // Extra penalty
             }
        }

        // Pawn Storm (Enemy pawns advancing)
        uint64_t enemy_pawns = (white ? pos.BlackPawns : pos.WhitePawns) & file_mask;
        while (enemy_pawns) {
            int sq = __builtin_ctzll(enemy_pawns);
            int r = sq / 8;
            // Distance to king rank
            int dist = std::abs(r - rank);
            if (dist <= 3) {
                safety_score -= config.pawn_storm_penalty * (4 - dist); // Closer = more penalty
            }
            enemy_pawns &= enemy_pawns - 1;
        }
    }

    // Taper bonus to middlegame
    score += white ? (safety_score * phase) / 24 : -(safety_score * phase) / 24;
  };
  king_safety_side(true);
  king_safety_side(false);
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

  score += countConnectedPawns(pos, true) * config.connected_pawn_bonus;
  score -= countConnectedPawns(pos, false) * config.connected_pawn_bonus;

  score -= countBackwardPawns(pos, true) * config.backward_pawn_penalty;
  score += countBackwardPawns(pos, false) * config.backward_pawn_penalty;
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
  // Use NNUE evaluation if available
  #ifdef USE_NNUE
  extern int nnue_evaluate_position(Position& pos);
  return nnue_evaluate_position(pos);
  #endif

  int score_mg = pos.psq_score_mg;
  int score_eg = pos.psq_score_eg;
  int phase = calculateGamePhase(pos); // 0..24

  // evaluate_material_and_pst(pos, score_mg, score_eg); // Replaced by incremental update

  int score = (score_mg * phase + score_eg * (24 - phase)) / 24;

  evaluate_pawns(pos, score, config);

  evaluate_bishop_pair(pos, score, config);

  evaluate_rook_files(pos, score, config);

  evaluate_king_safety(pos, score, phase, config);

  evaluate_mobility(pos, score, phase);

  // Tempo - bonus for side to move (add to white's score before flipping
  // perspective)
  score += config.tempo_bonus;

  // Return score from side-to-move perspective
  return pos.whiteToMove ? score : -score;
}
