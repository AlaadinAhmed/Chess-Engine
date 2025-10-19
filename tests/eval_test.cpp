#include "gtest/gtest.h"
#include "eval.hpp"
#include "fen.hpp"

TEST(EvalTest, MaterialAdvantage) {
    Position board;
    // White up a queen
    parseFEN(board, "rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    int score = evaluate(board);
    ASSERT_GT(score, 700);
}


