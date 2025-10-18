#pragma once
#include "position.hpp"
#include <cstdint>

enum HashFlag {
  HASH_FLAG_EXACT,
  HASH_FLAG_ALPHA, // Lower bound
  HASH_FLAG_BETA   // Upper bound
};

struct TTEntry {
  uint64_t key;      // Zobrist key
  int depth;         // Depth of the search
  HashFlag flag;     // Flag for the score type
  int score;         // Score of the position
  Move best_move;    // Best move found
};

class TranspositionTable {
public:
  TranspositionTable(int size_mb);
  ~TranspositionTable();

  void clear();
  TTEntry *probe(uint64_t key, bool &found);
  void save(uint64_t key, int depth, HashFlag flag, int score, Move best_move);

private:
  TTEntry *table;
  int num_entries;
};
