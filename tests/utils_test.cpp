#include "utils.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

TEST(ProgressBarTest, Update) {
  ProgressBar bar;

  // We can't easily test the visual output without mocking std::cout,
  // but we can at least ensure it doesn't crash and handles edge cases.

  // Capturing stdout to avoid cluttering test output
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

  bar.update(0, 100);
  bar.update(50, 100);
  bar.update(100, 100);

  std::cout.rdbuf(old);

  std::string output = buffer.str();
  EXPECT_FALSE(output.empty());
  EXPECT_TRUE(output.find("0 %") != std::string::npos);
  EXPECT_TRUE(output.find("50 %") != std::string::npos);
  EXPECT_TRUE(output.find("100 %") != std::string::npos);
}
