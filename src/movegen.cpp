#include "movegen.hpp"
#include "bitboard.hpp"
#include "hash.hpp"
#include "magics.hpp"
#include "position.hpp"
#include "search.hpp"
#include <cstdint>
#include "globals.hpp"
extern UndoInfo history[];
extern ZobristKeys zkey;

uint64_t peekAttackedSquares(Position pos) {
  uint64_t attackedSquares = 0ULL;

  for (int square = 0; square < 64; square++) {
    uint64_t bitboard = setBitboard(square);

    if (pos.whiteToMove) { 
      if (pos.WhitePawns & bitboard) {
        attackedSquares |= GetPawnAttacks(pos, square);
      }
    } else { 
      if (pos.BlackPawns & bitboard) {
        attackedSquares |= GetPawnAttacks(pos, square);
      }
    }

    if (pos.whiteToMove) { 
      if (pos.WhiteKnights & bitboard) {
        attackedSquares |= knightAttacks[square];
      }
    } else { 
      if (pos.BlackKnights & bitboard) {
        attackedSquares |= knightAttacks[square];
      }
    }

    if (pos.whiteToMove) { 
      if (pos.WhiteBishops & bitboard) {
        attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares);
      }
    } else { 
      if (pos.BlackBishops & bitboard) {
        attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares);
      }
    }

    if (pos.whiteToMove) { 
      if (pos.WhiteRooks & bitboard) {
        attackedSquares |= get_rook_attacks(square, pos.occupiedSquares);
      }
    } else { 
      if (pos.BlackRooks & bitboard) {
        attackedSquares |= get_rook_attacks(square, pos.occupiedSquares);
      }
    }

    if (pos.whiteToMove) { 
      if (pos.WhiteQueen & bitboard) {
        attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares) |
                           get_rook_attacks(square, pos.occupiedSquares);
      }
    } else { 
      if (pos.BlackQueen & bitboard) {
        attackedSquares |= get_bishop_attacks(square, pos.occupiedSquares) |
                           get_rook_attacks(square, pos.occupiedSquares);
      }
    }

    if (pos.whiteToMove) { 
      if (pos.WhiteKing & bitboard) {
        attackedSquares |= kingAttacks[square];
      }
    } else { 
      if (pos.BlackKing & bitboard) {
        attackedSquares |= kingAttacks[square];
      }
    }
  }

  return attackedSquares;
}

uint64_t GetPawnMoves(Position pos, int square) {
  uint64_t moves = 0ULL;
  uint64_t pawnBitboard = 1ULL << square;

  if (pos.whiteToMove) {
    if (pawnBitboard & pos.WhitePawns) {
      if ((pawnBitboard << 8) & pos.emptySquares) {
        moves |= (pawnBitboard << 8);

        if ((square >= 8 && square <= 15) &&
            ((pawnBitboard << 16) & pos.emptySquares)) {
          moves |= (pawnBitboard << 16);
        }
      }

      if ((square % 8) != 0 &&
          ((pawnBitboard << 7) & pos.BlackoccupiedSquares)) {
        moves |= (pawnBitboard << 7);
      }

      if ((square % 8) != 7 &&
          ((pawnBitboard << 9) & pos.BlackoccupiedSquares)) {
        moves |= (pawnBitboard << 9);
      }
    }
  } else {
    if (pawnBitboard & pos.BlackPawns) {
      if ((pawnBitboard >> 8) & pos.emptySquares) {
        moves |= (pawnBitboard >> 8);

        if ((square >= 48 && square <= 55) &&
            ((pawnBitboard >> 16) & pos.emptySquares)) {
          moves |= (pawnBitboard >> 16);
        }
      }

      if ((square % 8) != 0 &&
          ((pawnBitboard >> 9) & pos.WhiteoccupiedSquares)) {
        moves |= (pawnBitboard >> 9);
      }

      if ((square % 8) != 7 &&
          ((pawnBitboard >> 7) & pos.WhiteoccupiedSquares)) {
        moves |= (pawnBitboard >> 7);
      }
    }
  }

  return moves;
}

uint64_t GetPawnAttacks(Position pos, int square) {
  uint64_t attacks = 0ULL;
  uint64_t pawnBitboard = setBitboard(square);

  if (pos.whiteToMove) {
    if ((pawnBitboard & ~FileA) != 0) {
      attacks |= (pawnBitboard << 7);
    }
    if ((pawnBitboard & ~FileH) != 0) {
      attacks |= (pawnBitboard << 9);
    }
  } else {
    if ((pawnBitboard & ~FileA) != 0) {
      attacks |= (pawnBitboard >> 9);
    }
    if ((pawnBitboard & ~FileH) != 0) {
      attacks |= (pawnBitboard >> 7);
    }
  }

  return attacks;
}
uint64_t GetKingMoves(Position pos) {
  uint64_t moves = 0ULL;
  int kingSquare;

  if (pos.whiteToMove) {
    kingSquare = __builtin_ctzll(pos.WhiteKing);
    moves = kingAttacks[kingSquare];

    Position tempPos = pos;
    tempPos.whiteToMove = false;
    uint64_t attackedSquares = peekAttackedSquares(tempPos);

    moves &= ~(pos.WhiteoccupiedSquares | attackedSquares);
  } else {
    kingSquare = __builtin_ctzll(pos.BlackKing);
    moves = kingAttacks[kingSquare];

    Position tempPos = pos;
    tempPos.whiteToMove = true;
    uint64_t attackedSquares = peekAttackedSquares(tempPos);

    moves &= ~(pos.BlackoccupiedSquares | attackedSquares);
  }

  return moves;
}

void initKingAttacks() {
  for (int square = 0; square < 64; square++) {
    uint64_t bb = 1ULL << square;
    uint64_t attacks = 0ULL;

    attacks |= (bb << 8) & ~Rank8;           
    attacks |= (bb >> 8) & ~Rank1;           
    attacks |= (bb << 1) & ~FileH;           
    attacks |= (bb >> 1) & ~FileA;           
    attacks |= (bb << 9) & ~(Rank8 | FileH); 
    attacks |= (bb << 7) & ~(Rank8 | FileA); 
    attacks |= (bb >> 7) & ~(Rank1 | FileH); 
    attacks |= (bb >> 9) & ~(Rank1 | FileA); 

    kingAttacks[square] = attacks;
  }
}

uint64_t GetKnightAttacks(Position pos, int square) {
  if (pos.whiteToMove) {
    return knightAttacks[square] &
           ~(pos.WhiteoccupiedSquares | pos.BlackoccupiedSquares);
  }
  return knightAttacks[square] &
         ~(pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares);
}
uint64_t GetQueenAttacks(Position pos, int square) {
  if (pos.whiteToMove) {
    if (getBit(pos.WhiteQueen, square)) {
      return get_rook_attacks(square, pos.occupiedSquares) ||
             get_bishop_attacks(square, pos.occupiedSquares);
    }
  } else if (!pos.whiteToMove) {
    if (getBit(pos.BlackQueen, square)) {
      return get_rook_attacks(square, pos.occupiedSquares) ||
             get_bishop_attacks(square, pos.occupiedSquares);
    }
  } else {
    return 1;
  }
  return get_rook_attacks(square, pos.occupiedSquares) ||
         get_bishop_attacks(square, pos.occupiedSquares);
}

void move_piece(Position &pos, int from, int to, Pieces piece) {
    uint64_t from_to_bb = (1ULL << from) | (1ULL << to);
    switch (piece) {
        case W_PAWN: pos.WhitePawns ^= from_to_bb; break;
        case W_KNIGHT: pos.WhiteKnights ^= from_to_bb; break;
        case W_BISHOP: pos.WhiteBishops ^= from_to_bb; break;
        case W_ROOK: pos.WhiteRooks ^= from_to_bb; break;
        case W_QUEEN: pos.WhiteQueen ^= from_to_bb; break;
        case W_KING: pos.WhiteKing ^= from_to_bb; break;
        case B_PAWN: pos.BlackPawns ^= from_to_bb; break;
        case B_KNIGHT: pos.BlackKnights ^= from_to_bb; break;
        case B_BISHOP: pos.BlackBishops ^= from_to_bb; break;
        case B_ROOK: pos.BlackRooks ^= from_to_bb; break;
        case B_QUEEN: pos.BlackQueen ^= from_to_bb; break;
        case B_KING: pos.BlackKing ^= from_to_bb; break;
        case NO_PIECE: break;
    }
}

void makemove(Position &pos, Move m) {
    history->oldHashKey = currentHashKey;

    Pieces moved_piece = get_piece_at(pos, m.from);
    Pieces captured_piece = get_piece_at(pos, m.to);

    currentHashKey ^= zkey.pieceKeys[moved_piece][m.from];
    currentHashKey ^= zkey.pieceKeys[moved_piece][m.to];

    if (captured_piece != NO_PIECE) {
        currentHashKey ^= zkey.pieceKeys[captured_piece][m.to];
        uint64_t to_bb = 1ULL << m.to;
        switch (captured_piece) {
            case W_PAWN: pos.WhitePawns &= ~to_bb; break;
            case W_KNIGHT: pos.WhiteKnights &= ~to_bb; break;
            case W_BISHOP: pos.WhiteBishops &= ~to_bb; break;
            case W_ROOK: pos.WhiteRooks &= ~to_bb; break;
            case W_QUEEN: pos.WhiteQueen &= ~to_bb; break;
            case W_KING: pos.WhiteKing &= ~to_bb; break; 
            case B_PAWN: pos.BlackPawns &= ~to_bb; break;
            case B_KNIGHT: pos.BlackKnights &= ~to_bb; break;
            case B_BISHOP: pos.BlackBishops &= ~to_bb; break;
            case B_ROOK: pos.BlackRooks &= ~to_bb; break;
            case B_QUEEN: pos.BlackQueen &= ~to_bb; break;
            case B_KING: pos.BlackKing &= ~to_bb; break; 
            case NO_PIECE: break;
        }
    }

    move_piece(pos, m.from, m.to, moved_piece);

    currentHashKey ^= zkey.sideKey;

    uint8_t old_castling_rights = pos.castelingRights;
    if (moved_piece == W_KING) {
        pos.castelingRights &= ~1; 
        pos.castelingRights &= ~2; 
    } else if (moved_piece == B_KING) {
        pos.castelingRights &= ~4; 
        pos.castelingRights &= ~8; 
    } else if (moved_piece == W_ROOK) {
        if (m.from == 0) { 
            pos.castelingRights &= ~2; 
        } else if (m.from == 7) { 
            pos.castelingRights &= ~1; 
        }
    } else if (moved_piece == B_ROOK) {
        if (m.from == 56) { 
            pos.castelingRights &= ~8; 
        } else if (m.from == 63) { 
            pos.castelingRights &= ~4; 
        }
    }
    if (old_castling_rights != pos.castelingRights) {
        currentHashKey ^= zkey.castelingKeys[old_castling_rights];
        currentHashKey ^= zkey.castelingKeys[pos.castelingRights];
    }

    if (pos.enPassant != 0) {
        currentHashKey ^= zkey.epKeys[pos.enPassant / 8];
    }
    pos.enPassant = 0;
    if (moved_piece == W_PAWN) {
        if (m.to - m.from == 16) {
            pos.enPassant = m.from + 8;
        }
    } else if (moved_piece == B_PAWN) {
        if (m.from - m.to == 16) {
            pos.enPassant = m.to + 8;
        }
    }
    if (pos.enPassant != 0) {
        currentHashKey ^= zkey.epKeys[pos.enPassant / 8];
    }


    pos.BlackoccupiedSquares = pos.BlackPawns | pos.BlackKnights | pos.BlackBishops | pos.BlackRooks | pos.BlackQueen | pos.BlackKing;
    pos.WhiteoccupiedSquares = pos.WhitePawns | pos.WhiteKnights | pos.WhiteBishops | pos.WhiteRooks | pos.WhiteQueen | pos.WhiteKing;
    pos.occupiedSquares = pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;

    pos.whiteToMove = !pos.whiteToMove;
}

void generate_moves(Position &pos, MoveList &move_list) {
    move_list.count = 0;
    uint64_t pieces;
    if (pos.whiteToMove) {
        pieces = pos.WhitePawns;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetPawnMoves(pos, from);
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteKnights;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from);
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteBishops;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteRooks;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteQueen;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = (get_rook_attacks(from, pos.occupiedSquares) | get_bishop_attacks(from, pos.occupiedSquares)) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos);
        int from = __builtin_ctzll(pos.WhiteKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            move_list.moves[move_list.count++] = {from, to};
            moves &= moves - 1;
        }
    } else {
        pieces = pos.BlackPawns;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetPawnMoves(pos, from);
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackKnights;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from);
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackBishops;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackRooks;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackQueen;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = (get_rook_attacks(from, pos.occupiedSquares) | get_bishop_attacks(from, pos.occupiedSquares)) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos);
        int from = __builtin_ctzll(pos.BlackKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            move_list.moves[move_list.count++] = {from, to};
            moves &= moves - 1;
        }
    }
}

void generate_captures(Position &pos, MoveList &move_list) {
    move_list.count = 0;
    uint64_t pieces;
    if (pos.whiteToMove) {
        uint64_t opponent_pieces = pos.BlackoccupiedSquares;
        pieces = pos.WhitePawns;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetPawnMoves(pos, from) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteKnights;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteBishops;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteRooks;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteQueen;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = (get_rook_attacks(from, pos.occupiedSquares) | get_bishop_attacks(from, pos.occupiedSquares)) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos) & opponent_pieces;
        int from = __builtin_ctzll(pos.WhiteKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            move_list.moves[move_list.count++] = {from, to};
            moves &= moves - 1;
        }
    } else {
        uint64_t opponent_pieces = pos.WhiteoccupiedSquares;
        pieces = pos.BlackPawns;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetPawnMoves(pos, from) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackKnights;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackBishops;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackRooks;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackQueen;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = (get_rook_attacks(from, pos.occupiedSquares) | get_bishop_attacks(from, pos.occupiedSquares)) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                move_list.moves[move_list.count++] = {from, to};
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos) & opponent_pieces;
        int from = __builtin_ctzll(pos.BlackKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            move_list.moves[move_list.count++] = {from, to};
            moves &= moves - 1;
        }
    }
}