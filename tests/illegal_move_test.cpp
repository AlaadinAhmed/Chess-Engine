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
    printf("Best move: from %d to %d\n", best_move.from, best_move.to);
    printf("Best move UCI: %s\n", move_to_uci(best_move).c_str());
    ASSERT_NE(best_move.from, 0);
    ASSERT_NE(best_move.to, 0);
}