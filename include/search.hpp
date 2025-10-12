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
struct TTEntry {
  uint64_t key;
  int score;
  int depth;
  Move bestmove;
};
