#include "gtest/gtest.h"
#include "utils.hpp"

TEST(UtilsTest, UciToMove) {
    Move move = uci_to_move("e2e4");
    ASSERT_EQ(move.from, 12);
    ASSERT_EQ(move.to, 28);
}

TEST(UtilsTest, MoveToUci) {
    Move move = {12, 28};
    std::string uci_move = move_to_uci(move);
    ASSERT_EQ(uci_move, "e2e4");
}
