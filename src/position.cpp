#include "position.hpp"
#include "fen.hpp"
#include "bitboard.hpp"
#include "globals.hpp"
#include "hash.hpp"

Pieces get_piece_at(const Position &pos, int sq) {
    uint64_t bb = 1ULL << sq;
    Pieces piece = NO_PIECE;
    if (pos.WhitePawns & bb) piece = W_PAWN;
    else if (pos.WhiteKnights & bb) piece = W_KNIGHT;
    else if (pos.WhiteBishops & bb) piece = W_BISHOP;
    else if (pos.WhiteRooks & bb) piece = W_ROOK;
    else if (pos.WhiteQueen & bb) piece = W_QUEEN;
    else if (pos.WhiteKing & bb) piece = W_KING;
    else if (pos.BlackPawns & bb) piece = B_PAWN;
    else if (pos.BlackKnights & bb) piece = B_KNIGHT;
    else if (pos.BlackBishops & bb) piece = B_BISHOP;
    else if (pos.BlackRooks & bb) piece = B_ROOK;
    else if (pos.BlackQueen & bb) piece = B_QUEEN;
    else if (pos.BlackKing & bb) piece = B_KING;

    return piece;
}

void Position::setStartingPosition() {
    reset();
    setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

#include "eval.hpp"

void Position::setFen(const std::string& fen_string) {
    parseFEN(*this, fen_string);
    zobrist_key = calculate_initial_hash(*this);
    
    // Initialize incremental scores
    psq_score_mg = 0;
    psq_score_eg = 0;
    evaluate_material_and_pst(*this, psq_score_mg, psq_score_eg);
}

void Position::make_null_move() {
    // A null move just switches the side to move and updates the Zobrist key.
    // The en passant square is also cleared.
    if (enPassant != 0) {
        zobrist_key ^= zkey.epKeys[enPassant % 8];
        enPassant = 0;
    }
    zobrist_key ^= zkey.sideKey;
    whiteToMove = !whiteToMove;
}

void Position::unmake_null_move(uint64_t saved_ep) {
    // Restore side to move
    whiteToMove = !whiteToMove;
    zobrist_key ^= zkey.sideKey;

    // Restore en passant square
    if (saved_ep != 0) {
        enPassant = saved_ep;
        int ep_sq = __builtin_ctzll(enPassant);
        zobrist_key ^= zkey.epKeys[ep_sq % 8];
    }
}

bool Position::has_non_pawn_material(bool side) const {
    if (side) { // White
        return (WhiteKnights | WhiteBishops | WhiteRooks | WhiteQueen) != 0;
    } else { // Black
        return (BlackKnights | BlackBishops | BlackRooks | BlackQueen) != 0;
    }
}