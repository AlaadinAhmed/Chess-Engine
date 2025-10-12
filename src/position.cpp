#include "position.hpp"
#include "bitboard.hpp"
Pieces get_piece_at(Position pos, int sq) {
  if (getBit(pos.WhitePawns, sq)) {
    return W_PAWN;
  } else if (getBit(pos.WhiteBishops, sq)) {
    return W_BISHOP;
  } else if (getBit(pos.WhiteKnights, sq)) {
    return W_KNIGHT;
  } else if (getBit(pos.WhiteRooks, sq)) {
    return W_ROOK;
  } else if (getBit(pos.WhiteKing, sq)) {
    return W_KING;
  } else if (getBit(pos.WhiteQueen, sq)) {
    return W_QUEEN;
  }
  if (getBit(pos.BlackPawns, sq)) {
    return B_PAWN;
  } else if (getBit(pos.BlackBishops, sq)) {
    return B_BISHOP;
  } else if (getBit(pos.BlackKnights, sq)) {
    return B_KNIGHT;
  } else if (getBit(pos.BlackRooks, sq)) {
    return B_ROOK;
  } else if (getBit(pos.BlackKing, sq)) {
    return B_KING;
  } else if (getBit(pos.BlackQueen, sq)) {
    return B_QUEEN;
  } else {
    return NO_PIECE;
  }
}
