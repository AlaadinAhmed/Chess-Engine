/**
 * @file movegen.hpp
 * @brief Contains functions for move generation.
 */

#pragma once
#include "position.hpp"
#include "bitboard.hpp"

/**
 * @brief Checks if a square is attacked by a given color.
 *
 * @param pos The position.
 * @param square The square to check.
 * @param by_white True if checking for attacks by white, false for black.
 * @return True if the square is attacked, false otherwise.
 */
bool is_square_attacked(const Position &pos, int square, bool by_white);



/**
 * @brief Gets the pawn moves for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the pawn moves.
 */
uint64_t GetPawnMoves(const Position &pos, int square);

/**
 * @brief Gets the pawn attacks for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the pawn attacks.
 */
uint64_t GetPawnAttacks(const Position &pos, int square, bool by_white);

uint64_t get_king_attacks(int square);
uint64_t get_knight_attacks(int square);

/**
 * @brief Gets the king moves for a given position.
 *
 * @param pos The position.
 * @return A bitboard of the king moves.
 */
uint64_t GetKingMoves(const Position &pos);/**
 * @brief Gets the knight moves for a given position.
 *
 * @param pos The position.
 * @return A bitboard of the knight moves.
 */
uint64_t GetKnightMoves (const Position &pos);

/**
 * @brief Gets the knight attacks for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the knight attacks.
 */
uint64_t GetKnightAttacks(const Position &pos, int square);



/**
 * @brief Gets the queen moves for a given square.
 *
 * @param pos The position.
 * @param square The square.
 * @return A bitboard of the queen moves.
 */
uint64_t GetQueenMoves(const Position &pos, int square);

/**
 * @brief Makes a move on the board.
 *
 * @param pos The position.
 * @param m The move.
 */
void makemove (Position& pos, Move m);
void undomove (Position& pos, Move m);

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

void initKingAttacks();

/**
 * @brief Struct for storing information to undo a move.
 */
struct UndoInfo {
    uint64_t oldHashKey; /**< The Zobrist key before the move. */
    uint8_t oldCastelingRights; /**< The castling rights before the move. */
    uint64_t oldEnPassant; /**< The en passant square before the move. */
    int oldHalfMove; /**< The half-move clock before the move. */
    Pieces oldCapturedPiece; /**< The piece captured by the move. */
    bool isEnPassant; /**< True if the move was an en passant capture. */
    int enPassantCapturedPawnSquare; /**< The square of the pawn captured by en passant. */
    bool isCastling; /**< True if the move was a castling move. */
    int castlingRookFrom; /**< The original square of the rook in a castling move. */
    int castlingRookTo; /**< The destination square of the rook in a castling move. */
    bool side; /**< The side to move before the move. */
    Pieces movedPiece; /**< The piece that was moved. */
};

uint64_t get_attacks_from_square(int square, int direction, uint64_t occupied_squares);