/**
 * @file hash.hpp
 * @brief Contains functions for Zobrist hashing.
 */

#pragma once
#include "position.hpp"
#include <cstdint>

/**
 * @brief Struct for the Zobrist keys.
 */
struct ZobristKeys {
  uint64_t pieceKeys[12][64]; /**< The piece keys. */
  uint64_t castelingKeys[16]; /**< The castling keys. */
  uint64_t epKeys[8];         /**< The en passant keys. */
  uint64_t sideKey;           /**< The side to move key. */

  /**
   * @brief Initializes the Zobrist keys.
   */
  void initKeys();
};

extern ZobristKeys zkey; /**< The Zobrist keys. */

/**
 * @brief Generates a random 64-bit number.
 *
 * @return A random 64-bit number.
 */
uint64_t generate_random_key();

/**
 * @brief Calculates the initial Zobrist hash for a position.
 *
 * @param pos The position.
 * @return The Zobrist hash.
 */
uint64_t calculate_initial_hash(Position &pos);