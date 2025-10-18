/**
 * @file search.hpp
 * @brief Contains functions for searching for the best move.
 */

#pragma once
#include "bitboard.hpp"
#include "position.hpp"
#include <cstdint>

/**
 * @brief Struct for storing information needed to undo a move.
 */
struct UndoInfo {
  uint64_t oldHashKey;      /**< The old Zobrist key. */
  int oldCastelingRights; /**< The old castling rights. */
  int oldEnPassant;       /**< The old en passant square. */
  Pieces oldCapturedPiece;  /**< The old captured piece. */
  int oldHalfMove;        /**< The old half-move clock. */
  int side;               /**< The side to move. */
};

extern bool searching; /**< True if the engine is currently searching. */

/**
 * @brief Searches for the best move in a given position.
 *
 * @param pos The position.
 * @param depth The depth to search to.
 * @param alpha The alpha value.
 * @param beta The beta value.
 * @param best_move The best move found.
 * @return The score of the position.
 */
int search(Position &pos, int depth, int alpha, int beta, Move &best_move);