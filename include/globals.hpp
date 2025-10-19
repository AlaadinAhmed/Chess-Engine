#pragma once

#include <cstdint>
#include "position.hpp" // Include position.hpp for Move and Pieces
#include "tt.hpp"




extern TranspositionTable tt;
extern thread_local int history_ply;
extern bool debug_mode;
extern bool ponder_mode;
extern bool own_book_enabled;
extern std::string book_path;
extern int num_threads;
