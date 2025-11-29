#include "globals.hpp"

TranspositionTable tt(1512); // 2^21 entries
thread_local int history_ply = 0;
bool debug_mode = false; // Set debug mode to true for debugging move generation
bool ponder_mode = false;
bool own_book_enabled = false;
std::string book_path = "book.txt";
int num_threads = 1;
UndoInfo undo_history[MAX_PLY];
