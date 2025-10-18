/**
 * @file movegen.hpp
 * @brief Contains functions for move generation.
 */

#pragma once
#include "position.hpp"
#include "bitboard.hpp"

/**
 * @brief Peeks at the attacked squares.
 *
 * @param pos The position.
 * @return A bitboard of the attacked squares.
 */
uint64_t peekAttackedSquares (Position pos);

/**
 * @brief Gets the pawn moves for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the pawn moves.
 */
uint64_t GetPawnMoves(Position pos, int square);

/**
 * @brief Gets the pawn attacks for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the pawn attacks.
 */
uint64_t GetPawnAttacks(Position pos, int square);

/**
 * @brief Gets the king moves for a given position.
 *
 * @param pos The position.
 * @return A bitboard of the king moves.
 */
uint64_t GetKingMoves (Position pos);

/**
 * @brief Initializes the king attacks.
 */
void initKingAttacks();

/**
 * @brief Gets the knight moves for a given position.
 *
 * @param pos The position.
 * @return A bitboard of the knight moves.
 */
uint64_t GetKnightMoves (Position pos);

/**
 * @brief Gets the knight attacks for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the knight attacks.
 */
uint64_t GetKnightAttacks (Position pos, int square);

/**
 * @brief Gets the queen attacks for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the queen attacks.
 */
uint64_t GetQueenAttacks(Position pos, int square);

/**
 * @brief Gets the queen moves for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the queen moves.
 */
uint64_t GetQueenMoves(Position pos, int square);

/**
 * @brief Makes a move on the board.
 *
 * @param pos The position.
 * @param m The move.
 */
void makemove (Position& pos, Move m);

/**
 * @brief Generates all moves for a given position.
 *
 * @param pos The position.
 * @param move_list The list of moves to generate.
 */
void generate_moves(Position &pos, MoveList &move_list);

/**
 * @brief Generates all captures for a given position.
 *
 * @param pos The position.
 * @param move_list The list of captures to generate.
 */
void generate_captures(Position &pos, MoveList &move_list);