#include "book.hpp"
#include "fen.hpp"
#include "hash.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Book opening_book;

bool Book::load_from_file(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        // std::cerr << "Warning: Could not open book file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    int count = 0;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string fen_str, uci;
        if (!std::getline(ss, fen_str, ';')) continue;
        if (!std::getline(ss, uci)) continue;
        
        Position pos;
        parseFEN(pos, fen_str);
        uint64_t key = calculate_initial_hash(pos);
        
        moves[key] = uci_to_move(uci);
        count++;
    }
    std::cout << "Loaded " << count << " moves from opening book." << std::endl;
    return true;
}

Move Book::get_move(const Position& pos) {
    if (moves.find(pos.zobrist_key) != moves.end()) {
        return moves[pos.zobrist_key];
    }
    return {};
}
