/**
 * @file nnue.hpp
 * @brief NNUE (Neural Network Universal Chess Evaluator) interface
 */

#pragma once

#include "position.hpp"

namespace nnue {

/**
 * Initialize the NNUE evaluator with a network file
 * @param network_file Path to the .nnue file
 * @return true if initialization succeeded
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
 * Shutdown NNUE and free resources
 */
void shutdown();

} // namespace nnue
