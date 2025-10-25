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
#include "utils.hpp"

uint64_t kingAttacks[64];

void initKingAttacks() {
    for (int square = 0; square < 64; ++square) {
        kingAttacks[square] = 0ULL;
        int r = square / 8;
        int f = square % 8;
        if (r > 0) kingAttacks[square] |= (1ULL << (square - 8));
        if (r < 7) kingAttacks[square] |= (1ULL << (square + 8));
        if (f > 0) kingAttacks[square] |= (1ULL << (square - 1));
        if (f < 7) kingAttacks[square] |= (1ULL << (square + 1));
        if (r > 0 && f > 0) kingAttacks[square] |= (1ULL << (square - 9));
        if (r > 0 && f < 7) kingAttacks[square] |= (1ULL << (square - 7));
        if (r < 7 && f > 0) kingAttacks[square] |= (1ULL << (square + 7));
        if (r < 7 && f < 7) kingAttacks[square] |= (1ULL << (square + 9));
    }
}


// Castling rights
const int WK = 1, WQ = 2, BK = 4, BQ = 8;

void makemove(Position &pos, Move m) {
    log_debug("Enter makemove");
    UndoInfo& undo = undo_history[history_ply];
    undo.oldHashKey = pos.zobrist_key;
    undo.oldCastelingRights = pos.castelingRights;
    undo.oldEnPassant = pos.enPassant;
    undo.oldHalfMove = pos.move50rule;
    undo.movedPiece = get_piece_at(pos, m.from);
    undo.oldCapturedPiece = get_piece_at(pos, m.to);
    uint64_t to_bb_for_ep = 1ULL << m.to;
    undo.isEnPassant = (pos.enPassant != 0 && (to_bb_for_ep == pos.enPassant) && get_piece_type(undo.movedPiece) == PAWN);
    undo.isCastling = false;

    pos.move50rule++;

    if (pos.enPassant != 0) {
        int ep_sq = __builtin_ctzll(pos.enPassant);
        pos.zobrist_key ^= zkey.epKeys[ep_sq % 8];
        pos.enPassant = 0;
    }

    uint64_t from_bb = 1ULL << m.from;
    uint64_t to_bb = 1ULL << m.to;

    // Move piece
    if (pos.whiteToMove) {
        pos.WhiteoccupiedSquares &= ~from_bb;
        pos.WhiteoccupiedSquares |= to_bb;
        if (undo.movedPiece == W_PAWN) pos.WhitePawns &= ~from_bb, pos.WhitePawns |= to_bb;
        else if (undo.movedPiece == W_KNIGHT) pos.WhiteKnights &= ~from_bb, pos.WhiteKnights |= to_bb;
        else if (undo.movedPiece == W_BISHOP) pos.WhiteBishops &= ~from_bb, pos.WhiteBishops |= to_bb;
        else if (undo.movedPiece == W_ROOK) pos.WhiteRooks &= ~from_bb, pos.WhiteRooks |= to_bb;
        else if (undo.movedPiece == W_QUEEN) pos.WhiteQueen &= ~from_bb, pos.WhiteQueen |= to_bb;
        else if (undo.movedPiece == W_KING) pos.WhiteKing &= ~from_bb, pos.WhiteKing |= to_bb;
    } else {
        pos.BlackoccupiedSquares &= ~from_bb;
        pos.BlackoccupiedSquares |= to_bb;
        if (undo.movedPiece == B_PAWN) pos.BlackPawns &= ~from_bb, pos.BlackPawns |= to_bb;
        else if (undo.movedPiece == B_KNIGHT) pos.BlackKnights &= ~from_bb, pos.BlackKnights |= to_bb;
        else if (undo.movedPiece == B_BISHOP) pos.BlackBishops &= ~from_bb, pos.BlackBishops |= to_bb;
        else if (undo.movedPiece == B_ROOK) pos.BlackRooks &= ~from_bb, pos.BlackRooks |= to_bb;
        else if (undo.movedPiece == B_QUEEN) pos.BlackQueen &= ~from_bb, pos.BlackQueen |= to_bb;
        else if (undo.movedPiece == B_KING) pos.BlackKing &= ~from_bb, pos.BlackKing |= to_bb;
    }

    if (undo.movedPiece != NO_PIECE && m.from < 64) {
        pos.zobrist_key ^= zkey.pieceKeys[undo.movedPiece][m.from];
    }
    if (undo.movedPiece != NO_PIECE && m.to < 64) {
        pos.zobrist_key ^= zkey.pieceKeys[undo.movedPiece][m.to];
    }

    // Handle captures
    if (undo.oldCapturedPiece != NO_PIECE) {
        pos.move50rule = 0;
        if (pos.whiteToMove) pos.BlackoccupiedSquares &= ~to_bb;
        else pos.WhiteoccupiedSquares &= ~to_bb;
        
        // Remove captured piece from its specific bitboard
        switch (undo.oldCapturedPiece) {
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
            default: break;
        }
        
        if (undo.oldCapturedPiece != NO_PIECE) {
            pos.zobrist_key ^= zkey.pieceKeys[undo.oldCapturedPiece][m.to];
        }
    }

    // Handle pawn moves
    if (get_piece_type(undo.movedPiece) == PAWN) {
        pos.move50rule = 0;
        // En passant
        if (std::abs(m.from - m.to) == 16) {
            int ep_square = (m.from + m.to) / 2;
            pos.enPassant = 1ULL << ep_square;  // enPassant is a bitboard, not a square index!
            pos.zobrist_key ^= zkey.epKeys[ep_square % 8];
        }
        // En passant capture
        if (undo.isEnPassant) {
            int captured_pawn_sq = pos.whiteToMove ? m.to - 8 : m.to + 8;
            uint64_t captured_pawn_bb = 1ULL << captured_pawn_sq;
            if (pos.whiteToMove) {
                pos.BlackPawns &= ~captured_pawn_bb;
                pos.BlackoccupiedSquares &= ~captured_pawn_bb;
            } else {
                pos.WhitePawns &= ~captured_pawn_bb;
                pos.WhiteoccupiedSquares &= ~captured_pawn_bb;
            }
            if (pos.whiteToMove && captured_pawn_sq < 64) {
                pos.zobrist_key ^= zkey.pieceKeys[B_PAWN][captured_pawn_sq];
            } else if (!pos.whiteToMove && captured_pawn_sq < 64) {
                pos.zobrist_key ^= zkey.pieceKeys[W_PAWN][captured_pawn_sq];
            }
        }
        // Promotion
        if (m.promotion != NO_PIECE) {
            if (pos.whiteToMove) pos.WhitePawns &= ~to_bb;
            else pos.BlackPawns &= ~to_bb;
            pos.zobrist_key ^= zkey.pieceKeys[undo.movedPiece][m.to];
            pos.zobrist_key ^= zkey.pieceKeys[m.promotion][m.to];
            if (pos.whiteToMove) {
                if (m.promotion == W_QUEEN) pos.WhiteQueen |= to_bb;
                else if (m.promotion == W_ROOK) pos.WhiteRooks |= to_bb;
                else if (m.promotion == W_BISHOP) pos.WhiteBishops |= to_bb;
                else if (m.promotion == W_KNIGHT) pos.WhiteKnights |= to_bb;
            } else {
                if (m.promotion == B_QUEEN) pos.BlackQueen |= to_bb;
                else if (m.promotion == B_ROOK) pos.BlackRooks |= to_bb;
                else if (m.promotion == B_BISHOP) pos.BlackBishops |= to_bb;
                else if (m.promotion == B_KNIGHT) pos.BlackKnights |= to_bb;
            }
        }
    }

    // Handle castling
    if (get_piece_type(undo.movedPiece) == KING) {
        if (std::abs(m.from - m.to) == 2) { // Castling move
            undo.isCastling = true;
            int rook_from, rook_to;
            if (m.to == 6) rook_from = 7, rook_to = 5; // White kingside
            else if (m.to == 2) rook_from = 0, rook_to = 3; // White queenside
            else if (m.to == 62) rook_from = 63, rook_to = 61; // Black kingside
            else rook_from = 56, rook_to = 59; // Black queenside
            
            uint64_t rook_from_bb = 1ULL << rook_from;
            uint64_t rook_to_bb = 1ULL << rook_to;
            Pieces rook_piece = pos.whiteToMove ? W_ROOK : B_ROOK;

            if (pos.whiteToMove) {
                pos.WhiteRooks &= ~rook_from_bb, pos.WhiteRooks |= rook_to_bb;
                pos.WhiteoccupiedSquares &= ~rook_from_bb, pos.WhiteoccupiedSquares |= rook_to_bb;
            } else {
                pos.BlackRooks &= ~rook_from_bb, pos.BlackRooks |= rook_to_bb;
                pos.BlackoccupiedSquares &= ~rook_from_bb, pos.BlackoccupiedSquares |= rook_to_bb;
            }
            pos.zobrist_key ^= zkey.pieceKeys[rook_piece][rook_from];
            pos.zobrist_key ^= zkey.pieceKeys[rook_piece][rook_to];
        }
        if (pos.whiteToMove) pos.castelingRights &= ~(WK | WQ);
        else pos.castelingRights &= ~(BK | BQ);
    }

    // Update castling rights for rook moves
    if (undo.movedPiece == W_ROOK) {
        if (m.from == 0) pos.castelingRights &= ~WQ;
        if (m.from == 7) pos.castelingRights &= ~WK;
    }
    if (undo.movedPiece == B_ROOK) {
        if (m.from == 56) pos.castelingRights &= ~BQ;
        if (m.from == 63) pos.castelingRights &= ~BK;
    }
    if (undo.oldCapturedPiece == W_ROOK) {
        if (m.to == 0) pos.castelingRights &= ~WQ;
        if (m.to == 7) pos.castelingRights &= ~WK;
    }
    if (undo.oldCapturedPiece == B_ROOK) {
        if (m.to == 56) pos.castelingRights &= ~BQ;
        if (m.to == 63) pos.castelingRights &= ~BK;
    }

    pos.zobrist_key ^= zkey.castelingKeys[undo.oldCastelingRights];
    pos.zobrist_key ^= zkey.castelingKeys[pos.castelingRights];

    pos.whiteToMove = !pos.whiteToMove;
    pos.zobrist_key ^= zkey.sideKey;

    pos.occupiedSquares = pos.WhiteoccupiedSquares | pos.BlackoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;

    history_ply++;
}

void undomove(Position &pos, Move m) {
    history_ply--;
    const UndoInfo& undo = undo_history[history_ply];

    pos.whiteToMove = !pos.whiteToMove;
    pos.castelingRights = undo.oldCastelingRights;
    pos.enPassant = undo.oldEnPassant;
    pos.move50rule = undo.oldHalfMove;

    uint64_t from_bb = 1ULL << m.from;
    uint64_t to_bb = 1ULL << m.to;

    // Undo piece move
    if (pos.whiteToMove) {
        pos.WhiteoccupiedSquares |= from_bb;
        pos.WhiteoccupiedSquares &= ~to_bb;
        if (undo.movedPiece == W_PAWN) pos.WhitePawns |= from_bb, pos.WhitePawns &= ~to_bb;
        else if (undo.movedPiece == W_KNIGHT) pos.WhiteKnights |= from_bb, pos.WhiteKnights &= ~to_bb;
        else if (undo.movedPiece == W_BISHOP) pos.WhiteBishops |= from_bb, pos.WhiteBishops &= ~to_bb;
        else if (undo.movedPiece == W_ROOK) pos.WhiteRooks |= from_bb, pos.WhiteRooks &= ~to_bb;
        else if (undo.movedPiece == W_QUEEN) pos.WhiteQueen |= from_bb, pos.WhiteQueen &= ~to_bb;
        else if (undo.movedPiece == W_KING) pos.WhiteKing |= from_bb, pos.WhiteKing &= ~to_bb;
    } else {
        pos.BlackoccupiedSquares |= from_bb;
        pos.BlackoccupiedSquares &= ~to_bb;
        if (undo.movedPiece == B_PAWN) pos.BlackPawns |= from_bb, pos.BlackPawns &= ~to_bb;
        else if (undo.movedPiece == B_KNIGHT) pos.BlackKnights |= from_bb, pos.BlackKnights &= ~to_bb;
        else if (undo.movedPiece == B_BISHOP) pos.BlackBishops |= from_bb, pos.BlackBishops &= ~to_bb;
        else if (undo.movedPiece == B_ROOK) pos.BlackRooks |= from_bb, pos.BlackRooks &= ~to_bb;
        else if (undo.movedPiece == B_QUEEN) pos.BlackQueen |= from_bb, pos.BlackQueen &= ~to_bb;
        else if (undo.movedPiece == B_KING) pos.BlackKing |= from_bb, pos.BlackKing &= ~to_bb;
    }

    // Undo capture
    if (undo.oldCapturedPiece != NO_PIECE) {
        if (pos.whiteToMove) pos.BlackoccupiedSquares |= to_bb;
        else pos.WhiteoccupiedSquares |= to_bb;
        
        switch (undo.oldCapturedPiece) {
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
            default: break;
        }
    }

    // Undo promotion
    if (m.promotion != NO_PIECE) {
        // The pawn is already back at 'from' from the undo piece move above
        // We just need to remove the promoted piece from 'to'
        if (pos.whiteToMove) {
            if (m.promotion == W_QUEEN) pos.WhiteQueen &= ~to_bb;
            else if (m.promotion == W_ROOK) pos.WhiteRooks &= ~to_bb;
            else if (m.promotion == W_BISHOP) pos.WhiteBishops &= ~to_bb;
            else if (m.promotion == W_KNIGHT) pos.WhiteKnights &= ~to_bb;
        } else {
            if (m.promotion == B_QUEEN) pos.BlackQueen &= ~to_bb;
            else if (m.promotion == B_ROOK) pos.BlackRooks &= ~to_bb;
            else if (m.promotion == B_BISHOP) pos.BlackBishops &= ~to_bb;
            else if (m.promotion == B_KNIGHT) pos.BlackKnights &= ~to_bb;
        }
    }

    // Undo en passant
    if (undo.isEnPassant) {
        int captured_pawn_sq = pos.whiteToMove ? m.to - 8 : m.to + 8;
        uint64_t captured_pawn_bb = 1ULL << captured_pawn_sq;
        if (pos.whiteToMove) {
            pos.BlackPawns |= captured_pawn_bb;
            pos.BlackoccupiedSquares |= captured_pawn_bb;
        } else {
            pos.WhitePawns |= captured_pawn_bb;
            pos.WhiteoccupiedSquares |= captured_pawn_bb;
        }
    }

    // Undo castling
    if (undo.isCastling) {
        int rook_from, rook_to;
        if (m.to == 6) rook_from = 7, rook_to = 5; // White kingside
        else if (m.to == 2) rook_from = 0, rook_to = 3; // White queenside
        else if (m.to == 62) rook_from = 63, rook_to = 61; // Black kingside
        else rook_from = 56, rook_to = 59; // Black queenside
        
        uint64_t rook_from_bb = 1ULL << rook_from;
        uint64_t rook_to_bb = 1ULL << rook_to;

        if (pos.whiteToMove) {
            pos.WhiteRooks |= rook_from_bb, pos.WhiteRooks &= ~rook_to_bb;
            pos.WhiteoccupiedSquares |= rook_from_bb, pos.WhiteoccupiedSquares &= ~rook_to_bb;
        } else {
            pos.BlackRooks |= rook_from_bb, pos.BlackRooks &= ~rook_to_bb;
            pos.BlackoccupiedSquares |= rook_from_bb, pos.BlackoccupiedSquares &= ~rook_to_bb;
        }
    }

    pos.occupiedSquares = pos.WhiteoccupiedSquares | pos.BlackoccupiedSquares;
    pos.emptySquares = ~pos.occupiedSquares;
    pos.zobrist_key = undo.oldHashKey;
}


bool is_square_attacked(const Position &pos, int square, bool by_white) {
    uint64_t square_bb = 1ULL << square;
    if (by_white) {
        if (GetPawnAttacks(pos, square, false) & pos.WhitePawns) return true;
        if (GetKnightAttacks(pos, square) & pos.WhiteKnights) return true;
        if (get_bishop_attacks(square, pos.occupiedSquares) & (pos.WhiteBishops | pos.WhiteQueen)) return true;
        if (get_rook_attacks(square, pos.occupiedSquares) & (pos.WhiteRooks | pos.WhiteQueen)) return true;
        // Check if white king attacks this square
        int wking_sq = __builtin_ctzll(pos.WhiteKing);
        if (kingAttacks[wking_sq] & square_bb) return true;
    } else {
        if (GetPawnAttacks(pos, square, true) & pos.BlackPawns) return true;
        if (GetKnightAttacks(pos, square) & pos.BlackKnights) return true;
        if (get_bishop_attacks(square, pos.occupiedSquares) & (pos.BlackBishops | pos.BlackQueen)) return true;
        if (get_rook_attacks(square, pos.occupiedSquares) & (pos.BlackRooks | pos.BlackQueen)) return true;
        // Check if black king attacks this square
        int bking_sq = __builtin_ctzll(pos.BlackKing);
        if (kingAttacks[bking_sq] & square_bb) return true;
    }
    return false;
}

uint64_t GetKnightAttacks(const Position &pos, int square) {
    return knightAttacks[square];
}

uint64_t GetQueenMoves(const Position &pos, int square) {
    return get_bishop_attacks(square, pos.occupiedSquares) | get_rook_attacks(square, pos.occupiedSquares);
}

uint64_t GetKingMoves(const Position &pos) {
    int square = pos.whiteToMove ? __builtin_ctzll(pos.WhiteKing) : __builtin_ctzll(pos.BlackKing);
    return kingAttacks[square];
}

uint64_t GetPawnMoves(const Position &pos, int square, bool by_white) {
    uint64_t moves = 0;
    uint64_t bitboard = 1ULL << square;
    if (by_white) {
        if (!(pos.occupiedSquares & (bitboard << 8))) {
            moves |= (bitboard << 8);
            if ((bitboard & Rank2) && !(pos.occupiedSquares & (bitboard << 16))) {
                moves |= (bitboard << 16);
            }
        }
    } else {
        if (!(pos.occupiedSquares & (bitboard >> 8))) {
            moves |= (bitboard >> 8);
            if ((bitboard & Rank7) && !(pos.occupiedSquares & (bitboard >> 16))) {
                moves |= (bitboard >> 16);
            }
        }
    }
    return moves;
}

uint64_t GetPawnAttacks(const Position &pos, int square, bool by_white) {
    uint64_t attacks = 0;
    uint64_t bitboard = 1ULL << square;
    if (by_white) {
        if ((bitboard & ~FileA)) attacks |= (bitboard << 7);
        if ((bitboard & ~FileH)) attacks |= (bitboard << 9);
    } else {
        if ((bitboard & ~FileA)) attacks |= (bitboard >> 9);
        if ((bitboard & ~FileH)) attacks |= (bitboard >> 7);
    }
    return attacks;
}

void generate_moves(Position &pos, MoveList &move_list) {
    move_list.count = 0;
    MoveList quiet_moves;
    generate_quiet_moves(pos, quiet_moves);
    for (int i = 0; i < quiet_moves.count; i++) {
        if (move_list.count < 255) {
            move_list.moves[move_list.count++] = quiet_moves.moves[i];
        }
    }
    MoveList captures;
    generate_captures(pos, captures);
    for (int i = 0; i < captures.count; i++) {
        if (move_list.count < 255) {
            move_list.moves[move_list.count++] = captures.moves[i];
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
            uint64_t moves = GetPawnAttacks(pos, from, true) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos) & opponent_pieces;
        int from = __builtin_ctzll(pos.WhiteKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            if (move_list.count < 255) {
                move_list.moves[move_list.count++] = {from, to};
            }
            moves &= moves - 1;
        }
    } else {
        uint64_t opponent_pieces = pos.WhiteoccupiedSquares;
        pieces = pos.BlackPawns;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetPawnAttacks(pos, from, false) & opponent_pieces;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
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
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos) & opponent_pieces;
        int from = __builtin_ctzll(pos.BlackKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            if (move_list.count < 255) {
                move_list.moves[move_list.count++] = {from, to};
            }
            moves &= moves - 1;
        }
    }
}

void generate_quiet_moves(Position &pos, MoveList &move_list) {
    move_list.count = 0;
    uint64_t pieces;
    if (pos.whiteToMove) {
        uint64_t empty_or_opponent = ~pos.WhiteoccupiedSquares; // Can move to empty squares or opponent squares
        pieces = pos.WhitePawns;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetPawnMoves(pos, from, true) & empty_or_opponent & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteKnights;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from) & empty_or_opponent & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteBishops;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & empty_or_opponent & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteRooks;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & empty_or_opponent & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.WhiteQueen;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = (get_rook_attacks(from, pos.occupiedSquares) | get_bishop_attacks(from, pos.occupiedSquares)) & empty_or_opponent & ~pos.BlackoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos) & empty_or_opponent & ~pos.BlackoccupiedSquares;
        int from = __builtin_ctzll(pos.WhiteKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            if (move_list.count < 255) {
                move_list.moves[move_list.count++] = {from, to};
            }
            moves &= moves - 1;
        }
    } else {
        uint64_t empty_or_opponent = ~pos.BlackoccupiedSquares;
        pieces = pos.BlackPawns;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetPawnMoves(pos, from, false) & empty_or_opponent & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackKnights;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = GetKnightAttacks(pos, from) & empty_or_opponent & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackBishops;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_bishop_attacks(from, pos.occupiedSquares) & empty_or_opponent & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackRooks;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = get_rook_attacks(from, pos.occupiedSquares) & empty_or_opponent & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        pieces = pos.BlackQueen;
        while (pieces) {
            int from = __builtin_ctzll(pieces);
            uint64_t moves = (get_rook_attacks(from, pos.occupiedSquares) | get_bishop_attacks(from, pos.occupiedSquares)) & empty_or_opponent & ~pos.WhiteoccupiedSquares;
            while (moves) {
                int to = __builtin_ctzll(moves);
                if (move_list.count < 255) {
                    move_list.moves[move_list.count++] = {from, to};
                }
                moves &= moves - 1;
            }
            pieces &= pieces - 1;
        }
        uint64_t moves = GetKingMoves(pos) & empty_or_opponent & ~pos.WhiteoccupiedSquares;
        int from = __builtin_ctzll(pos.BlackKing);
        while (moves) {
            int to = __builtin_ctzll(moves);
            if (move_list.count < 255) {
                move_list.moves[move_list.count++] = {from, to};
            }
            moves &= moves - 1;
        }
    }
}
int make_count = 0;
int unmake_count = 0;

void reset_move_counters() {
    make_count = 0;
    unmake_count = 0;
}

void check_move_balance() {
    if (make_count != unmake_count) {
        std::cerr << "WARNING: Unbalanced make/unmake! make=" << make_count << " unmake=" << unmake_count << std::endl;
    }
}
