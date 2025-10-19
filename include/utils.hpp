/**
 * @file utils.hpp
 * @brief Contains utility functions.
 */

#pragma once
#include "position.hpp"
#include <string>


void log_debug(const std::string& message);

// Other utility functions...
/**
 * @brief Converts a UCI move string to a Move struct.
 *
 * @param uci_move The UCI move string.
 * @return The Move struct.
 */
Move uci_to_move(std::string uci_move);

/**
 * @brief Converts a Move struct to a UCI move string.
 *
 * @param move The Move struct.
 * @return The UCI move string.
 */
std::string move_to_uci(Move move);

// Opening book helper
std::string get_book_move(const Position &pos, const std::string &book_file_path);