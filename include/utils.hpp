#pragma once
#include "position.hpp"
#include <string>

Move uci_to_move(std::string uci_move);
std::string move_to_uci(Move move);
