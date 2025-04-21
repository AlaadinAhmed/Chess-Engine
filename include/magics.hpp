#pragma once
#include <cstdint>
#include "magics.hpp"
// lookup bishop attacks 
uint64_t get_bishop_attacks(int square, uint64_t occupancy);

// lookup rook attacks 
uint64_t get_rook_attacks(int square, uint64_t occupancy);
