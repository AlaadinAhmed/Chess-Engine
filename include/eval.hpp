/**
 * @file eval.hpp
 * @brief Contains the evaluation function.
 */

#pragma once
#include "position.hpp"

/**
 * @brief Calculates the current game phase (e.g., opening, middlegame, endgame).
 *
 * @param pos The current position.
 * @return An integer representing the game phase.
 */
int calculateGamePhase(const Position &pos);

/**
 * @brief Counts the number of isolated pawns for a given color.
 *
 * @param pos The current position.
 * @param color The color of the pawns (true for white, false for black).
 * @return The number of isolated pawns.
 */
int countIsolatedPawns(const Position &pos, bool is_white);

/**
 * @brief Counts the number of doubled pawns for a given color.
 *
 * @param pos The current position.
 * @param color The color of the pawns (true for white, false for black).
 * @return The number of doubled pawns.
 */
int countDoubledPawns(const Position &pos, bool is_white);

/**
 * @brief Counts the number of passed pawns for a given color.
 *
 * @param pos The current position.
 * @param color The color of the pawns (true for white, false for black).
 * @return The number of passed pawns.
 */
int countPassedPawns(const Position &pos, bool is_white);

/**
 * @brief Evaluates a position.
 *
 * @param pos The position to evaluate.
 * @return The score of the position.
 */
int evaluate(Position &pos);
