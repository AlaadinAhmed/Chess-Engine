#include "utils.hpp"
#include <iostream>
#include "globals.hpp"

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
