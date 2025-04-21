#pragma once
#include <string>
#include "position.hpp"
std::string const defaultFEN = "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR w KQkq - 0 1";

void parseFEN(Position &pos,std::string fen);