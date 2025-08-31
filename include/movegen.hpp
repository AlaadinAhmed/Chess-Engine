#pragma once
#include "position.hpp"
// uint64_t GetPawnPromotions(Position pos);
// bool IsMoveLegal(Position pos, uint8_t from, uint8_t to);
uint64_t peekAttackedSquares (Position pos);
uint64_t GetPawnMoves(Position pos, int square);
uint64_t GetPawnAttacks(Position pos, int square);
uint64_t GetKingMoves (Position pos, uint64_t taboo);
uint64_t GetKnightMoves (Position pos);
uint64_t GetKnightAttacks (Position pos, int square);