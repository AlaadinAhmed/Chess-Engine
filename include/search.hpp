#pragma once
#include "bitboard.hpp"
#include <cstdint>
struct TTEntry {
  uint64_t key;
  int score;
  int depth;
  Move bestmove;
};
