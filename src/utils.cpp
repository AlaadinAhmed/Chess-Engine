#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "globals.hpp"
#include "fen.hpp"

Move uci_to_move(std::string uci_move) {
    int from_file = uci_move[0] - 'a';
    int from_rank = uci_move[1] - '1';
    int to_file = uci_move[2] - 'a';
    int to_rank = uci_move[3] - '1';
    int from = from_rank * 8 + from_file;
    int to = to_rank * 8 + to_file;
    return {from, to};
}

std::string move_to_uci(Move move) {
    std::string uci_move = "";
    int from_file = move.from % 8;
    int from_rank = move.from / 8;
    int to_file = move.to % 8;
    int to_rank = move.to / 8;
    uci_move += (char)('a' + from_file);
    uci_move += (char)('1' + from_rank);
    uci_move += (char)('a' + to_file);
    uci_move += (char)('1' + to_rank);
    return uci_move;
}

void log_debug(const std::string& message) {
    if (debug_mode) {
        std::cout << "info string Debug: " << message << std::endl;
    }
}

// Simple text-based opening book: lines of "<fen>;<uci>"
std::string get_book_move(const Position &pos, const std::string &book_file_path) {
    std::ifstream fin(book_file_path);
    if (!fin.is_open()) {
        return std::string();
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string fen_str, uci;
        if (!std::getline(ss, fen_str, ';')) continue;
        if (!std::getline(ss, uci)) continue;
        Position cmp;
        parseFEN(cmp, fen_str);
        if (cmp.WhitePawns == pos.WhitePawns && cmp.BlackPawns == pos.BlackPawns &&
            cmp.WhiteKnights == pos.WhiteKnights && cmp.BlackKnights == pos.BlackKnights &&
            cmp.WhiteBishops == pos.WhiteBishops && cmp.BlackBishops == pos.BlackBishops &&
            cmp.WhiteRooks == pos.WhiteRooks && cmp.BlackRooks == pos.BlackRooks &&
            cmp.WhiteQueen == pos.WhiteQueen && cmp.BlackQueen == pos.BlackQueen &&
            cmp.WhiteKing == pos.WhiteKing && cmp.BlackKing == pos.BlackKing &&
            cmp.whiteToMove == pos.whiteToMove && cmp.castelingRights == pos.castelingRights &&
            cmp.enPassant == pos.enPassant) return uci;
    }
    return std::string();
}
