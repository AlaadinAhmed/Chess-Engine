#pragma once
#include <cstdint>
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
struct Move {
    int from;
    int to;
};

struct MoveList {
    Move moves[256];
    int count = 0;
};

struct Position {
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

  bool whiteToMove;
  uint64_t enPassant = 0;
  int move50rule = 0;
  int move = 0;
  uint8_t castelingRights = 0;

  uint64_t BlackoccupiedSquares = 0;
  uint64_t WhiteoccupiedSquares = 0;
  uint64_t occupiedSquares = 0;
  uint64_t emptySquares = 0;

  uint64_t zobrist_key = 0;

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
Pieces get_piece_at(Position pos, int sq);
