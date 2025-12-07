#include "pst.hpp"
#include "position.hpp"

void get_pst_score(Pieces piece, int square, int &mg, int &eg) {
    switch (piece) {
        case W_PAWN: mg = 100 + pawn_pst_mg[square]; eg = 100 + pawn_pst_eg[square]; break;
        case W_KNIGHT: mg = 300 + knight_pst_mg[square]; eg = 300 + knight_pst_eg[square]; break;
        case W_BISHOP: mg = 320 + bishop_pst_mg[square]; eg = 320 + bishop_pst_eg[square]; break;
        case W_ROOK: mg = 500 + rook_pst_mg[square]; eg = 500 + rook_pst_eg[square]; break;
        case W_QUEEN: mg = 900; eg = 900; break;
        case W_KING: mg = king_pst_mg[square]; eg = king_pst_eg[square]; break;
        case B_PAWN: mg = -(100 + pawn_pst_mg[63 - square]); eg = -(100 + pawn_pst_eg[63 - square]); break;
        case B_KNIGHT: mg = -(300 + knight_pst_mg[63 - square]); eg = -(300 + knight_pst_eg[63 - square]); break;
        case B_BISHOP: mg = -(320 + bishop_pst_mg[63 - square]); eg = -(320 + bishop_pst_eg[63 - square]); break;
        case B_ROOK: mg = -(500 + rook_pst_mg[63 - square]); eg = -(500 + rook_pst_eg[63 - square]); break;
        case B_QUEEN: mg = -900; eg = -900; break;
        case B_KING: mg = -king_pst_mg[63 - square]; eg = -king_pst_eg[63 - square]; break;
        default: mg = 0; eg = 0; break;
    }
}
