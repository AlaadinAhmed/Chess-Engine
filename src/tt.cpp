#include "tt.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "utils.hpp"
#include "globals.hpp"

TranspositionTable::TranspositionTable(int size_mb) {
  log_debug("TranspositionTable constructor called with size_mb = " + std::to_string(size_mb));
  num_entries = (size_mb * 1024 * 1024) / sizeof(TTEntry);
  log_debug("num_entries = " + std::to_string(num_entries));
  table = (TTEntry *)malloc(num_entries * sizeof(TTEntry));
  if (table == nullptr) {
    std::cerr << "ERROR: malloc failed for TranspositionTable!" << std::endl;
    // Handle error, maybe throw an exception or exit
  }
  log_debug("table pointer = " + std::to_string((uintptr_t)table));
  clear();
  log_debug("TranspositionTable constructor finished.");
}

TranspositionTable::~TranspositionTable() { free(table); }

void TranspositionTable::clear() {
  log_debug("TranspositionTable::clear() called.");
  memset(table, 0, num_entries * sizeof(TTEntry));
}

TTEntry *TranspositionTable::probe(uint64_t key, bool &found) {
  log_debug("tt.probe called with key = " + std::to_string(key) + ", num_entries = " + std::to_string(num_entries));
  int index = key % num_entries;
  log_debug("calculated index = " + std::to_string(index));
  log_debug("Accessing table[index].key");
  if (table[index].key == key) {
    found = true;
  } else {
    found = false;
  }
  return &table[index];
}

void TranspositionTable::save(uint64_t key, int depth, HashFlag flag,
                            int score, Move best_move) {
  int index = key % num_entries;
  table[index].key = key;
  table[index].depth = depth;
  table[index].flag = flag;
  table[index].score = score;
  table[index].best_move = best_move;
}
