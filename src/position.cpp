#include "position.hpp"

Pieces get_piece_at(Position pos, int sq) {
    uint64_t bb = 1ULL << sq;
    if (pos.WhitePawns & bb) return W_PAWN;
    if (pos.WhiteKnights & bb) return W_KNIGHT;
    if (pos.WhiteBishops & bb) return W_BISHOP;
    if (pos.WhiteRooks & bb) return W_ROOK;
    if (pos.WhiteQueen & bb) return W_QUEEN;
    if (pos.WhiteKing & bb) return W_KING;
    if (pos.BlackPawns & bb) return B_PAWN;
    if (pos.BlackKnights & bb) return B_KNIGHT;
    if (pos.BlackBishops & bb) return B_BISHOP;
    if (pos.BlackRooks & bb) return B_ROOK;
    if (pos.BlackQueen & bb) return B_QUEEN;
    if (pos.BlackKing & bb) return B_KING;
    return NO_PIECE;
}