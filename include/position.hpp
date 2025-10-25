/**
 * @file position.hpp
 * @brief Contains the core data structures for representing the chess board state,
 * including the Position struct, Move representation, and piece definitions.
 */

#pragma once
#include <cstdint>
#include <string>

/**
 * @brief Defines all piece types for both colors.
 * The naming convention is `W_` for White and `B_` for Black.
 */
enum Pieces {
  W_KNIGHT, ///< White Knight
  W_BISHOP, ///< White Bishop
  W_ROOK,   ///< White Rook
  W_PAWN,   ///< White Pawn
  W_QUEEN,  ///< White Queen
  W_KING,   ///< White King
  B_KNIGHT, ///< Black Knight
  B_BISHOP, ///< Black Bishop
  B_ROOK,   ///< Black Rook
  B_PAWN,   ///< Black Pawn
  B_QUEEN,  ///< Black Queen
  B_KING,   ///< Black King
  NO_PIECE  ///< Represents an empty square or no promotion piece.
};

enum PieceType {
    NO_PIECE_TYPE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

inline PieceType get_piece_type(Pieces p) {
    switch (p) {
        case W_PAWN: case B_PAWN: return PAWN;
        case W_KNIGHT: case B_KNIGHT: return KNIGHT;
        case W_BISHOP: case B_BISHOP: return BISHOP;
        case W_ROOK: case B_ROOK: return ROOK;
        case W_QUEEN: case B_QUEEN: return QUEEN;
        case W_KING: case B_KING: return KING;
        default: return NO_PIECE_TYPE;
    }
}

/**
 * @brief Represents a single chess move.
 * A move is defined by a starting square, an ending square, and an optional
 * promotion piece.
 */
struct Move {
    int from; ///< The starting square of the move (0-63).
    int to;   ///< The ending square of the move (0-63).
    Pieces promotion; ///< If the move is a promotion, this holds the piece to promote to. Defaults to NO_PIECE.
    int score = 0; ///< A score for move ordering.
};

/**
 * @brief A container for a list of possible moves in a position.
 */
struct MoveList {
    Move moves[256]; ///< A static array capable of holding all possible moves in any position.
    int count = 0;   ///< The number of moves currently stored in the list.
};

/**
 * @brief The core data structure representing the entire state of a chess game.
 *
 * This struct holds all information necessary to describe a position: piece locations,
 * current turn, castling rights, en passant square, and move counters. It uses
 * bitboards for an efficient representation of piece locations.
 */
struct Position {
  /** @name Piece Bitboards
   *  A set of 12 bitboards, one for each piece type and color. Each bit
   *  in the 64-bit integer corresponds to a square on the board.
   * @{
   */
  uint64_t BlackKnights = 0;
  uint64_t BlackBishops = 0;
  uint64_t BlackRooks = 0;
  uint64_t BlackQueen = 0;
  uint64_t BlackKing = 0;
  uint64_t BlackPawns = 0;

  uint64_t WhiteKnights = 0;
  uint64_t WhiteBishops = 0;
  uint64_t WhiteRooks = 0;
  uint64_t WhiteQueen = 0;
  uint64_t WhiteKing = 0;
  uint64_t WhitePawns = 0;
  /** @} */

  /** @name Game State
   *  Variables that define the current state of the game.
   * @{
   */
  bool whiteToMove;        ///< True if it is white's turn to move, false for black.
  uint64_t enPassant = 0;    ///< A bitboard with a single bit set on the en passant target square, if any. 0 otherwise.
  int move50rule = 0;      ///< Counter for the 50-move rule. Incremented for moves that are not pawn pushes or captures.
  int move = 0;            ///< The current full move number. Starts at 1 and increments after black moves.
  uint8_t castelingRights = 0; ///< Bitfield for castling rights. (e.g., 1=WK, 2=WQ, 4=BK, 8=BQ).
  /** @} */

  /** @name Occupancy Bitboards
   *  Combined bitboards for efficient lookup of occupied squares. These are derived
   *  from the individual piece bitboards.
   * @{
   */
  uint64_t BlackoccupiedSquares = 0; ///< Bitboard of all squares occupied by black pieces.
  uint64_t WhiteoccupiedSquares = 0; ///< Bitboard of all squares occupied by white pieces.
  uint64_t occupiedSquares = 0;    ///< Bitboard of all occupied squares (White | Black).
  uint64_t emptySquares = 0;       ///< Bitboard of all empty squares (~occupiedSquares).
  /** @} */

  /**
   * @brief A unique key for the position, used for transposition tables.
   * The Zobrist key is calculated incrementally by XORing random numbers
   * associated with each piece on each square, castling rights, en passant square,
   * and side to move.
   */
  uint64_t zobrist_key = 0;

  /**
   * @brief Sets the board to the standard chess starting position.
   */
  void setStartingPosition();

  /**
   * @brief Sets the board state from a Forsyth-Edwards Notation (FEN) string.
   * @param fen_string A valid FEN string.
   */
  void setFen(const std::string& fen_string);

  void make_null_move();
  void unmake_null_move();
  bool has_non_pawn_material(bool side) const;

  /**
   * @brief Resets all members of the Position struct to their default (empty) state.
   */
  void reset() {
    // Implementation is inlined for performance
    BlackKnights = 0; BlackBishops = 0; BlackRooks = 0; BlackQueen = 0; BlackKing = 0; BlackPawns = 0;
    WhiteKnights = 0; WhiteBishops = 0; WhiteRooks = 0; WhiteQueen = 0; WhiteKing = 0; WhitePawns = 0;
    enPassant = 0; move50rule = 0; move = 0; castelingRights = 0;
    BlackoccupiedSquares = 0; WhiteoccupiedSquares = 0; occupiedSquares = 0; emptySquares = 0;
    zobrist_key = 0;
  }
};

/**
 * @brief Gets the piece type located at a specific square.
 *
 * @param pos The const Position object to inspect.
 * @param sq The square index (0-63) to check.
 * @return The piece at the given square, or NO_PIECE if the square is empty.
 */
Pieces get_piece_at(const Position &pos, int sq);