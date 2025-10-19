#pragma once

#include <cstdint>
#include "position.hpp" // Include position.hpp for Move and Pieces
#include "tt.hpp"




extern TranspositionTable tt;
extern int history_ply;
extern bool debug_mode;
extern bool ponder_mode;
