/**
 * @file magics.hpp
 * @brief Contains functions for magic bitboards.
 * @defgroup move_generation Move Generation
 * @ingroup move_generation
 */

#pragma once
#include "position.hpp"

/**
 * @brief Initializes the magic bitboards.
 */
void init_magics();

/**
 * @brief Gets the rook attacks for a given square and occupancy.
 *
 * @param square The square.
 * @param occupancy The occupancy.
 * @return A bitboard of the rook attacks.
 */
uint64_t get_rook_attacks(int square, uint64_t occupancy);

/**
 * @brief Gets the bishop attacks for a given square and occupancy.
 *
 * @param square The square.
 * @param occupancy The occupancy.
 * @return A bitboard of the bishop attacks.
 */
uint64_t get_bishop_attacks(int square, uint64_t occupancy);
