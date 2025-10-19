#include "position.hpp"
#include "fen.hpp"
#include "bitboard.hpp"
#include "globals.hpp"

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

void Position::setFen(const std::string& fen_string) {
    parseFEN(*this, fen_string);
}