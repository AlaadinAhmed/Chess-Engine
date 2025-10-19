/**
 * @file eval.hpp
 * @brief Contains the evaluation function.
 */

#pragma once
#include "position.hpp"

struct EvalConfig {
    int isolated_pawn_penalty;
    int doubled_pawn_penalty;
    int passed_pawn_bonus;
    int bishop_pair_bonus;
    int tempo_bonus;
    int rook_open_file_bonus;
    int rook_semi_open_file_bonus;
};

extern const EvalConfig default_eval_config;

/**
 * @brief Calculates tapered game phase in [0, 24], higher -> more middlegame.
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
int evaluate(Position &pos, const EvalConfig &config = default_eval_config);
