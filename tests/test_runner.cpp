#include "gtest/gtest.h"
#include "magics.hpp"
#include "movegen.hpp"
#include "hash.hpp"

int main(int argc, char **argv) {
  init_magics();
  initKingAttacks();
  zkey.initKeys();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
