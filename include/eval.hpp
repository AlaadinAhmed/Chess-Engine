/**
 * @file eval.hpp
 * @brief Contains the core position evaluation logic.
 * The evaluation function analyzes a position and assigns it a numerical score
 * representing how favorable it is for White. A positive score favors White,
 * while a negative score favors Black.
 * @defgroup evaluation Evaluation
 * @ingroup evaluation
 */

#pragma once
#include "position.hpp"

/**
 * @brief Configuration struct holding the weights for various evaluation terms.
 * This allows for easy tuning of the engine's positional understanding.
 */
struct EvalConfig {
    int isolated_pawn_penalty;   ///< Penalty for each isolated pawn.
    int doubled_pawn_penalty;    ///< Penalty for each doubled pawn.
    int passed_pawn_bonus;       ///< Bonus for each passed pawn.
    int bishop_pair_bonus;       ///< Bonus for having a bishop pair.
    int tempo_bonus;             ///< A small bonus for the side to move.
    int rook_open_file_bonus;    ///< Bonus for a rook on an open file.
    int rook_semi_open_file_bonus; ///< Bonus for a rook on a semi-open file.
};

/// @brief The default set of evaluation parameters used by the engine.
extern const EvalConfig default_eval_config;

/**
 * @brief Calculates the game phase, tapering from middlegame to endgame.
 * The phase is a number between 0 (endgame) and 24 (middlegame), based on the
 * non-pawn material on the board. This is used for tapered evaluations where
 * certain positional features have different importance in different phases of the game.
 * @param pos The current board position.
 * @return An integer score from 0 to 24.
 */
int calculateGamePhase(const Position &pos);

/**
 * @brief Counts the number of isolated pawns for a given color.
 * An isolated pawn has no friendly pawns on adjacent files.
 *
 * @param pos The current board position.
 * @param is_white The color of the pawns to count (true for white, false for black).
 * @return The number of isolated pawns.
 */
int countIsolatedPawns(const Position &pos, bool is_white);

/**
 * @brief Counts the number of doubled pawns for a given color.
 * A doubled pawn is a pawn that is on the same file as another friendly pawn.
 *
 * @param pos The current board position.
 * @param is_white The color of the pawns to count (true for white, false for black).
 * @return The number of doubled pawns.
 */
int countDoubledPawns(const Position &pos, bool is_white);

/**
 * @brief Counts the number of passed pawns for a given color.
 * A passed pawn has no opposing pawns in front of it on the same or adjacent files.
 *
 * @param pos The current board position.
 * @param is_white The color of the pawns to count (true for white, false for black).
 * @return The number of passed pawns.
 */
int countPassedPawns(const Position &pos, bool is_white);

/**
 * @brief Evaluates the given board position and returns a score.
 * The score is from the perspective of the side to move. It considers material balance,
 * piece-square tables (PST), pawn structure, and other positional factors.
 *
 * @param pos The position to evaluate.
 * @param config The evaluation configuration with weights for different terms.
 * @return The score of the position in centipawns. Positive favors white, negative favors black.
 */
void evaluate_bishop_pair(const Position &pos, int &score, const EvalConfig &config);
void evaluate_rook_files(const Position &pos, int &score, const EvalConfig &config);
void evaluate_king_shield(const Position &pos, int &score, int phase);
void evaluate_mobility(Position &pos, int &score, int phase);
void evaluate_pawns(const Position &pos, int &score, const EvalConfig &config);
void evaluate_material_and_pst(const Position &pos, int &score_mg, int &score_eg);

/**
 * @brief Evaluates the given board position and returns a score.
 * The score is from the perspective of the side to move. It considers material balance,
 * piece-square tables (PST), pawn structure, and other positional factors.
 *
 * @param pos The position to evaluate.
 * @param config The evaluation configuration with weights for different terms.
 * @return The score of the position in centipawns. Positive favors white, negative favors black.
 */
int evaluate(Position &pos, const EvalConfig &config = default_eval_config);
