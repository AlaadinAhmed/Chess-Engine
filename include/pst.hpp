#pragma once

extern const int pawn_pst_mg[64];
extern const int pawn_pst_eg[64];
extern const int knight_pst_mg[64];
extern const int knight_pst_eg[64];
extern const int bishop_pst_mg[64];
extern const int bishop_pst_eg[64];
extern const int rook_pst_mg[64];
extern const int rook_pst_eg[64];
extern const int king_pst_mg[64];
extern const int king_pst_eg[64];

#include "position.hpp"
void get_pst_score(Pieces piece, int square, int &mg, int &eg);
