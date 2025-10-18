/**
 * @file position.hpp
 * @brief Contains the Position struct and related enums and functions.
 */

#pragma once
#include <cstdint>
#include <string>

/**
 * @brief Enum for the pieces.
 */
enum Pieces {
  W_KNIGHT,
  W_BISHOP,
  W_ROOK,
  W_PAWN,
  W_QUEEN,
  W_KING,
  B_KNIGHT,
  B_BISHOP,
  B_ROOK,
  B_PAWN,
  B_QUEEN,
  B_KING,
  NO_PIECE
};

/**
 * @brief Struct for a move.
 */
struct Move {
    int from; /**< The starting square of the move. */
    int to;   /**< The ending square of the move. */
    Pieces promotion = NO_PIECE; /**< The piece to promote to. */
};

/**
 * @brief Struct for a list of moves.
 */
struct MoveList {
    Move moves[256]; /**< The list of moves. */
    int count = 0;   /**< The number of moves in the list. */
};

/**
 * @brief Struct for the position on the board.
 */
struct Position {
  uint64_t BlackKnights = 0; /**< Bitboard for the black knights. */
  uint64_t BlackBishops = 0; /**< Bitboard for the black bishops. */
  uint64_t BlackRooks = 0;   /**< Bitboard for the black rooks. */
  uint64_t BlackQueen = 0;   /**< Bitboard for the black queen. */
  uint64_t BlackKing = 0;    /**< Bitboard for the black king. */
  uint64_t BlackPawns = 0;   /**< Bitboard for the black pawns. */

  uint64_t WhiteKnights = 0; /**< Bitboard for the white knights. */
  uint64_t WhiteBishops = 0; /**< Bitboard for the white bishops. */
  uint64_t WhiteRooks = 0;   /**< Bitboard for the white rooks. */
  uint64_t WhiteQueen = 0;   /**< Bitboard for the white queen. */
  uint64_t WhiteKing = 0;    /**< Bitboard for the white king. */
  uint64_t WhitePawns = 0;   /**< Bitboard for the white pawns. */

  bool whiteToMove;        /**< True if it's white's turn to move. */
  uint64_t enPassant = 0;    /**< The en passant square. */
  int move50rule = 0;      /**< The 50-move rule counter. */
  int move = 0;            /**< The current move number. */
  uint8_t castelingRights = 0; /**< The castling rights. */

  uint64_t BlackoccupiedSquares = 0; /**< Bitboard for all black pieces. */
  uint64_t WhiteoccupiedSquares = 0; /**< Bitboard for all white pieces. */
  uint64_t occupiedSquares = 0;    /**< Bitboard for all occupied squares. */
  uint64_t emptySquares = 0;       /**< Bitboard for all empty squares. */

  uint64_t zobrist_key = 0; /**< The Zobrist key for the position. */

  void setStartingPosition();
  void setFen(const std::string& fen_string);

  /**
   * @brief Resets the position to the starting position.
   */
  void reset() {
    BlackKnights = 0;
    BlackBishops = 0;
    BlackRooks = 0;
    BlackQueen = 0;
    BlackKing = 0;
    BlackPawns = 0;
    WhiteKnights = 0;
    WhiteBishops = 0;
    WhiteRooks = 0;
    WhiteQueen = 0;
    WhiteKing = 0;
    WhitePawns = 0;

    enPassant = 0;
    move50rule = 0;
    move = 0;
    castelingRights = 0;

    BlackoccupiedSquares = 0;
    WhiteoccupiedSquares = 0;
    occupiedSquares = 0;
    emptySquares = 0;

    zobrist_key = 0;
  }
};

/**
 * @brief Gets the piece at a given square.
 *
 * @param pos The position.
 * @param sq The square.
 * @return The piece at the given square.
 */
Pieces get_piece_at(const Position &pos, int sq);