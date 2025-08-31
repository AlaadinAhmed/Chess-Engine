#pragma once
#include <iostream>
#include "position.hpp"
std::string const defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void parseFEN(Position &pos,std::string fen);