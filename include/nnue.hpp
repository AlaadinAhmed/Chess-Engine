/**
 * @file nnue.hpp
 * @brief NNUE (Neural Network Universal Chess Evaluator) interface
 */

#pragma once

#include "position.hpp"

namespace nnue {

/**
 * Initialize the NNUE evaluator with two network files (big + small)
 * @param big_net_file Path to the big .nnue file (~65MB)
 * @param small_net_file Path to the small .nnue file (~3.5MB)
 * @return true if initialization succeeded
 */
bool init(const char* big_net_file, const char* small_net_file);

/**
 * Legacy single-file init (not supported by new library)
 * @param network_file Path to the .nnue file
 * @return false (not supported)
 */
bool init(const char* network_file);

/**
 * Evaluate a position using NNUE
 * @param pos The position to evaluate
 * @return Score in centipawns from the side to move's perspective
 */
int evaluate(const Position& pos);

/**
 * Check if NNUE is initialized
 * @return true if NNUE is ready to use
 */
bool is_initialized();

/**
 * Update NNUE accumulator when a move is made
 * @param pos The position BEFORE the move is made (to detect captures)
 * @param m The move being made
 */
void make_move(const Position& pos, Move m);

/**
 * Revert NNUE accumulator when a move is undone
 * @param pos The position AFTER the move is undone (restored state)
 * @param m The move being undone
 */
void undo_move(const Position& pos, Move m);

/**
 * Update NNUE accumulator when a null move is made
 * @param pos The position BEFORE the null move is made
 */
void make_null_move(const Position& pos);

/**
 * Revert NNUE accumulator when a null move is undone
 * @param pos The position AFTER the null move is undone (restored state)
 */
void undo_null_move(const Position& pos);

/**
 * Refresh the NNUE accumulator from scratch (e.g. new game)
 * @param pos The current position
 */
void refresh_accumulator(const Position& pos);
void print_stats();

/**
 * Shutdown NNUE and free resources
 */
void shutdown();

} // namespace nnue
