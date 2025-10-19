#include "gtest/gtest.h"
#include "position.hpp"
#include "search.hpp"
#include "movegen.hpp"
#include "globals.hpp"
#include "utils.hpp"

TEST(IllegalMoveTest, ReproduceBug) {
    Position pos;
    pos.setStartingPosition();
    Move best_move;
    search(pos, 6, -1, best_move);
    ASSERT_NE(best_move.from, 0);
    ASSERT_NE(best_move.to, 0);
}