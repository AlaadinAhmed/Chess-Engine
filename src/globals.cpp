#include "globals.hpp"


TranspositionTable tt(700);
bool debug_mode = false; // Set debug mode to true for debugging move generation
bool ponder_mode = false;
bool own_book_enabled = false;
std::string book_path = "book.txt";
int num_threads = 1;
