/**
 * @file bitboard.hpp
 * @brief Contains functions for working with bitboards.
 * @defgroup board_representation Board Representation
 * @ingroup board_representation
 */

#pragma once
#include <cstdint>
#include <vector>
#include "position.hpp"

typedef uint64_t Bitboard;

/**
 * @brief Sets a bit on a bitboard.
 *
 * @param square The square to set.
 * @return The new bitboard.
 */
uint64_t setBitboard(int square);

/**
 * @brief Gets a bit from a bitboard.
 *
 * @param bitboard The bitboard.
 * @param square The square to get.
 * @return True if the bit is set, false otherwise.
 */
bool getBit(uint64_t bitboard, int square);

/**
 * @brief Gets the index of the least significant bit (LSB) from a bitboard.
 *
 * @param bitboard The bitboard.
 * @return The index of the LSB.
 */
int get_lsb_index(uint64_t bitboard);


extern uint64_t kingAttacks[64];
extern uint64_t knightAttacks[64];

extern const uint64_t FileA;
extern const uint64_t FileB;
extern const uint64_t FileC;
extern const uint64_t FileD;
extern const uint64_t FileE;
extern const uint64_t FileF;
extern const uint64_t FileG;
extern const uint64_t FileH;
extern const uint64_t Rank1;
extern const uint64_t Rank2;
extern const uint64_t Rank7;
extern const uint64_t Rank8;