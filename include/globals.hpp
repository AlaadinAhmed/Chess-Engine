#pragma once

#include <cstdint>
#include "position.hpp" // Include position.hpp for Move and Pieces
#include "tt.hpp"
#include "movegen.hpp"

#define MAX_PLY 128
#define MATE_SCORE 1000000
#define MATE_THRESHOLD (MATE_SCORE - MAX_PLY)


extern TranspositionTable tt;
extern thread_local int history_ply;
extern bool debug_mode;
extern bool ponder_mode;
extern bool own_book_enabled;
extern std::string book_path;
extern int num_threads;
extern UndoInfo undo_history[MAX_PLY];

extern uint64_t kingAttacks[64];
extern uint64_t whitePawnAttacks[64];
extern uint64_t blackPawnAttacks[64];
extern uint64_t whitePawnMoves[64];
extern uint64_t blackPawnMoves[64];
