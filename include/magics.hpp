#pragma once
#include "position.hpp"

void init_magics();
uint64_t get_rook_attacks(int square, uint64_t occupancy);
uint64_t get_bishop_attacks(int square, uint64_t occupancy);