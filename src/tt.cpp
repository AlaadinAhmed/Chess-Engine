#include "tt.hpp"
#include <cstdlib>
#include <cstring>

TranspositionTable::TranspositionTable(int size_mb) {
  num_entries = (size_mb * 1024 * 1024) / sizeof(TTEntry);
  table = (TTEntry *)malloc(num_entries * sizeof(TTEntry));
  clear();
}

TranspositionTable::~TranspositionTable() { free(table); }

void TranspositionTable::clear() {
  memset(table, 0, num_entries * sizeof(TTEntry));
}

TTEntry *TranspositionTable::probe(uint64_t key, bool &found) {
  int index = key % num_entries;
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
