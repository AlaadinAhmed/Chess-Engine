#include "hash.hpp"
#include <chrono>
#include <cstdint>
#include <random>
ZobristKeys zkey;
void ZobristKeys::initKeys() {
  for (int piece = 0; piece < 12; piece++) {
    for (int square; square < 64; square++) {
      zkey.pieceKeys[piece][square] = generate_random_key();
    }
  }
  for (int i = 0; i < 16; i++) {
    zkey.castelingKeys[i] = generate_random_key();
  }
  for (int i = 0; i < 8; i++) {
    zkey.epKeys[i] = generate_random_key();
  }
  zkey.sideKey = generate_random_key();
}
uint64_t calculate_initial_hash() std::mt19937_64 rngenerator(
    std::chrono::high_resolution_clock::now().time_since_epoch().count());
uint64_t generate_random_key() { return rngenerator(); }
