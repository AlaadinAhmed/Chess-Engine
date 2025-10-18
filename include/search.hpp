#pragma once
#include "bitboard.hpp"
#include "position.hpp"
#include <cstdint>
struct UndoInfo {
  uint64_t oldHashKey;
  int oldCastelingRights;
  int oldEnPassant;
  Pieces oldCapturedPiece;
  int oldHalfMove;
  int side;
};

extern bool searching;

int search(Position &pos, int depth, int alpha, int beta, Move &best_move);
