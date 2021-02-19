#include <gtest/gtest.h>

TEST(sample_test_case, sample_test)
{
    EXPECT_EQ(1, 1);
}

auto main(int argc, char **argv) -> int
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}