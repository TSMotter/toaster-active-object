#include <gtest/gtest.h>

TEST(HelloWorldTest, HelloWorldAssertions)
{
    EXPECT_STRNE("hello", "world");
}