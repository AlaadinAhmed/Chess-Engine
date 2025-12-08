#pragma once
#include "position.hpp"
#include <string>
#include <unordered_map>

class Book {
public:
    bool load_from_file(const std::string& filename);
    Move get_move(const Position& pos);

private:
    std::unordered_map<uint64_t, Move> moves;
};

extern Book opening_book;
