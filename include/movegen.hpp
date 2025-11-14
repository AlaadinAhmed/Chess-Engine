/**
 * @file movegen.hpp
 * @brief Declares functions and structures for move generation, validation, and execution.
 * This file contains the core logic for finding legal moves, checking for attacks,
 * and applying/reversing moves on the board state.
 */

#pragma once
#include "position.hpp"
#include "bitboard.hpp"

/**
 * @brief Checks if a specific square is under attack by a given color.
 * This is crucial for determining the legality of moves, especially for castling and king moves.
 *
 * @param pos The current board position.
 * @param square The square to check (0-63).
 * @param by_white True to check for attacks by White, false to check for attacks by Black.
 * @return True if the square is attacked, false otherwise.
 */
bool is_square_attacked(const Position &pos, int square, bool by_white);

/**
 * @brief Generates a bitboard of all possible moves for a pawn on a given square.
 * This includes single and double pushes, but not captures.
 *
 * @param pos The current board position.
 * @param square The square the pawn is on.
 * @param by_white True if the pawn is white, false if it is black.
 * @return A bitboard of squares the pawn can move to.
 */
uint64_t GetPawnMoves(const Position &pos, int square, bool by_white);

/**
 * @brief Generates a bitboard of all possible captures for a pawn on a given square.
 *
 * @param pos The current board position.
 * @param square The square the pawn is on.
 * @param by_white True if the pawn is white, false if it is black.
 * @return A bitboard of squares the pawn can capture.
 */
uint64_t GetPawnAttacks(const Position &pos, int square, bool by_white);

/**
 * @brief Gets pre-calculated king attacks from a given square.
 * @param square The square the king is on.
 * @return A bitboard of all squares a king can attack from that square.
 */
uint64_t get_king_attacks(int square);

/**
 * @brief Gets pre-calculated knight attacks from a given square.
 * @param square The square the knight is on.
 * @return A bitboard of all squares a knight can attack from that square.
 */
uint64_t get_knight_attacks(int square);

/**
 * @brief Generates a bitboard of all legal king moves in a given position.
 * This considers blockers and other pieces.
 *
 * @param pos The current board position.
 * @return A bitboard of squares the king can legally move to.
 */
uint64_t GetKingMoves(const Position &pos);

/**
 * @brief Generates a bitboard of all legal knight moves in a given position.
 *
 * @param pos The current board position.
 * @return A bitboard of squares the knight can legally move to.
 */
uint64_t GetKnightMoves (const Position &pos);

/**
 * @brief Generates a bitboard of all knight attacks in a given position.
 *
 * @param pos The current board position.
 * @param square The square of the knight.
 * @return A bitboard of squares the knight attacks.
 */
uint64_t GetKnightAttacks(const Position &pos, int square);

/**
 * @brief Generates a bitboard of all legal queen moves from a given square.
 *
 * @param pos The current board position.
 * @param square The square of the queen.
 * @return A bitboard of squares the queen can legally move to.
 */
uint64_t GetQueenMoves(const Position &pos, int square);

/**
 * @brief Applies a move to the board, updating the Position state.
 * This function does not check for legality; it only executes the move.
 *
 * @param pos The position to modify.
 * @param m The move to make.
 */
void makemove (Position& pos, Move m);

/**
 * @brief Reverts a move, restoring the Position state.
 * This is essential for the backtracking nature of the search algorithm.
 *
 * @param pos The position to modify.
 * @param m The move to undo.
 */
void undomove (Position& pos, Move m);

/**
 * @brief Generates all pseudo-legal and legal moves for the current side to move.
 *
 * @param pos The current board position.
 * @param move_list A reference to a MoveList that will be populated with the generated moves.
 */
void generate_moves(Position &pos, MoveList &move_list);

/**
 * @brief Generates all pseudo-legal and legal captures for the current side to move.
 * This is primarily used in quiescence search to analyze tactical sequences.
 *
 * @param pos The current board position.
 * @param move_list A reference to a MoveList that will be populated with the generated captures.
 */
void generate_captures(Position &pos, MoveList &move_list);

/**
 * @brief Generates all pseudo-legal and legal quiet (non-capture) moves for the current side to move.
 *
 * @param pos The current board position.
 * @param move_list A reference to a MoveList that will be populated with the generated quiet moves.
 */

void generate_quiet_moves(Position &pos, MoveList &move_list);

extern uint64_t kingAttacks[64];
extern uint64_t whitePawnAttacks[64];
extern uint64_t blackPawnAttacks[64];
extern uint64_t whitePawnMoves[64];
extern uint64_t blackPawnMoves[64];

/**
 * @brief Initializes pre-calculated attack tables for kings.
 * This should be called once at program startup.
 */
void initKingAttacks();

/**
 * @brief Initializes pre-calculated attack tables for pawns.
 * This should be called once at program startup.
 */
void initPawnAttacks();

/**
 * @brief Initializes pre-calculated move tables for pawns (single and double pushes).
 * This should be called once at program startup.
 */
void initPawnMoves();

/**
 * @brief Stores all necessary information to undo a move.
 * An instance of this struct is created before making a move during search,
 * allowing the engine to perfectly restore the state after backtracking.
 */
struct UndoInfo {
    uint64_t oldHashKey; ///< The Zobrist key before the move.
    uint8_t oldCastelingRights; ///< The castling rights before the move.
    uint64_t oldEnPassant; ///< The en passant square before the move.
    int oldHalfMove; ///< The half-move clock (for the 50-move rule) before the move.
    Pieces oldCapturedPiece; ///< The piece type that was captured, if any.
    bool isEnPassant; ///< True if the move was an en passant capture.
    int enPassantCapturedPawnSquare; ///< The square of the pawn captured by en passant.
    bool isCastling; ///< True if the move was a castling move.
    int castlingRookFrom; ///< The original square of the rook in a castling move.
    int castlingRookTo; ///< The destination square of the rook in a castling move.
    bool side; ///< The side to move before the move.
    Pieces movedPiece; ///< The piece that was moved.
};

/**
 * @brief Generates a bitboard of attacks for sliding pieces (rooks, bishops, queens).
 * @param square The square the piece is on.
 * @param direction The direction of attack (e.g., N, S, E, W, NE, etc.).
 * @param occupied_squares A bitboard of all occupied squares, used to determine blockers.
 * @return A bitboard of attacked squares in the given direction.
 */
uint64_t get_attacks_from_square(int square, int direction, uint64_t occupied_squares);