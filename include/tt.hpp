/**
 * @file tt.hpp
 * @brief Contains the TranspositionTable class and related enums and structs.
 */

#pragma once
#include "position.hpp"
#include <cstdint>

/**
 * @brief Enum for the hash flag.
 */
enum HashFlag {
  HASH_FLAG_EXACT, /**< The score is an exact value. */
  HASH_FLAG_ALPHA, /**< The score is a lower bound. */
  HASH_FLAG_BETA   /**< The score is an upper bound. */
};

/**
 * @brief Struct for a transposition table entry.
 */
struct TTEntry {
  uint64_t key;      /**< The Zobrist key. */
  int depth;         /**< The depth of the search. */
  HashFlag flag;     /**< The flag for the score type. */
  int score;         /**< The score of the position. */
  Move best_move;    /**< The best move found. */
};

/**
 * @brief Class for the transposition table.
 */
class TranspositionTable {
public:
  /**
   * @brief Constructs a new TranspositionTable object.
   *
   * @param size_mb The size of the table in megabytes.
   */
  TranspositionTable(int size_mb);

  /**
   * @brief Destroys the TranspositionTable object.
   */
  ~TranspositionTable();

  /**
   * @brief Clears the transposition table.
   */
  void clear();

  /**
   * @brief Probes the transposition table for a given key.
   *
   * @param key The Zobrist key.
   * @param found True if the entry was found, false otherwise.
   * @return A pointer to the transposition table entry.
   */
  TTEntry *probe(uint64_t key, bool &found);

  /**
   * @brief Saves a new entry in the transposition table.
   *
   * @param key The Zobrist key.
   * @param depth The depth of the search.
   * @param flag The flag for the score type.
   * @param score The score of the position.
   * @param best_move The best move found.
   */
  void save(uint64_t key, int depth, HashFlag flag, int score, Move best_move);

private:
  TTEntry *table;      /**< The transposition table. */
  int num_entries; /**< The number of entries in the table. */
};