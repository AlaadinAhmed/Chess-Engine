#include <cstdint>
struct ZobristKeys {
  uint64_t pieceKeys[12][64];
  uint64_t castelingKeys[16];
  uint64_t epKeys[8];
  uint64_t sideKey;

  void initKeys();
};
uint64_t generate_random_key();
