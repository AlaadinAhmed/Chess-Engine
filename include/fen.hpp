/**
 * @file fen.hpp
 * @brief Contains functions for parsing FEN strings.
 * @ingroup board_representation
 */

#pragma once
#include "position.hpp"
#include <string>

const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * @brief Parses a FEN string and sets the position on the board.
 *
 * @param pos The board to set the position on.
 * @param fen The FEN string.
 */
void parseFEN(Position &pos, std::string fen = defaultFEN);
void positionToFEN(const Position &pos, char *fen_string);