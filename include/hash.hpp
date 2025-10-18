#pragma once
#include "position.hpp"
#include <cstdint>

struct ZobristKeys {
  uint64_t pieceKeys[12][64];
  uint64_t castelingKeys[16];
  uint64_t epKeys[8];
  uint64_t sideKey;

  void initKeys();
};

extern ZobristKeys zkey;
uint64_t generate_random_key();
uint64_t calculate_initial_hash(Position &pos);
