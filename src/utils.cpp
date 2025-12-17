#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "globals.hpp"
#include "fen.hpp"
#include "bitboard.hpp"
#include "magics.hpp"
#include "movegen.hpp"

Move uci_to_move(std::string uci_move) {
    int from_file = uci_move[0] - 'a';
    int from_rank = uci_move[1] - '1';
    int to_file = uci_move[2] - 'a';
    int to_rank = uci_move[3] - '1';
    int from = from_rank * 8 + from_file;
    int to = to_rank * 8 + to_file;
    
    Pieces promotion = NO_PIECE;
    if (uci_move.length() == 5) {
        // Determine color based on destination rank
        bool is_white = (to_rank == 7); // White promotes on rank 8 (index 7)
        char promo_char = uci_move[4];
        if (promo_char == 'q') promotion = is_white ? W_QUEEN : B_QUEEN;
        else if (promo_char == 'r') promotion = is_white ? W_ROOK : B_ROOK;
        else if (promo_char == 'b') promotion = is_white ? W_BISHOP : B_BISHOP;
        else if (promo_char == 'n') promotion = is_white ? W_KNIGHT : B_KNIGHT;
    }
    
    return {from, to, promotion};
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
    
    if (move.promotion != NO_PIECE) {
        switch (move.promotion) {
            case W_QUEEN: case B_QUEEN: uci_move += 'q'; break;
            case W_ROOK: case B_ROOK: uci_move += 'r'; break;
            case W_BISHOP: case B_BISHOP: uci_move += 'b'; break;
            case W_KNIGHT: case B_KNIGHT: uci_move += 'n'; break;
            default: break;
        }
    }
    
    return uci_move;
}

void log_debug(const std::string& message) {
    if (debug_mode) {
        std::ofstream log_file("debug.log", std::ios_base::app);
        if (log_file.is_open()) {
            log_file << message << std::endl;
        }
        std::cout << "info string Debug: " << message << std::endl;
    }
}

// Simple text-based opening book: lines of "<fen>;<uci>"
// std::string get_book_move(const Position &pos, const std::string &book_file_path) {
//    // Implementation removed in favor of Book class
//    return "";
// }

// see() implementation moved to see.cpp