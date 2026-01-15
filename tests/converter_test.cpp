#include "converter.hpp"
#include <gtest/gtest.h>
#include <thread>

class TestConverter : public Converter {
public:
  using Converter::calculateNumThreads;
  using Converter::Converter;
};

TEST(ConverterTest, CalculateNumThreads) {
  TestConverter conv("input.pdf", "output.cbz");

  // Test explicit thread counts
  EXPECT_EQ(conv.calculateNumThreads(1), 1);
  EXPECT_EQ(conv.calculateNumThreads(4), 4);
  EXPECT_EQ(conv.calculateNumThreads(32), 32);

  // Test clamping
  EXPECT_EQ(conv.calculateNumThreads(64), 32);

  // Test auto-detection
  int autoThreads = conv.calculateNumThreads(0);
  EXPECT_GT(autoThreads, 0);
  EXPECT_LE(autoThreads, 32);

  EXPECT_EQ(conv.calculateNumThreads(-1), autoThreads);
}
