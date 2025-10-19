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
#include "print.hpp"

thread_local int history_ply = 0;
thread_local UndoInfo history[256];

static inline void clear_history() {
    history_ply = 0;
}

extern ZobristKeys zkey;

void move_piece(Position &pos, int from, int to, Pieces piece);

uint64_t GetPawnAttacks(const Position &pos, int square, bool by_white) {
    uint64_t attacks = 0;
    uint64_t pawn = 1ULL << square;
    if (by_white) {
        attacks |= (pawn << 7) & ~FileA; // Not on A file
        attacks |= (pawn << 9) & ~FileH; // Not on H file
    } else {
        attacks |= (pawn >> 7) & ~FileH; // Not on H file
        attacks |= (pawn >> 9) & ~FileA; // Not on A file
    }
    return attacks;
}

bool is_square_attacked(const Position &pos, int square, bool by_white) {
    if (debug_mode) { printf("DEBUG: is_square_attacked for square %d, by_white %d\n", square, by_white); }
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

uint64_t GetPawnMoves(const Position &pos, int square, bool by_white) {
  uint64_t moves = 0ULL;
  uint64_t pawnBitboard = 1ULL << square;

  if (debug_mode) { printf("DEBUG: GetPawnMoves for square %d, by_white %d\n", square, by_white); }
  if (debug_mode) { printf("DEBUG: pos.emptySquares: %llu\n", pos.emptySquares); }
  if (debug_mode) { printf("DEBUG: pos.BlackoccupiedSquares: %llu\n", pos.BlackoccupiedSquares); }
  if (debug_mode) { printf("DEBUG: pos.WhiteoccupiedSquares: %llu\n", pos.WhiteoccupiedSquares); }

  if (by_white) {
    if (get_piece_at(pos, square + 8) == NO_PIECE) { // Single push
      moves |= (pawnBitboard << 8);
      if (debug_mode) { printf("DEBUG: moves after single push: %llu\n", moves); }

      if ((square >= 8 && square <= 15) && // On 2nd rank
          (get_piece_at(pos, square + 16) == NO_PIECE)) { // Double push
        moves |= (pawnBitboard << 16);
        if (debug_mode) { printf("DEBUG: moves after double push: %llu\n", moves); }
      }
    }

    if ((square % 8) != 0 &&
        ((pawnBitboard << 7) & pos.BlackoccupiedSquares)) {
      moves |= (pawnBitboard << 7);
      if (debug_mode) { printf("DEBUG: moves after left capture: %llu\n", moves); }
    }

    if ((square % 8) != 7 &&
        ((pawnBitboard << 9) & pos.BlackoccupiedSquares)) {
      moves |= (pawnBitboard << 9);
      if (debug_mode) { printf("DEBUG: moves after right capture: %llu\n", moves); }
    }

    // En passant
    if (pos.enPassant != 0) {
        if (((pawnBitboard << 7) & (1ULL << pos.enPassant)) && ((square % 8) != 0)) {
            moves |= (pawnBitboard << 7);
            if (debug_mode) { printf("DEBUG: moves after en passant left: %llu\n", moves); }
        }
        if (((pawnBitboard << 9) & (1ULL << pos.enPassant)) && ((square % 8) != 7)) {
            moves |= (pawnBitboard << 9);
            if (debug_mode) { printf("DEBUG: moves after en passant right: %llu\n", moves); }
        }
    }
  } else { // Black to move
    if (get_piece_at(pos, square - 8) == NO_PIECE) { // Single push
      moves |= (pawnBitboard >> 8);
      if (debug_mode) { printf("DEBUG: moves after single push: %llu\n", moves); }

      if ((square >= 48 && square <= 55) && // On 7th rank
          (get_piece_at(pos, square - 16) == NO_PIECE)) { // Double push
        moves |= (pawnBitboard >> 16);
        if (debug_mode) { printf("DEBUG: moves after double push: %llu\n", moves); }
      }
    }

    if ((square % 8) != 0 &&
        ((pawnBitboard >> 9) & pos.WhiteoccupiedSquares)) {
      moves |= (pawnBitboard >> 9);
      if (debug_mode) { printf("DEBUG: moves after left capture: %llu\n", moves); }
    }

    if ((square % 8) != 7 &&
        ((pawnBitboard >> 7) & pos.WhiteoccupiedSquares)) {
      moves |= (pawnBitboard >> 7);
      if (debug_mode) { printf("DEBUG: moves after right capture: %llu\n", moves); }
    }

    // En passant
    if (pos.enPassant != 0) {
        if (((pawnBitboard >> 9) & (1ULL << pos.enPassant)) && ((square % 8) != 0)) {
            moves |= (pawnBitboard >> 9);
            if (debug_mode) { printf("DEBUG: moves after en passant left: %llu\n", moves); }
        }
        if (((pawnBitboard >> 7) & (1ULL << pos.enPassant)) && ((square % 8) != 7)) {
            moves |= (pawnBitboard >> 7);
            if (debug_mode) { printf("DEBUG: moves after en passant right: %llu\n", moves); }
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
    history[history_ply].oldHashKey = pos.zobrist_key;
    history[history_ply].oldCastelingRights = pos.castelingRights;
    history[history_ply].oldEnPassant = pos.enPassant;
    history[history_ply].oldHalfMove = pos.move50rule;
    history[history_ply].side = pos.whiteToMove;

    Pieces moved_piece = get_piece_at(pos, m.from);
    Pieces captured_piece = get_piece_at(pos, m.to);

    history[history_ply].movedPiece = moved_piece;

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

    pos.zobrist_key ^= zkey.pieceKeys[moved_piece][m.from];
    pos.zobrist_key ^= zkey.pieceKeys[moved_piece][m.to];

    if (captured_piece != NO_PIECE) {
        pos.zobrist_key ^= zkey.pieceKeys[captured_piece][m.to];
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
            pos.zobrist_key ^= zkey.pieceKeys[B_PAWN][m.to - 8];
        } else { // Black pawn capturing white pawn en passant
            captured_pawn_bb = 1ULL << (m.to + 8);
            pos.WhitePawns &= ~captured_pawn_bb;
            pos.zobrist_key ^= zkey.pieceKeys[W_PAWN][m.to + 8];
        }
    }

    move_piece(pos, m.from, m.to, moved_piece);

    history[history_ply].isCastling = false; // Default to false
    // Handle castling rook move
    if (moved_piece == W_KING && m.from == 4) {
        if (m.to == 6) { // Kingside castling
            move_piece(pos, 7, 5, W_ROOK);
            pos.zobrist_key ^= zkey.pieceKeys[W_ROOK][7];
            pos.zobrist_key ^= zkey.pieceKeys[W_ROOK][5];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 7;
            history[history_ply].castlingRookTo = 5;
        } else if (m.to == 2) { // Queenside castling
            move_piece(pos, 0, 3, W_ROOK);
            pos.zobrist_key ^= zkey.pieceKeys[W_ROOK][0];
            pos.zobrist_key ^= zkey.pieceKeys[W_ROOK][3];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 0;
            history[history_ply].castlingRookTo = 3;
        }
    } else if (moved_piece == B_KING && m.from == 60) {
        if (m.to == 62) { // Kingside castling
            move_piece(pos, 63, 61, B_ROOK);
            pos.zobrist_key ^= zkey.pieceKeys[B_ROOK][63];
            pos.zobrist_key ^= zkey.pieceKeys[B_ROOK][61];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 63;
            history[history_ply].castlingRookTo = 61;
        } else if (m.to == 58) { // Queenside castling
            move_piece(pos, 56, 59, B_ROOK);
            pos.zobrist_key ^= zkey.pieceKeys[B_ROOK][56];
            pos.zobrist_key ^= zkey.pieceKeys[B_ROOK][59];
            history[history_ply].isCastling = true;
            history[history_ply].castlingRookFrom = 56;
            history[history_ply].castlingRookTo = 59;
        }
    }

    pos.zobrist_key ^= zkey.sideKey;

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
        pos.zobrist_key ^= zkey.castelingKeys[old_castling_rights];
        pos.zobrist_key ^= zkey.castelingKeys[pos.castelingRights];
    }

    if (pos.enPassant != 0) {
        pos.zobrist_key ^= zkey.epKeys[pos.enPassant % 8];
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
        pos.zobrist_key ^= zkey.epKeys[pos.enPassant % 8];
    }
    pos.BlackoccupiedSquares = pos.BlackPawns | pos.BlackKnights | pos.BlackBishops | pos.BlackRooks | pos.BlackQueen | pos.BlackKing;
    pos.WhiteoccupiedSquares = pos.WhitePawns | pos.WhiteKnights | pos.WhiteBishops | pos.WhiteRooks | pos.WhiteQueen | pos.WhiteKing;
    pos.occupiedSquares = pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;

    history_ply++;
    pos.whiteToMove = !pos.whiteToMove;
}

void undomove(Position &pos, Move m) {
    history_ply--;

    pos.whiteToMove = history[history_ply].side;
    pos.castelingRights = history[history_ply].oldCastelingRights;
    pos.enPassant = history[history_ply].oldEnPassant;
    pos.move50rule = history[history_ply].oldHalfMove;
    pos.zobrist_key = history[history_ply].oldHashKey;

    Pieces moved_piece = history[history_ply].movedPiece;
    Pieces captured_piece = history[history_ply].oldCapturedPiece;

    // Move back the moved piece
    move_piece(pos, m.to, m.from, moved_piece);

    // Restore captured piece if any
    if (captured_piece != NO_PIECE) {
        uint64_t to_bb = 1ULL << m.to;
        switch (captured_piece) {
            case W_PAWN: pos.WhitePawns |= to_bb; break;
            case W_KNIGHT: pos.WhiteKnights |= to_bb; break;
            case W_BISHOP: pos.WhiteBishops |= to_bb; break;
            case W_ROOK: pos.WhiteRooks |= to_bb; break;
            case W_QUEEN: pos.WhiteQueen |= to_bb; break;
            case W_KING: pos.WhiteKing |= to_bb; break;
            case B_PAWN: pos.BlackPawns |= to_bb; break;
            case B_KNIGHT: pos.BlackKnights |= to_bb; break;
            case B_BISHOP: pos.BlackBishops |= to_bb; break;
            case B_ROOK: pos.BlackRooks |= to_bb; break;
            case B_QUEEN: pos.BlackQueen |= to_bb; break;
            case B_KING: pos.BlackKing |= to_bb; break;
            case NO_PIECE: break;
        }
    } else if (history[history_ply].isEnPassant) { // En passant capture
        uint64_t captured_pawn_bb = 1ULL << history[history_ply].enPassantCapturedPawnSquare;
        if (moved_piece == W_PAWN) { // White pawn captured black pawn en passant
            pos.BlackPawns |= captured_pawn_bb;
        } else { // Black pawn captured white pawn en passant
            pos.WhitePawns |= captured_pawn_bb;
        }
    }

    // Handle castling rook move
    if (history[history_ply].isCastling) {
        if (moved_piece == W_KING) {
            if (m.to == 6) { // Kingside castling
                move_piece(pos, 5, 7, W_ROOK);
            } else if (m.to == 2) { // Queenside castling
                move_piece(pos, 3, 0, W_ROOK);
            }
        } else if (moved_piece == B_KING) {
            if (m.to == 62) { // Kingside castling
                move_piece(pos, 61, 63, B_ROOK);
            } else if (m.to == 58) { // Queenside castling
                move_piece(pos, 59, 56, B_ROOK);
            }
        }
    }

    pos.BlackoccupiedSquares = pos.BlackPawns | pos.BlackKnights | pos.BlackBishops | pos.BlackRooks | pos.BlackQueen | pos.BlackKing;
    pos.WhiteoccupiedSquares = pos.WhitePawns | pos.WhiteKnights | pos.WhiteBishops | pos.WhiteRooks | pos.WhiteQueen | pos.WhiteKing;
    pos.occupiedSquares = pos.BlackoccupiedSquares | pos.WhiteoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;
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
        if (debug_mode) { printf("DEBUG: Generating White Pawn moves\n"); } // Added debug print
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            if (get_piece_at(pos, from) == NO_PIECE) {
                printf("ERROR: from square is empty! from: %d", from);
                print_board_from_pos(pos);
                exit(1);
            }
            uint64_t current_pawn_moves = GetPawnMoves(pos, from, true);
            uint64_t temp_moves = current_pawn_moves;
            if (debug_mode) { printf("DEBUG: Pawn from %d, to %d\n", from, __builtin_ctzll(temp_moves)); } // Added debug print
            while (temp_moves) {
                int to = __builtin_ctzll(temp_moves);
                                    if ((1ULL << to) & 0xFF00000000000000) { // Promotion
                                        if (debug_mode) { printf("DEBUG: Pawn promotion\n"); } // Added debug print
                    current_move = {from, to, W_QUEEN};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, W_ROOK};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, W_BISHOP};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, W_KNIGHT};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);
                } else {
                    if (debug_mode) { printf("DEBUG: Regular pawn move\n"); } // Added debug print
                    current_move = {from, to};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
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
            if (get_piece_at(pos, from) == NO_PIECE) {
                printf("ERROR: from square is empty! from: %d", from);
                print_board_from_pos(pos);
                exit(1);
            }
            uint64_t moves = GetKnightAttacks(pos, from) & ~pos.WhiteoccupiedSquares;
            if (debug_mode) { printf("DEBUG: Knight from %d, to %d\n", from, __builtin_ctzll(moves)); }
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                    move_list.moves[move_list.count++] = current_move;
                }                undomove(temp_pos, current_move);
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
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
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
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
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
            uint64_t moves = (get_bishop_attacks(from, pos.occupiedSquares) | get_rook_attacks(from, pos.occupiedSquares)) & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false)) {
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
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
            moves = GetKingMoves(pos);
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                bool king_attacked = is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.WhiteKing), false);
                if (debug_mode) { printf("DEBUG: King move %d to %d, king attacked after move: %d\n", from, to, king_attacked); }
                if (!king_attacked) {
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;

            // Castling for White
            if (!is_square_attacked(pos, from, false)) { // King not in check
                // Kingside Castling
                if ((pos.castelingRights & 1) && // White Kingside Castling Right
                    get_piece_at(pos, 5) == NO_PIECE && get_piece_at(pos, 6) == NO_PIECE &&
                    !is_square_attacked(pos, 5, false) && // f1 not attacked
                    !is_square_attacked(pos, 6, false)) { // g1 not attacked
                    if (debug_mode) { printf("DEBUG: White Kingside Castling generated (e1g1)\n"); }
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                    move_list.moves[move_list.count++] = {from, 6}; // e1g1
                }
                // Queenside Castling
                if ((pos.castelingRights & 2) && // White Queenside Castling Right
                    get_piece_at(pos, 1) == NO_PIECE && get_piece_at(pos, 2) == NO_PIECE && get_piece_at(pos, 3) == NO_PIECE &&
                    !is_square_attacked(pos, 3, false) && // d1 not attacked
                    !is_square_attacked(pos, 2, false)) { // c1 not attacked
                    if (debug_mode) { printf("DEBUG: White Queenside Castling generated (e1c1)\n"); }
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                    move_list.moves[move_list.count++] = {from, 2}; // e1c1
                }
            }
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
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, B_ROOK};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, B_BISHOP};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);

                    current_move = {from, to, B_KNIGHT};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
                    }
                    undomove(temp_pos, current_move);
                } else {
                    if (debug_mode) { printf("DEBUG: Regular pawn move\n"); } // Added debug print
                    current_move = {from, to};
                    temp_pos = pos;
                    makemove(temp_pos, current_move);
                    if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                        if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                        move_list.moves[move_list.count++] = current_move;
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
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
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
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
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
            uint64_t moves = (get_rook_attacks(from, pos.occupiedSquares) | get_bishop_attacks(from, pos.occupiedSquares)) & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                if (!is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true)) {
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }

        // King
        pieces = pos.BlackKing;
        if (debug_mode) { printf("DEBUG: Generating Black King moves\n"); }
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            moves = GetKingMoves(pos);
            while (moves) {
                int to = __builtin_ctzll(moves);
                current_move = {from, to};
                temp_pos = pos;
                makemove(temp_pos, current_move);
                bool king_attacked = is_square_attacked(temp_pos, __builtin_ctzll(temp_pos.BlackKing), true);
                if (debug_mode) { printf("DEBUG: King move %d to %d, king attacked after move: %d\n", from, to, king_attacked); }
                if (!king_attacked) {
                    if (debug_mode) { printf("DEBUG: Incrementing move_list.count. Current count: %d\n", move_list.count); }
                    move_list.moves[move_list.count++] = current_move;
                }
                undomove(temp_pos, current_move);
                moves &= moves - 1;
            }

            // Castling for Black
            if (!is_square_attacked(pos, from, true)) { // King not in check
                // Kingside Castling
                if ((pos.castelingRights & 4) && // Black Kingside Castling Right
                    get_piece_at(pos, 61) == NO_PIECE && get_piece_at(pos, 62) == NO_PIECE &&
                    !is_square_attacked(pos, 61, true) && // f8 not attacked
                    !is_square_attacked(pos, 62, true)) { // g8 not attacked
                    move_list.moves[move_list.count++] = {from, 62}; // e8g8
                    if (debug_mode) { printf("DEBUG: Adding move: %s\n", move_to_uci({from, 62}).c_str()); } // Added debug print
                }
                // Queenside Castling
                if ((pos.castelingRights & 8) && // Black Queenside Castling Right
                    get_piece_at(pos, 57) == NO_PIECE && get_piece_at(pos, 58) == NO_PIECE && get_piece_at(pos, 59) == NO_PIECE &&
                    !is_square_attacked(pos, 59, true) && // d8 not attacked
                    !is_square_attacked(pos, 58, true)) { // c8 not attacked
                    move_list.moves[move_list.count++] = {from, 58}; // e8c8
                    if (debug_mode) { printf("DEBUG: Adding move: %s\n", move_to_uci({from, 58}).c_str()); } // Added debug print
                }
            }
            pieces &= pieces - 1;
        }
        if (debug_mode) { printf("DEBUG: Finished Black King moves\n"); } // Added debug print
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