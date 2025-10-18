#include "eval.hpp"

int evaluate(Position &pos) {
    int score = 0;
    score += __builtin_popcountll(pos.WhitePawns) * 100;
    score += __builtin_popcountll(pos.WhiteKnights) * 300;
    score += __builtin_popcountll(pos.WhiteBishops) * 320;
    score += __builtin_popcountll(pos.WhiteRooks) * 500;
    score += __builtin_popcountll(pos.WhiteQueen) * 900;
    score -= __builtin_popcountll(pos.BlackPawns) * 100;
    score -= __builtin_popcountll(pos.BlackKnights) * 300;
    score -= __builtin_popcountll(pos.BlackBishops) * 320;
    score -= __builtin_popcountll(pos.BlackRooks) * 500;
    score -= __builtin_popcountll(pos.BlackQueen) * 900;
    return pos.whiteToMove ? score : -score;
}
