/**
 * @file search.hpp
 * @brief Contains the declarations for the engine's search algorithms.
 * These functions are the "brain" of the engine, responsible for exploring
 * the game tree of possible moves to find the best one.
 */

#pragma once

#include "position.hpp"
#include <chrono>
#include <atomic>

/// @brief A flag to indicate if the engine is currently in a search. Used for time management.
extern std::atomic<bool> searching;
/// @brief A counter for the total number of nodes (positions) evaluated during a search (thread-safe).
extern std::atomic<long long> nodes_searched;
/// @brief The maximum selective depth reached during quiescence search (thread-safe).
extern std::atomic<int> seldepth;

/**
 * @brief Performs a quiescence search to stabilize the evaluation.
 *
 * Quiescence search is a limited-depth search that only considers "violent" moves
 * like captures. Its purpose is to avoid the "horizon effect", where a fixed-depth
 * search might stop just before a critical capture, leading to a misleading evaluation.
 * This function is called by the main alpha-beta search at the leaf nodes.
 *
 * @param pos The position to search from.
 * @param alpha The lower bound of the search window (best score for the maximizing player).
 * @param beta The upper bound of the search window (best score for the minimizing player).
 * @param start_time The time point when the entire search began.
 * @param move_time The total time allocated for the move in milliseconds.
 * @param ply The current ply from the root (for seldepth tracking).
 * @return The evaluated score of the position after tactical exchanges are resolved.
 */
int quiescence(Position &pos, int alpha, int beta, const std::chrono::high_resolution_clock::time_point& start_time, long long move_time, int ply);

/**
 * @brief The core recursive alpha-beta search function (implemented with Negamax).
 *
 * This function explores the game tree to a specified depth, using alpha-beta pruning
 * to dramatically reduce the number of nodes that need to be evaluated.
 *
 * @param pos The position to search from.
 * @param depth The remaining depth to search (counts down to 0).
 * @param alpha The lower bound of the search window.
 * @param beta The upper bound of the search window.
 * @param best_move Output parameter to store the best move found at this node.
 * @param start_time The time point when the entire search began.
 * @param move_time The total time allocated for the move in milliseconds.
 * @return The score of the position from the perspective of the side to move.
 */
int alpha_beta_search(Position &pos, int current_depth, int max_depth, int alpha, int beta, Move &best_move, std::chrono::time_point<std::chrono::high_resolution_clock> start_time, int move_time, bool allow_null, Move prev_move);

/**
 * @brief Entry point for a single-threaded search.
 *
 * This function initializes the search and calls the main recursive alpha-beta function.
 *
 * @param pos The position to search.
 * @param max_depth The maximum depth for the search.
 * @param move_time The allocated time for the search in milliseconds.
 * @param best_move Output parameter to store the best move found.
 * @return The score of the best move.
 */
int search(Position &pos, int max_depth, long long move_time, Move &best_move);

/**
 * @brief The top-level entry point for a parallel (multi-threaded) search.
 *
 * This function parallelizes the search at the root node by assigning different moves
 * to different threads (a technique often called Lazy SMP). It aggregates the results
 * to find the overall best move.
 *
 * @param pos The position to search.
 * @param max_depth The maximum depth for the search.
 * @param move_time The allocated time for the search in milliseconds.
 * @param best_move Output parameter to store the best move found.
 * @return The score of the best move.
 */
int search_root_parallel(Position &pos, int max_depth, long long move_time, Move &best_move);