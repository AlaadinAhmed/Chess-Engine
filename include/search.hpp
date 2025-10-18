/**
 * @file search.hpp
 * @brief Contains functions for searching for the best move.
 */

#pragma once

#include "position.hpp"

extern bool searching;
extern long long nodes_searched;

int quiescence(Position &pos, int alpha, int beta);
int alpha_beta_search(Position &pos, int current_depth, int max_depth, int alpha, int beta, Move &best_move);
int search(Position &pos, int max_depth, long long move_time, Move &best_move);