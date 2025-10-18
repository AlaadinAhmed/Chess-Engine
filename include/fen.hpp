/**
 * @file fen.hpp
 * @brief Contains functions for parsing FEN strings.
 */

#pragma once
#include "position.hpp"
#include <string>

/**
 * @brief Parses a FEN string and sets the position on the board.
 *
 * @param board The board to set the position on.
 * @param fen The FEN string.
 */
void parseFEN(Position &board, std::string fen);
