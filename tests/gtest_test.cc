#include "gtest/gtest.h"

TEST(GTestTest, Assertion) { ASSERT_EQ(0, 0); }

TEST(GTestTest, Expectation) { EXPECT_EQ(1, 1); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
