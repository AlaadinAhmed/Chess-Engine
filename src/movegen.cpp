#include <cstdio>
#include "movegen.hpp"
#include "bitboard.hpp"
#include "hash.hpp"
#include "magics.hpp"
#include "position.hpp"
#include "search.hpp"
#include <cstdint>
#include "globals.hpp"
#include "fen.hpp"

int history_ply = 0;
UndoInfo history[256];

extern ZobristKeys zkey;

void move_piece(Position &pos, int from, int to, Pieces piece);

uint64_t peekAttackedSquares(Position pos) {
  uint64_t attackedSquares = 0ULL;

  for (int square = 0; square < 64; square++) {
    uint64_t bitboard = setBitboard(square);

    if (pos.whiteToMove) { 
      if (pos.WhitePawns & bitboard) {
        attackedSquares |= GetPawnAttacks(pos, square, true);
      }
    } else { 
      if (pos.BlackPawns & bitboard) {
        attackedSquares |= GetPawnAttacks(pos, square, false);
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

uint64_t GetPawnMoves(const Position &pos, int square, bool by_white) {
  uint64_t moves = 0ULL;
  uint64_t pawnBitboard = 1ULL << square;

  if (by_white) {
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

    // En passant
    if (pos.enPassant != 0) {
        if (((pawnBitboard << 7) & (1ULL << pos.enPassant)) && ((square % 8) != 0)) {
            moves |= (pawnBitboard << 7);
        }
        if (((pawnBitboard << 9) & (1ULL << pos.enPassant)) && ((square % 8) != 7)) {
            moves |= (pawnBitboard << 9);
        }
    }
  } else { // Black to move
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

    // En passant
    if (pos.enPassant != 0) {
        if (((pawnBitboard >> 9) & (1ULL << pos.enPassant)) && ((square % 8) != 0)) {
            moves |= (pawnBitboard >> 9);
        }
        if (((pawnBitboard >> 7) & (1ULL << pos.enPassant)) && ((square % 8) != 7)) {
            moves |= (pawnBitboard >> 7);
        }
    }
  }
  return moves;
}

uint64_t GetKingMoves(const Position &pos) {
    uint64_t moves = 0;
    int king_square;
    if (pos.whiteToMove) {
        king_square = __builtin_ctzll(pos.WhiteKing);
        moves = kingAttacks[king_square] & ~pos.WhiteoccupiedSquares;
    } else {
        king_square = __builtin_ctzll(pos.BlackKing);
        moves = kingAttacks[king_square] & ~pos.BlackoccupiedSquares;
    }
    return moves;
}

uint64_t GetKnightAttacks(const Position &pos, int square) {
    if (pos.whiteToMove)
        return knightAttacks[square] & ~pos.WhiteoccupiedSquares;
    else
        return knightAttacks[square] & ~pos.BlackoccupiedSquares;
}

bool is_square_attacked(const Position &pos, int square, bool by_white) {
    if (!by_white) { // Check if attacked by black
        // Check for pawn attacks
        uint64_t black_pawns = pos.BlackPawns;
        while (black_pawns) {
            int pawn_sq = __builtin_ctzll(black_pawns);
            if (GetPawnAttacks(pos, pawn_sq, false) & (1ULL << square)) return true;
            black_pawns &= black_pawns - 1;
        }
        if (knightAttacks[square] & pos.BlackKnights) return true;
        if (get_bishop_attacks(square, pos.occupiedSquares) & (pos.BlackBishops | pos.BlackQueen)) return true;
        if (get_rook_attacks(square, pos.occupiedSquares) & (pos.BlackRooks | pos.BlackQueen)) return true;
        if (kingAttacks[square] & pos.BlackKing) return true;
    } else { // Check if attacked by white
        // Check for pawn attacks
        uint64_t white_pawns = pos.WhitePawns;
        while (white_pawns) {
            int pawn_sq = __builtin_ctzll(white_pawns);
            if (GetPawnAttacks(pos, pawn_sq, true) & (1ULL << square)) return true;
            white_pawns &= white_pawns - 1;
        }
        if (knightAttacks[square] & pos.WhiteKnights) return true;
        if (get_bishop_attacks(square, pos.occupiedSquares) & (pos.WhiteBishops | pos.WhiteQueen)) return true;
        if (get_rook_attacks(square, pos.occupiedSquares) & (pos.WhiteRooks | pos.WhiteQueen)) return true;
        if (kingAttacks[square] & pos.WhiteKing) return true;
    }
    return false;
}

void undomove(Position &pos, Move m) {
    history_ply--;
    pos.whiteToMove = history[history_ply].side;

    pos.enPassant = history[history_ply].oldEnPassant;
    pos.castelingRights = history[history_ply].oldCastelingRights;
    pos.move50rule = history[history_ply].oldHalfMove;
    currentHashKey = history[history_ply].oldHashKey;

    Pieces moved_piece = get_piece_at(pos, m.to);
    Pieces captured_piece = history[history_ply].oldCapturedPiece;

    uint64_t from_to_bb = (1ULL << m.from) | (1ULL << m.to);

    switch (moved_piece) {
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

    if (history[history_ply].isCastling) {
        if (moved_piece == W_KING) {
            move_piece(pos, history[history_ply].castlingRookTo, history[history_ply].castlingRookFrom, W_ROOK);
        } else { // B_KING
            move_piece(pos, history[history_ply].castlingRookTo, history[history_ply].castlingRookFrom, B_ROOK);
        }
    }

    if (captured_piece != NO_PIECE) {
        uint64_t captured_bb;
        if (history[history_ply].isEnPassant) { // En passant capture
            captured_bb = 1ULL << history[history_ply].enPassantCapturedPawnSquare;
            if (moved_piece == W_PAWN) { // White pawn captured black pawn
                pos.BlackPawns |= captured_bb;
            } else { // Black pawn captured white pawn
                pos.WhitePawns |= captured_bb;
            }
        } else { // Regular capture
            captured_bb = 1ULL << m.to;
            switch (captured_piece) {
                case W_PAWN: pos.WhitePawns |= captured_bb; break;
                case W_KNIGHT: pos.WhiteKnights |= captured_bb; break;
                case W_BISHOP: pos.WhiteBishops |= captured_bb; break;
                case W_ROOK: pos.WhiteRooks |= captured_bb; break;
                case W_QUEEN: pos.WhiteQueen |= captured_bb; break;
                case W_KING: pos.WhiteKing |= captured_bb; break;
                case B_PAWN: pos.BlackPawns |= captured_bb; break;
                case B_KNIGHT: pos.BlackKnights |= captured_bb; break;
                case B_BISHOP: pos.BlackBishops |= captured_bb; break;
                case B_ROOK: pos.BlackRooks |= captured_bb; break;
                case B_QUEEN: pos.BlackQueen |= captured_bb; break;
                case B_KING: pos.BlackKing |= captured_bb; break;
                case NO_PIECE: break;
            }
        }
    }

    pos.BlackoccupiedSquares = pos.BlackPawns | pos.BlackKnights | pos.BlackBishops | pos.BlackRooks | pos.BlackQueen | pos.BlackKing;
    pos.WhiteoccupiedSquares = pos.WhitePawns | pos.WhiteKnights | pos.WhiteBishops | pos.WhiteRooks | pos.WhiteQueen | pos.WhiteKing;
    pos.occupiedSquares = pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;
}

uint64_t GetPawnAttacks(const Position &pos, int square, bool by_white) {
    uint64_t attacks = 0;
    uint64_t pawn = 1ULL << square;
    if (by_white) {
        attacks |= (pawn << 7) & ~0x0101010101010101; // Not on H file
        attacks |= (pawn << 9) & ~0x8080808080808080; // Not on A file
    } else {
        attacks |= (pawn >> 7) & ~0x8080808080808080; // Not on A file
        attacks |= (pawn >> 9) & ~0x0101010101010101; // Not on H file
    }
    return attacks;
}

uint64_t GetQueenAttacks(const Position &pos, int square) {
  return get_rook_attacks(square, pos.occupiedSquares) |
         get_bishop_attacks(square, pos.occupiedSquares);
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
    history[history_ply].oldHashKey = currentHashKey;
    history[history_ply].oldCastelingRights = pos.castelingRights;
    history[history_ply].oldEnPassant = pos.enPassant;
    history[history_ply].oldHalfMove = pos.move50rule;
    history[history_ply].side = pos.whiteToMove;

    Pieces moved_piece = get_piece_at(pos, m.from);
    Pieces captured_piece = get_piece_at(pos, m.to);

    history[history_ply].isEnPassant = false; // Default to false
    if ((moved_piece == W_PAWN || moved_piece == B_PAWN) && (m.to == pos.enPassant)) { // En passant capture
        history[history_ply].isEnPassant = true;
        if (moved_piece == W_PAWN) {
            history[history_ply].oldCapturedPiece = B_PAWN;
            history[history_ply].enPassantCapturedPawnSquare = m.to - 8;
        } else {
            history[history_ply].oldCapturedPiece = W_PAWN;
            history[history_ply].enPassantCapturedPawnSquare = m.to + 8;
        }
    } else {
        history[history_ply].oldCapturedPiece = captured_piece;
    }
    history_ply++;

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
    } else if ((moved_piece == W_PAWN || moved_piece == B_PAWN) && (m.to == pos.enPassant)) { // En passant capture
        uint64_t captured_pawn_bb;
        if (moved_piece == W_PAWN) { // White pawn capturing black pawn en passant
            captured_pawn_bb = 1ULL << (m.to - 8);
            pos.BlackPawns &= ~captured_pawn_bb;
            currentHashKey ^= zkey.pieceKeys[B_PAWN][m.to - 8];
        } else { // Black pawn capturing white pawn en passant
            captured_pawn_bb = 1ULL << (m.to + 8);
            pos.WhitePawns &= ~captured_pawn_bb;
            currentHashKey ^= zkey.pieceKeys[W_PAWN][m.to + 8];
        }
    }

    move_piece(pos, m.from, m.to, moved_piece);

    history[history_ply].isCastling = false; // Default to false
    // Handle castling rook move
    if (moved_piece == W_KING && m.from == 4) {
        if (m.to == 6) { // Kingside castling
            move_piece(pos, 7, 5, W_ROOK);
            currentHashKey ^= zkey.pieceKeys[W_ROOK][7];
            currentHashKey ^= zkey.pieceKeys[W_ROOK][5];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 7;
            history[history_ply].castlingRookTo = 5;
        } else if (m.to == 2) { // Queenside castling
            move_piece(pos, 0, 3, W_ROOK);
            currentHashKey ^= zkey.pieceKeys[W_ROOK][0];
            currentHashKey ^= zkey.pieceKeys[W_ROOK][3];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 0;
            history[history_ply].castlingRookTo = 3;
        }
    } else if (moved_piece == B_KING && m.from == 60) {
        if (m.to == 62) { // Kingside castling
            move_piece(pos, 63, 61, B_ROOK);
            currentHashKey ^= zkey.pieceKeys[B_ROOK][63];
            currentHashKey ^= zkey.pieceKeys[B_ROOK][61];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 63;
            history[history_ply].castlingRookTo = 61;
        } else if (m.to == 58) { // Queenside castling
            move_piece(pos, 56, 59, B_ROOK);
            currentHashKey ^= zkey.pieceKeys[B_ROOK][56];
            currentHashKey ^= zkey.pieceKeys[B_ROOK][59];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 56;
            history[history_ply].castlingRookTo = 59;
        }
    }

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
    char fen_string[100];
    positionToFEN(pos, fen_string);
    // printf("Processing FEN: %s\n", fen_string);
    if (debug_mode) { printf("DEBUG: Entering generate_moves\n"); } // Added debug print

    uint64_t pieces;
    uint64_t moves; // Declared moves at function scope
    Move current_move;
    Position temp_pos;

    if (pos.whiteToMove) {
        // Pawns
        pieces = pos.WhitePawns;
        int pawn_moves_count = 0;
        if (debug_mode) { printf("DEBUG: Generating White Pawn moves\n"); } // Added debug print
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t current_pawn_moves = GetPawnMoves(pos, from, true);
            uint64_t temp_moves = current_pawn_moves;
            if (debug_mode) { printf("DEBUG: Pawn from %d, moves %llu\n", from, temp_moves); } // Added debug print
            while (temp_moves) {
                int to = __builtin_ctzll(temp_moves);
                                    if ((1ULL << to) & 0xFF00000000000000) { // Promotion
                                        if (debug_mode) { printf("DEBUG: Pawn promotion\n"); } // Added debug print
                    current_move = {from, to, W_QUEEN};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, W_ROOK};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, W_BISHOP};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, W_KNIGHT};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);
                } else {
                    if (debug_mode) { printf("DEBUG: Regular pawn move\n"); } // Added debug print
                    current_move = {from, to};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);
                }
                temp_moves &= temp_moves - 1;
            }
            pieces &= pieces - 1; // Move to the next pawn
        }
        if (debug_mode) { printf("DEBUG: Finished White Pawn moves\n"); } // Added debug print

        // Knights
        pieces = pos.WhiteKnights;
        if (debug_mode) { printf("DEBUG: Generating White Knight moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished White Knight moves\n"); }

        // Bishops
        pieces = pos.WhiteBishops;
        if (debug_mode) { printf("DEBUG: Generating White Bishop moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished White Bishop moves\n"); }

        // Rooks
        pieces = pos.WhiteRooks;
        if (debug_mode) { printf("DEBUG: Generating White Rook moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished White Rook moves\n"); }

        // Queens
        pieces = pos.WhiteQueen;
        if (debug_mode) { printf("DEBUG: Generating White Queen moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetQueenAttacks(pos, from) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished White Queen moves\n"); }

        // King
        pieces = pos.WhiteKing;
        if (debug_mode) { printf("DEBUG: Generating White King moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKingMoves(pos) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished White King moves\n"); }

    } else { // Black to move
        // Pawns
        pieces = pos.BlackPawns;
        int pawn_moves_count = 0;
        if (debug_mode) { printf("DEBUG: Generating Black Pawn moves\n"); } // Added debug print
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t current_pawn_moves = GetPawnMoves(pos, from, false);
            uint64_t temp_moves = current_pawn_moves;
            if (debug_mode) { printf("DEBUG: Pawn from %d, moves %llu\n", from, temp_moves); } // Added debug print
            while (temp_moves) {
                int to = __builtin_ctzll(temp_moves);
                if ((1ULL << to) & 0x00000000000000FF) { // Promotion
                    if (debug_mode) { printf("DEBUG: Pawn promotion\n"); } // Added debug print
                    current_move = {from, to, B_QUEEN};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, B_ROOK};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, B_BISHOP};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, B_KNIGHT};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);
                } else {
                    if (debug_mode) { printf("DEBUG: Regular pawn move\n"); } // Added debug print
                    current_move = {from, to};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        move_list.moves[move_list.count++] = current_move;
                        pawn_moves_count++;
                    }
                    undomove(temp_pos, current_move);
                }
                temp_moves &= temp_moves - 1;
            }
            pieces &= pieces - 1; // Move to the next pawn
        }
        if (debug_mode) { printf("DEBUG: Finished Black Pawn moves\n"); } // Added debug print

        // Knights
        pieces = pos.BlackKnights;
        if (debug_mode) { printf("DEBUG: Generating Black Knight moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished Black Knight moves\n"); }

        // Bishops
        pieces = pos.BlackBishops;
        if (debug_mode) { printf("DEBUG: Generating Black Bishop moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished Black Bishop moves\n"); }

        // Rooks
        pieces = pos.BlackRooks;
        if (debug_mode) { printf("DEBUG: Generating Black Rook moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished Black Rook moves\n"); }

        // Queens
        pieces = pos.BlackQueen;
        if (debug_mode) { printf("DEBUG: Generating Black Queen moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetQueenAttacks(pos, from) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished Black Queen moves\n"); }

        // King
        pieces = pos.BlackKing;
        if (debug_mode) { printf("DEBUG: Generating Black King moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKingMoves(pos) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished Black King moves\n"); }
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
            uint64_t moves = GetPawnMoves(pos, from, true) & opponent_pieces;
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
            uint64_t moves = GetPawnMoves(pos, from, false) & opponent_pieces;
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