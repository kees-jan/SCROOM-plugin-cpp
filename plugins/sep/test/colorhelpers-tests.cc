#include <gtest/gtest.h>

#include "../colorconfig/CustomColorHelpers.hh"

TEST(ColorHelpers_Tests, colorHelpers_toUint8_lower) { // NOLINT
  int32_t value = -1;
  CustomColorHelpers helpers;
  EXPECT_EQ(helpers.toUint8(value), 0);
}

TEST(ColorHelpers_Tests, colorHelpers_toUint8_upper) { // NOLINT
  int32_t value = 420;
  CustomColorHelpers helpers;
  EXPECT_EQ(helpers.toUint8(value), 255);
}

TEST(ColorHelpers_Tests, colorHelpers_toUint8_inrange) { // NOLINT
  int32_t value = 42;
  CustomColorHelpers helpers;
  EXPECT_EQ(helpers.toUint8(value), 42);
}

TEST(ColorHelpers_Tests, colorHelpers_calculateCMYK) { // NOLINT
  CustomColor::Ptr color(new CustomColor("test", 1, 1, 1, 1));
  CustomColorHelpers helpers;
  int16_t c = 1;
  helpers.calculateCMYK(color, c, c, c, c, 1);
  EXPECT_EQ(c, 5);
}

TEST(ColorHelpers_Tests, colorHelpers_calculateCMYKall) { // NOLINT
  CustomColor::Ptr color(new CustomColor("test", 1, 1, 1, 1));
  CustomColorHelpers helpers;
  int16_t c = 1;
  int16_t m = 2;
  int16_t y = 3;
  int16_t k = 4;
  helpers.calculateCMYK(color, c, m, y, k, 1);
  EXPECT_EQ(c, 2);
  EXPECT_EQ(m, 3);
  EXPECT_EQ(y, 4);
  EXPECT_EQ(k, 5);
}